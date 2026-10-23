# 下位机与 ROS 桥接审查

状态：2026-10-23 静态审查；未通电、未烧录、未实车运行。

## 已修复但未实机验证

### 编码器回复格式

历史固件对 `READ_ENCODERS` 先返回 `左前 右前`，再多发一行后轮数据；上游 `ros_arduino_python` 的 `get_encoder_counts()` 只接收两个数。V0.4 已收敛为单行两列，后两路计数仍在 Arduino 内用于四路 PID。

### D13 复用

`motor_driver.h` 将 D13 用作右电机方向输出，历史 ROS 参数又把 D13 配置为 LED 输出。V0.4 已删除额外 LED 输出配置；UNO 板载 LED 与启动电平的物理影响仍需检查。

### PID 小数精度

V0.4 使用 `int Kd = 8.835`、`int Ki = 0.9`，实际分别变为 8 和 0；在线调参也用 `atoi`。v1.0.0 将四个增益改为 `float`、使用 `atof` 并限制为恰好四个参数。此修复只消除数据类型错误，不代表参数已经重新整定。

### ROS 本地路径

`test_amcl_rviz.launch` 的两个 include 写成 `$(find daohang)launch/...`。v1.0.0 补上 `/`，并由验证脚本检查本地包引用。

## 仍需硬件确认

- 配置文件使用 `/dev/ttyUSB0`，论文示例为 `/dev/ttyACM0`；必须以实际设备枚举或 udev 规则为准。
- `wheel_diameter: 0.044`、`wheel_track: 0.167`、`encoder_resolution: 1040` 来自历史归档，不可跨电机/平台直接复用。
- 后轮只使用 PWM 使能，方向逻辑依赖前后轮接线与机械安装，必须逐路验证。
- 四路编码器依赖 UNO 的 AVR PCINT 寄存器和 D2-D5/A0-A1/A4-A5 映射。
- `AUTO_STOP_INTERVAL=2200 ms` 是软件超时停止，不是经认证的急停；仍需物理断电和限流手段。
- 历史 `ros_arduino_python` 面向 Python 2 时代，不应默认兼容 ROS Noetic/Python 3。

## 安全复现顺序

先断开电机动力完成编译和串口协议测试，再核对供电/共地/引脚；随后架空轮胎、限流、逐路低速测试和故障断连测试，最后才进入地面导航。任何阶段都不能用论文历史演示替代本次实测。
