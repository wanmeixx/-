# Dumper7 CppGenerator 崩溃分析报告

> 分析日期：2026-08-26  
> 目标：AION2 定制 Dumper7 的 CppGenerator 生成链路  
> 状态：源码修复与构建已完成；游戏内端到端复测待执行

## 1. 范围与结论

授权与边界见 [scope](work/dumper7-cppgenerator-crash/scope.md)。本次仅分析本地源码、崩溃包和生成结果，没有修改游戏安装目录或访问网络。

问题由两个连续暴露的缺陷组成：原始流程先写入耗时约 90 秒的 GObjects 文本，再使用此前缓存的 UObject 索引，导致名称冲突生成阶段解引用已被 GC 清空的槽位；第一处修复后，`PropertyFixup` 又对所有属性执行不必要的完整类型解析，晚加载的 Struct/Enum 不在初始化映射中，从而由 `unordered_map::at()` 抛出异常。现已将原始对象 Dump 移到所有生成器之后，并把碰撞名称所需数据快照化；未知属性扫描不再解析完整类型，晚加载对象使用安全查找和名称回退。

## 2. Evidence

### E-001：原始 UE 访问违规调用栈

- `observed_at`: 2026-08-26 11:26 +08:00
- `source_type`: file
- `source_ref`: `%LOCALAPPDATA%\AION2\Saved_TW\Crashes\UECC-Windows-3ECCD2444C55DB4E37483C9D5C34BDE4_0000\CrashContext.runtime-xml`
- `content_hash`: `F00C891F5791ABC3A08FD705FBEBCCF1E5AECB074AF709877E8E716337A4C4A4`
- `repro_command`:

```powershell
$xml = Join-Path $env:LOCALAPPDATA 'AION2\Saved_TW\Crashes\UECC-Windows-3ECCD2444C55DB4E37483C9D5C34BDE4_0000\CrashContext.runtime-xml'
[xml]$crash = Get-Content -LiteralPath $xml -Raw
$crash.FGenericCrashContext.RuntimeProperties.ErrorMessage
$crash.FGenericCrashContext.RuntimeProperties.PCallStack
```

- `raw_excerpt`: `EXCEPTION_ACCESS_VIOLATION reading address 0x0000000000000010`；符号化链路为 `UEObject::GetClass → UEObject::IsA → CppGenerator::GenerateNameCollisionsInl → CppGenerator::Generate`。

### E-002：第二次失败停在 PropertyFixup

- `observed_at`: 2026-08-26 11:40 +08:00
- `source_type`: file/log
- `source_ref`: `C:\Dumper-7\5.3.2-0+++UE5+Release-5.3.2-AION2_20260826_1140\CppSDK_OLD\PropertyFixup.hpp`
- `content_hash`: `43E599446ACE1BDEB74A18F9CF74AF3076CD5909A21E09CD28AD46AE40597A49`
- `repro_command`:

```powershell
$out = 'C:\Dumper-7\5.3.2-0+++UE5+Release-5.3.2-AION2_20260826_1140\CppSDK_OLD'
Get-ChildItem -LiteralPath $out -File | Select-Object Name,Length,LastWriteTime
Get-Content -LiteralPath (Join-Path $out 'PropertyFixup.hpp') -Raw
```

- `raw_excerpt`: 目录只有完整 `SDK.hpp` 与仅含文件头的 `PropertyFixup.hpp`，尚无 `NameCollisions.inl`；控制台连续输出 `CppGenerator failed: invalid unordered_map<K, T> key`。

### E-003：修复后的构建产物

- `observed_at`: 2026-08-26 11:49 +08:00
- `source_type`: command/file
- `source_ref`: `x64\Debug\version.dll`、`x64\Release\version.dll`
- `content_hash`: Debug `EA7617F4F91EE38F8EC3263948CBCE02BFFE6FE9010E1F5A6049D10F193E7B1A`；Release `57842E9E23FBDAA25E97832AF385BF6F7F7A9E6A8690EC5EDE0D7C482BE1CB04`
- `repro_command`:

