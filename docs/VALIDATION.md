# v1.0.0 验证记录

验证日期：2026-10-23。此记录严格区分静态检查、构建和实机验证。

| 项目 | 结果 | 能证明什么 |
|---|---|---|
| 19 个 ROS Launch XML | 通过本地 XML 解析与本地 `$(find ...)` 路径检查 | 语法和本仓库内引用存在；不代表节点可启动 |
| 3 个 `package.xml` | 通过 XML、维护者、许可证和仓库 URL 检查 | 发布元数据不再是占位符 |
| 7 个 YAML、2 组地图 | 通过 PyYAML 解析与相对图片路径检查 | 文件格式和地图图片存在；不证明参数/TF 正确 |
| README 本地链接、12 张图片 | 通过路径和 PNG/JPEG 文件签名检查 | Git 仓库内目标存在；发布后仍需核对 GitHub 渲染 |
| 12 张图片与论文 DOCX 媒体 | 像素尺寸与 RGBA 像素逐一一致 | 图片内容来自论文原图；文件哈希因移除元数据/重存储而不同 |
| 私有文件/敏感文本/绝对用户路径 | 规则扫描通过 | 未发现常见来源包、办公文档、密钥赋值或用户目录路径 |
| `READ_ENCODERS`、D13、PID 类型 | 源码规则检查通过 | v1.0.0 的预期修改存在；不证明串口和电机行为正确 |
| ROS/catkin 构建 | **未执行** | 当前 Windows 环境没有可用 ROS 1，WSL 访问被拒绝 |
| Arduino UNO 编译 | **未执行** | 当前环境没有 Arduino CLI/AVR 工具链 |
| 串口联调、上电、带载、导航 | **未执行** | 无新实机结论 |

本地静态检查命令：

```bash
python3 -m pip install -r requirements-dev.txt
python3 scripts/validate_repo.py
```

GitHub Actions 会在 push 和 pull request 上运行相同脚本。静态检查通过不能替代以下复现步骤：

1. 在隔离的兼容 ROS 1 环境完成 `catkin_make`。
2. 断开电机动力，使用 Arduino Uno 目标板编译固件并检查串口命令/回复。
3. 核对供电、共地、D13、PWM、编码器 PCINT、电机方向和急停。
4. 架空车轮完成逐路低速测试、失联停车与 PID 重新整定。
5. 最后在安全场地复测里程计、SLAM、AMCL 和 Move Base。
