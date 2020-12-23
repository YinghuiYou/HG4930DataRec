# HG4930DataRec
基于CSerialPort类实现了用于接收HoneyWell 4930 MEMS传感器数据的串口通信程序。
## 实现的功能有：
1.接收串口数据进行解析，
2.将解析后的数据保存到txt；
3.将解析后的数据显示到图形化界面。
4.计算帧率（每秒接收到的数据帧数量）和总共接收到的数据帧帧数（有待优化）

## 测试结果：
使用友善串口调试助手以及虚拟串口VSPD进行了测试，波特率设置为1048576（1024*1024）bit/s，
在友善串口调试助手设置为10ms的间隔发送，测试得到的帧率为91~97之间波动，和预期不符。
计算了一下每帧传输时间 2*44*8/1048576*1000 = 0.67ms , 
在程序利用<ctime>中的clock()计算串口消息响应程序的时间，每一帧的处理时间大致为4ms。
也即程序的处理上并不会对数据接收产生影响。

## 问题分析

1.串口调试助手的数据发送时间达不到这么精准的时间间隔，在此之前利用另一个串口调试助手进行测试，同样的时间间隔，帧率大致为65Hz
2.使用定时器消息来进行帧率的计算，存在一定误差，在去掉定时器的消息后，一些特定帧处理的时间有所下降。
3.数据处理的程序全部在串口消息响应函数里,可以考虑使用子线程来接收数据，但这个存在的问题是线程间的变量如何传递 

