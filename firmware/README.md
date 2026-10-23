# Arduino 固件说明

本目录以原始材料中独立顶层 `ros_arduino_bridge/ros_arduino_firmware/src/libraries/ROSArduinoBridge4wd/` 为基线，不是 `bishe/src/ros_arduino_bridge` 的另一份副本。作者已确认目标硬件为 Arduino UNO R3 + Sensor Shield V5、L298N 和四路编码器。

固件源自 ROSArduinoBridge，保留上游 BSD 版权，见 [`THIRD_PARTY_LICENSE_BSD.txt`](../THIRD_PARTY_LICENSE_BSD.txt)。工作空间副本与独立副本的历史差异见 [`PID_WORKSPACE_DIFF.patch`](../../docs/PID_WORKSPACE_DIFF.patch)；V0.4 到 v1.0.0 的固件修改见 [`V04_TO_V1_FIRMWARE.patch`](../../docs/V04_TO_V1_FIRMWARE.patch)。

公开版的源码级改动：

1. `READ_ENCODERS` 遵循上位机双值单行回复。
2. ROS 配置不再占用 D13 LED 输出。
3. PID 增益改为浮点类型，在线更新支持小数并检查参数数量。
4. 串口参数缓冲区增加边界，避免历史代码继续越界写入。

这些改动尚未完成 Arduino Uno 编译、烧录、串口联调或带载验证。上电前必须阅读 [`HARDWARE_AND_PROTOCOL_AUDIT.md`](../../docs/HARDWARE_AND_PROTOCOL_AUDIT.md)。
