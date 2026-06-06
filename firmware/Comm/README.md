# Comm 模块说明

## 这个目录是干嘛的

`Comm` 用来放通信相关逻辑，核心是“数据怎么发出去、怎么组织格式、电脑怎么解析”。

## 建议放什么

- 串口协议
- 数据帧封装
- 文本格式输出
- 二进制格式输出
- 校验和

## 以后可以新增的文件示例

- `uart_protocol.c`
- `uart_protocol.h`
- `frame_encoder.c`
- `frame_encoder.h`

## 当前项目里的典型用途

- 发送 `ADC` 原始采样值
- 发送 `FRAME_BEGIN` / `FRAME_END`
- 后续发送算法结果和状态信息

## 一句话理解

`Comm` 负责“把系统里的信息传出去”。
