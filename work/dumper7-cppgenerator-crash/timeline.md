# Dumper7 CppGenerator crash timeline

- `2026-08-26 11:24 +08:00`：Dump 启动，控制台停在 `Attempting CppGenerator`。
- `2026-08-26 11:26 +08:00`：AION2 生成 UE 崩溃包；异常为读取地址 `0x10`。
- `2026-08-26 11:40 +08:00`：第一轮修复后复测；`CppGenerator` 连续报告 `invalid unordered_map<K, T> key`，游戏退出，未生成新的 UE/WER dump。
- `2026-08-26 11:42 +08:00`：根据截断的 `PropertyFixup.hpp` 和缺失的 `NameCollisions.inl`，将第二个异常定位到 `GeneratePropertyFixupFile()`。
- `2026-08-26 11:49 +08:00`：完成快照化与安全查找修复；Debug/Release 构建退出码均为 `0`，等待游戏内复测。

