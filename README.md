![Image](logo.png?raw=true)
## 《群侠转》- 开放源代码和美术资源

《群侠转》是深圳烁动在2013年初上线的一款三消卡牌游戏。现在我们决定把游戏代码和美术资源开放给大家，欢迎随意修改，自搭私服。《群侠转》的质量和完成度在目前开源的手游项目应该算数一数二了。不过由于项目一直比较赶，又是多人合作写的，代码风格水准不入眼还请轻喷。

注意！我司正在积极寻找牛逼的程序/美术/策划，欢迎投递简历至:   
y@shorttaillab.com / 657235341@qq.com

相对于原版，我们做了一些调整：
* 关闭了全部的付费接口。
* 渠道SDK，原来有几十个，开源版中全部删除了，感觉真是好清爽！
* 第三方登陆因为要用到我们自己的账号秘钥，所以全部关闭了，需要的填上自己的秘钥就能用。
* 现在编译运行可以直接连接运营中的服务器，方便大家试玩。游戏没有开放服务器代码，因为配置需要导入大量的策划表格，这些不放出服务器也用不了。

游戏是基于Cocos2dx-2.2的，还有若干第三方库，都已包含在打包内。下载后可以直接编译iOS和Android版本。

游戏代码是基于MIT LICESNE发布，可以随意修改随意使用。  
美术资源是基于CC Non-Commercial发布，请勿用作其他商业用途。

祝大家撸的愉快！

再次，手游求职的朋友们，欢迎来信：  y@shorttaillab.com / 657235341@qq.com

## PuzzleHero 

We first released PuzzleHero to the AppStore in early 2013. Then the publisher edition came mid-2013. Now the game is not exactly making us money so we decided, heck, why not make it open source. At least it's good for the karma, and hopefully something good will come out of it. So we are releasing the full client source code and art assets to the community.

Feel free to do whatever with the source code, but please do not use the art assets in other commercial products. See end of this note for licensing details.

The game uses Cocos2d-x 2.2 and supports both iOS and Android. Cocos2d-x is already included in the release. The game is largely written in C++ with a few lua scripts. It's a typical Puzzle & Dragons style game with a few spins and real-time PvP support. The code is not in its best style and we are aware of that, but still, you should find it fairly readable.

Btw, we are a mobile game studio based in Shenzhen, China.   
We are **hiring** experienced game **developers/artists/designers**.   
Send me an email if you're interested: **a.banknote@gmail.com** or  **723600882@qq.com**

### BUILD THE GAME

For iOS
```
Xcode:   proj.ios/PuzzleHero.xcodeproj
Target:  PuzzleHeroApple
```

For Android
```
Require android SDK and NDK.
We use android-ndk-r8e for this project.

native binary:   proj.android/build_native.sh  --high  APP_OPTIM=debug
This always copies the necesasry assets. Remove --high if you want to build low res version.

IntelliJ project:  proj.android/
You need to make a new configuration to launch the game module.
```

### RESOURCES
Resources are located in Resources/ folder. We do some post-processing to the resources but we will not elaborate the steps here. All resource processing scripts are located in the same folder so feel free to dig into it.

### GAME SERVER
The server code is not released but the default project connects to our server and gets you in game smoothly.

We do not provide the server code for two reasons.
1. the server setup is pretty invovled.  
2. Even after the server setup, it still needs to import a dozen excel forms to be of any use but we have no plan to open source our desgin docs.  

Feel free to write your own server. All communications are in plain text json format.

### LICENSE

Copyright 2014 深圳市烁动科技有限公司.    
All art resources(under Resources/ folder) are licensed under the Creative Commons Attribution-NonCommercial 4.0 International license.    
All source code excluding 3rd party code is licensed under the MIT license.    
Both licesnes are included in the repository.    
    
版权所有 2014  深圳市烁动科技有限公司。    
所有美术资源是基于 Creative Commons Attribution-NonCommercial 4.0 International 许可发布。    
除去第三方外的所有代码是基于 MIT License 许可发布。    
