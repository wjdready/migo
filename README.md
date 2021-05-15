## MiGo 开源掌机

这是一个迷你游戏机项目，[演示视频(bilibili)](https://www.bilibili.com/video/BV1qK4y1A7zA)，其目的是打造钥匙扣大小的功能齐全的掌机，同时也希望能兼顾掌机爱好者在它上面开发自己的应用。类似于 [Arduboy](https://github.com/Arduboy/Arduboy) 这个项目，它做的真的很好，很多的游戏爱好者在它上面开发了好几百个游戏，这主要归功于其简单的特性以及支持使用Arduino来开发自己的游戏。遗憾的是，Arduboy 局限于有限的机能（主控芯片为Atmega32u4，32kb falsh + 4kb SRAM），无法做出更好玩的功能。MiGO使用ESP32作为主控芯片，这款芯片功能强大但又足够简单上手，网上资源足够多，支持Arduino编程，所以就使用它了。

### 主要功能
MiGO 都有功能都有那些呢?以下是目前正在实现或者已经实现的一些，但实际上可以有更多的:
* fc 模拟器，对这是最基本的，实测运行帧率达60帧是完全没有问题的（屏幕分辨率240*240的情况下）
* gbc模拟器 目前网上已经有人在ESP32下实现gbc模拟器了，只需拿过来使用即可；
* gba模拟器 好吧，这个可能有些困难，因为性能可能还是不够，不过如果有人能够找出优化方案或许可以呢？
* sfc模拟器 这个模拟器可以做；
* arduboy模拟器 是的，你没有看错，MiGo其实可以运行Arduboy模拟器然后玩Arduboy游戏，真是非常不错
* xxx模拟器 更多模拟器可能会实现
* 音乐播放器，这个功能已经实现了，在使用软件解码mp3格式的情况下，播放48kHZ的音乐还是没有问题的
* 时间显示，这个功能因为加入了32.768kHz的晶振
* 录音功能, 因为 MiGo 主板上安装有一个MEMS传声器，所以可以用来录音并保存到内存卡；
* 网络通话， 得益于ESP32自带的WIFI功能，结合传声器和扬声器可以实现远程的网络语音聊天
* 游戏联机, 使用蓝牙或者wifi可以在nes模拟器上玩双人游戏；
* 网络收音机，通过wifi来连接Web Radio，这个已经实现了，让MiGO可以当不错的老人机；
* 网络音乐，已经有人通过esp32来播放网易云音乐，那么 MiGO 也一定行;
* 播放视频, 或许这个有一些困难，主要在于解码速率如何，不过值得一式；
* 远程投屏，这个还是挺有意思的，在电脑端搭建一个TCP服务器，MiGO通过WIFI接收电脑的屏幕信息，这个可以让MiGO和电脑互动。
* 无线手柄，MiGO可以通过蓝牙或者wifi来连接电脑来控制电脑上的游戏；
* 连接蓝牙耳机，虽然MiGO有一个3.5mm的耳机插孔，但是还可以通过蓝牙连接像airpod这样的蓝牙耳机；
* 网络电子书浏览器，利用LVGL可以实现一个非常精美的小说阅读器
* ftp文件服务器，web服务器 让MiGO连接上网站，并作为一个服务器，利用内网穿透，将你的网站发布的英特网；
* 英语词典，英语学习机，这个还可以用来学习，没想到吧；
* ... 太多, 省略

### Example
下面是MiGO运行fc模拟器的一个示意图:
![fc模拟器](./doc/image.jpg)
看起来不错，小巧可爱的！但其实可以做的更小，但那就意味这电池容量不能做大太大，游玩时间缩短了。注意按键下面那个孔，它是用来录音的。旁边留有钥匙孔，可以穿钥匙扣。

### 文件说明
hardware 是硬件部分，里面包含了可以打印的3D外壳和Altium Designer工程文件，工程文件包含主板的整个工程文件和原理图，另外没有AD的话，里面的gerberfile_v4.0.zip可以直接发给PCB厂家打样,3D外壳可以使用3D打印机打印出外壳。

代码文件主要包含在两个文件夹，sdk是MiGO应用开发的软件开发工具包，里面包含MiGO的硬件驱动和UI库，应用通过调用SDK的接口来实现各种功能而不用关系底层问题。migo文件夹是各种应用所在的目录。你也可以自己创建一个文件夹来创建应用，只需要在makefile 中指定sdk组件的目录即可。
```makefile
EXTRA_COMPONENT_DIRS := ../sdk
```