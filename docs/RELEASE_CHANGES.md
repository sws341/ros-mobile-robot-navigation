# v1.0.0 发布变更

v1.0.0 以交接包中的 V0.4 候选仓库为基础，保留已确认的独立 `ros_arduino_bridge/ROSArduinoBridge4wd` 固件来源和 Arduino UNO R3 + Sensor Shield V5 硬件事实。

## V0.4 已有的两项修订

1. `arduino_params.yaml` 禁用 D13 `arduino_led`，避免与 `RIGHT_MOTOR_FORWARD = 13` 冲突。
2. `READ_ENCODERS` 只向上位机返回左右两列、单行数据；后两路编码器继续用于下位机四路 PID。

## v1.0.0 新增的源码修订

1. `diff_controller.h`：`Kp/Kd/Ki/Ko` 从 `int` 改为 `float`，避免 `8.835 → 8`、`0.9 → 0` 的静默截断。
2. `ROSArduinoBridge4wd.ino`：`UPDATE_PID` 改用 `atof` 解析四个浮点参数，拒绝参数数量错误；串口参数缓冲区扩大并增加写入边界。
3. `test_amcl_rviz.launch`：修复两个 `$(find daohang)` 后缺少 `/` 的 include 路径。
4. 三个 ROS 包：补齐维护者 `Zhanpeng Fan <sws341@users.noreply.github.com>`、版本 `1.0.0`、MIT、仓库 URL 和实际运行依赖。

## 发布与合规

- 明确 MIT、第三方 BSD 和论文图片的不同许可边界，增加 `NOTICE` 与 `CITATION.cff`。
- README 重新组织为平台区分、系统架构、真实实验记录、复现和验证状态。
- 验证脚本扩展到 package/launch/YAML、内部链接、图片签名、私有文件、占位符、绝对用户路径和关键固件规则。
- 增加 GitHub Actions 静态检查，移除 V0.4 中仅供发布前使用的临时脚本和过期清单。

所有源码修改均为静态审查结果，**尚未完成 Arduino 编译、串口联调或实机复测**；V0.4 交接包由作者私下保留，公开仓库同时提供 [`V04_TO_V1_FIRMWARE.patch`](V04_TO_V1_FIRMWARE.patch) 供逐行追溯。
