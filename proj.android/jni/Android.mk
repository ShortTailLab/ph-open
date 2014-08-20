LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := game_shared

LOCAL_MODULE_FILENAME := libgame

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
  LOCAL_ARM_NEON  := true
endif # TARGET_ARCH_ABI == armeabi-v7a

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   puzzleHero/android_cpp_sdk_lib.cpp \
                   puzzleHero/com/stl/puzzlehero/NativeCallback.cpp \
                   puzzleHero/com/stl/puzzlehero/WebViewLayerClient.cpp \
                   ../../Classes/AppDelegate.cpp \
                   ../../Classes/ResourceSyncMan.cpp \
                   ../../Classes/ControlExtensions.cpp \
                   ../../Classes/HeroProfileLayer.cpp \
                   ../../Classes/PlayerData.cpp \
                   ../../Classes/Console.cpp \
                   ../../Classes/GameScene.cpp \
                   ../../Classes/LoadingScene.cpp \
                   ../../Classes/FlowerSystem.cpp \
                   ../../Classes/ContentLayer.cpp \
                   ../../Classes/ZqlScrollView.cpp \
                   ../../Classes/ZhuanSheng.cpp \
                   ../../Classes/JiangHu.cpp  \
                   ../../Classes/XiaKe.cpp \
                   ../../Classes/BattleScene.cpp \
                   ../../Classes/Store.cpp \
                   ../../Classes/QiangHua.cpp \
                   ../../Classes/Sale.cpp \
                   ../../Classes/PageScrollView.cpp \
                   ../../Classes/ZhaoMu.cpp \
                   ../../Classes/PlayerRank.cpp \
                   ../../Classes/WashRelation.cpp \
                   ../../Classes/friend.cpp \
                   ../../Classes/Login.cpp \
                   ../../Classes/SystemInform.cpp \
                   ../../Classes/Victory.cpp \
                   ../../Classes/HeroIcon.cpp \
                   ../../Classes/UIAnimation.cpp \
                   ../../Classes/UIPresets.cpp \
                   ../../Classes/UIComponent.cpp \
                   ../../Classes/More.cpp \
                   ../../Classes/HeroSelectionLayer.cpp \
                   ../../Classes/Achievement.cpp \
                   ../../Classes/IuputLabel.cpp \
                   ../../Classes/ScreenConsole.cpp \
                   ../../Classes/ButtonSprite.cpp \
                   ../../Classes/ObjectPool.cpp \
                   ../../Classes/Tutorial.cpp \
                   ../../Classes/HomePage.cpp \
                   ../../Classes/PVPBoard.cpp \
                   ../../Classes/PVPLayer.cpp \
                   ../../Classes/PvESkill.cpp \
                   ../../Classes/PvPSkill.cpp \
                   ../../Classes/PvPData.cpp \
                   ../../Classes/PvPSearchGame.cpp \
                   ../../Classes/PvPEndGame.cpp \
                   ../../Classes/PvPUtils.cpp \
                   ../../Classes/Animation/CCCallLambda.cpp \
                   ../../Classes/Animation/PvPHourGlass.cpp \
                   ../../Classes/Animation/HealthBox.cpp \
                   ../../Classes/Board/SkillContext.cpp \
                   ../../Classes/Board/Board.cpp \
                   ../../Classes/Board/BoardControls.cpp \
                   ../../Classes/Board/Common.cpp \
                   ../../Classes/Board/Gem.cpp \
                   ../../Classes/Board/HeroTable.cpp \
                   ../../Classes/Board/LuaSkillContext.cpp \
                   ../../Classes/Board/LuaGem.cpp \
                   ../../Classes/Board/LuaGrid2D.cpp \
                   ../../Classes/Board/EnemySkill.cpp \
                   ../../Classes/Board/EnemyControl.cpp \
                   ../../Classes/Board/HeroControl.cpp \
                   ../../Classes/Board/StatusLabel.cpp \
                   ../../Classes/Board/PlayerControl.cpp \
                   ../../Classes/Board/LuaHeroControl.cpp \
                   ../../Classes/Board/LuaEnemyControl.cpp \
                   ../../Classes/Board/LuaPlayerControl.cpp \
                   ../../Classes/Board/LuaEnemySkill.cpp \
                   ../../Classes/Board/TouchMaskLayer.cpp \
                   ../../Classes/Board/TutorialBoard.cpp \
                   ../../Classes/Board/Property.cpp \
                   ../../Classes/Board/AnimPreset.cpp \
                   ../../Classes/Board/Task.cpp \
                   ../../Classes/Board/MoveTimer.cpp \
                   ../../Classes/Board/PassiveSkill.cpp \
                   ../../Classes/Board/ActiveSkill.cpp \
                   ../../Classes/Board/SerializationUtils.cpp \
                   ../../Classes/Board/GemUtils.cpp \
                   ../../Classes/Board/BuffGroup.cpp \
                   ../../Classes/network/PuzzleRPC.cpp \
                   ../../Classes/network/MessageChannel.cpp \
                   ../../Classes/network/PvPMessageHandler.cpp \
                   ../../Classes/network/EventsMessageHandler.cpp \
                   ../../Classes/network/URLHelpers.cpp \
                   ../../Classes/network/JsonParser.cpp \
                   ../../Classes/network/android/WebViewLayer_android.cpp \
                   ../../Classes/network/android/PuzzleRPC.cpp \
                   ../../Classes/extensions/network/HttpClient.cpp \
                   ../../Classes/extensions/sha1.cpp \
                   ../../Classes/extensions/utils.cpp \
                   ../../Classes/extensions/platform/android/CCNative.cpp \
                   ../../libs/librcksum/hash.c \
                   ../../libs/librcksum/range.c \
                   ../../libs/librcksum/rsum.c \
                   ../../libs/librcksum/state.c \
                   ../../libs/librcksum/android.cpp \
                   ../../libs/zsync/zmap.c \
                   ../../libs/zsync/zsync.c \
                   ../../Classes/extensions/platform/android/CCCrypto_android.cpp \
                   ../../Classes/extensions/auth/Weibo.cpp \
                   ../../Classes/extensions/auth/WeiboAuthLayer.cpp \
                   ../../Classes/network/android/Weixin_android.cpp

                   
