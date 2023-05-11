# canfd测试

## 设置波特率
```
sudo ip link set can0 down
sudo ip link set up can0 type can bitrate 500000
```
## 发送数据
```
cansend can0 123#5A5A5A5A5A5A5A5A
```