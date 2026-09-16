# Dumper7 CppGenerator crash scope

- `auth`: 用户在本地 Dumper7 工作区中明确要求诊断并修复 Dump/CppGenerator 崩溃。
- `in_scope`: `D:\Github\Dumper7` 源码、该源码生成的 Debug/Release DLL 与 PDB、AION2 本机崩溃包、`C:\Dumper-7` 本机生成结果。
- `out_of_scope`: 游戏服务端、网络接口、账号数据、反作弊绕过、远程主机与第三方系统。
- `network_profile`: offline；分析与验证均为本地只读取证和本地编译，没有网络请求。
- `write_scope`: 仅修改 Dumper7 工作区源码、构建产物和本报告；没有修改游戏安装目录。