LOCAL_C_INCLUDES := . \
                    $(LOCAL_PATH)/puzzleHero \
                    $(LOCAL_PATH)/puzzleHero/j2cpp \
                    $(LOCAL_PATH)/puzzleHero/platforms/android-9 \
                    $(LOCAL_PATH)/../../Classes \
					$(LOCAL_PATH)/../../Classes/Board \
					$(LOCAL_PATH)/../../Classes/Animation \
					$(LOCAL_PATH)/../../Classes/network \
					$(LOCAL_PATH)/../../Classes/extensions \
					$(LOCAL_PATH)/../../Classes/network/json_spirit \
					$(LOCAL_PATH)/puzzleHero/com/stl/puzzlehero/ \
					$(LOCAL_PATH)/../../libs/ \
					$(LOCAL_PATH)/../../libs/cocos2dx-common/include \
					$(LOCAL_PATH)/../../libs/zsync \
					$(LOCAL_PATH)/../../libs/librcksum \
					$(LOCAL_PATH)/../../libs/scripting/lua/cocos2dx_support \
					$(LOCAL_PATH)/../../libs/scripting/lua/lua \
					$(LOCAL_PATH)/../../libs/scripting/lua/tolua \
					$(LOCAL_PATH)/../../Classes/extensions/native


LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static cocosdenshion_static cocos_extension_static cocos_lua_static cocos2dx-common

APP_PROJECT_PATH := $(call my-dir)/../../libs/scripting/lua/proj.android
APP_MODULES := cocos_lua_static

LOCAL_LDFLAGS := $(LOCAL_PATH)/../3rdparty/armeabi/libboost_thread.a \
         	     $(LOCAL_PATH)/../3rdparty/armeabi/libboost_system.a \
                 $(LOCAL_PATH)/../3rdparty/armeabi/libboost_random.a \
                 $(LOCAL_PATH)/../3rdparty/armeabi/libboost_date_time.a \
                 $(LOCAL_PATH)/../3rdparty/armeabi/libssl.a \
                 $(LOCAL_PATH)/../3rdparty/armeabi/libcrypto.a

include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/../../libs/scripting/lua/proj.android/Android.mk

$(call import-module,CocosDenshion/android) \
$(call import-module,cocos2dx-common) \
$(call import-module,cocos2dx) \
$(call import-module,extensions)