```powershell
$msbuild = 'D:\Program Files\Microsoft Visual Studio\18\Enterprise\MSBuild\Current\Bin\MSBuild.exe'
& $msbuild 'Version-Dumper-7.sln' /m /t:Build /p:Configuration=Debug /p:Platform=x64 /nologo /v:quiet
& $msbuild 'Version-Dumper-7.sln' /m /t:Build /p:Configuration=Release /p:Platform=x64 /nologo /v:quiet
Get-FileHash -Algorithm SHA256 'x64\Debug\version.dll','x64\Release\version.dll'
git diff --check
```

- `raw_excerpt`: Debug 与 Release 均返回退出码 `0`；`git diff --check` 返回 `0`。

## 3. Findings

### F-001：生成阶段使用失效 UObject 索引

- `severity`: n/a_re
- `category`: design
- `status`: validated
- `evidence_ids`: `[E-001]`
- `confidence`: high
- `location`: `Generator.h` 的生成顺序、`CppGenerator.cpp:GenerateNameCollisionsInl`
- `impact`: 游戏 GC/异步加载可在耗时的 GObjects 文本写入期间清空缓存槽位，随后 `Struct.IsA()` 读取空指针偏移 `0x10`，直接导致游戏进程崩溃。
- `remediation`: GObjects 文本移到最后一个生成器完成后再写；名称碰撞生成使用初始化时缓存的包、名称和类型信息，不再重新解引用 UObject。

### F-002：PropertyFixup 的全类型解析触发缺键异常

- `severity`: n/a_re
- `category`: design
- `status`: validated
- `evidence_ids`: `[E-002, E-003]`
- `confidence`: high
- `location`: `CppGenerator.cpp:GetUnknownProperties`
- `impact`: 晚加载 Struct/Enum 不在初始化映射中，包装器通过 `unordered_map::at()` 查询时抛出 `invalid unordered_map<K, T> key`；10 次重试重复同一确定性扫描并延长游戏暴露于竞态的时间。
- `remediation`: 未知属性扫描仅比较 `EClassCastFlags`；Struct/Enum 查找改用 `find()`，缺失信息时回退到实时类型名；成员碰撞和依赖遍历对缺失快照安全降级。

### F-003：修复仍需游戏内端到端确认

- `severity`: info
- `category`: other
- `status`: candidate
- `evidence_ids`: `[E-003]`
- `confidence`: high
- `location`: AION2 运行时 Dump 流程
- `impact`: 静态审计和本地构建不能模拟 UE GC、异步加载及实际对象规模。
- `remediation`: 用新 DLL 启动一次完整 Dump，确认控制台出现 `CppGenerator completed successfully`，并检查 `NameCollisions.inl`、`UnrealContainers.hpp`、包文件以及最后生成的 GObjects 文本均非截断状态。

## 4. Path

### P-001：崩溃与修复调用路径

- `path_type`: callflow
- `start`: `GenerateWithRetry<CppGenerator>`
- `goal`: 稳定生成 SDK 并在最后写入原始对象列表
- `steps`:
  1. `Generator::InitInternal` 建立 Struct/Enum/Package 索引快照 — evidence: E-001 — finding: F-001
  2. 旧流程先长时间写 GObjects，UE GC 使部分索引失效 — evidence: E-001 — finding: F-001
  3. `GenerateNameCollisionsInl` 解引用空槽位并访问违规 — evidence: E-001 — finding: F-001
  4. 第一处修复后，`GetUnknownProperties` 的完整类型解析查询晚加载对象并抛出缺键 — evidence: E-002 — finding: F-002
  5. 新流程使用快照、安全查找和轻量属性分类，并把 GObjects 写入移到末尾 — evidence: E-003 — finding: F-002
- `residual_risks`: 尚缺一次真实 AION2 进程内的完整 Dump 验证。

## 5. 复测标准

1. 将本次 Debug 或 Release `version.dll` 与同配置 PDB 保留在一起，按现有方式加载游戏。
2. 触发一次 Dump；不应再出现 `invalid unordered_map<K, T> key`，也不应在 `GenerateNameCollisionsInl` 崩溃。
3. `CppSDK` 中至少应存在闭合的 `PropertyFixup.hpp`、非空 `NameCollisions.inl`、`UnrealContainers.hpp`、`UtfN.hpp`、`SDK/Basic.hpp` 和包文件。
4. 控制台应依次报告各生成器成功；`GObjects-Dump*.txt` 应在最后阶段生成。

关键时间点见 [timeline](work/dumper7-cppgenerator-crash/timeline.md)。

