## PuzzleHero 

We first released PuzzleHero to the AppStore early 2013. Then the publisher edition launched mid-2013. Now the game is not exactly making us money so we decided, heck, why not make it open source and maybe something good will come out of it. So we are releasing the full client source code and art assets to the community.

Feel free to do whatever with the source code, but please do not use the art assets for other commercial products. See end of this note for licensing details.

The game uses cocos2d-x 2.2, and supports both iOS and Android. cocos2dx is already included in the release. The game is largely written C++ with a few lua scripts. It's a typical Puzzle & Dragons style game with a few spins and real-time PvP support. The code is not in its best style and we are aware of that, but still you should find it fairly readable.

Btw, **we are hiring** experienced **game developers/artists/designers**.
Send me email if you're interested: **a.banknote@gmail.com**

We are a small company based in Shenzhen, China. Beef hotpot is great around here.

我们是深圳的一觉手游小团队，正在招聘优秀的**程序/美术/策划**，有兴趣入伙，请邮件我们 **a.banknote@gmail.com / 723600882@qq.com** 

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

native binary:   ./proj.android/build_native.sh  --high  APP_OPTIM=debug
This always copies the necesasry assets. Remove --high if you want to build low res version.

IntelliJ project:  proj.android/
```

### RESOURCES
Resources are located in Resources/ folder. We do some post-processing and packing to the resources but we choose not elaborate it here. All resource processing scripts are located in the same folder so feel free to dig into it.

### GAME SERVER
The server code is not released but the default project connects to our server and gets you in game smoothly.

We do not provide the server code for two reasons.
1. the server setup is pretty invovled.
2. Even after the server setup, it still needs to import a dozen of excel forms to be of any use but we have no plan to open source our desgin docs.

Feel free to write your own server. All communications are in plain text json format.

### LICENSE

Copyright 2014 深圳市烁动科技有限公司.
All art resources(under Resources/ folder) are licensed under the Creative Commons Attribution-NonCommercial 4.0 International license. 
All source code excluding 3rd party code is licensed under the MIT license.
Both licesnes are included in the repository.

版权所有 2014  深圳市烁动科技有限公司。
所有美术资源是基于 Creative Commons Attribution-NonCommercial 4.0 International 许可发布。
除去第三方外的所有代码是基于 MIT License 许可发布。
