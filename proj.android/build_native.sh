#!/bin/bash
source ~/.profile

# set params
NDK_ROOT=$NDK_ROOT
COCOS2DX_ROOT=$PUZZLE_HERO_ROOT/libs/

GAME_ROOT=../
GAME_ANDROID_ROOT=$GAME_ROOT/proj.android
RESOURCE_ROOT=$GAME_ROOT/Resources

APPNAME="PuzzleHero"

# options

buildexternalsfromsource=

ResType="low"

usage(){
cat << EOF
usage: $0 [options]

Build C/C++ code for $APPNAME using Android NDK

OPTIONS:
--high  use high resolution resources
--low   use low resolution resources
-h  this help
EOF
}

if test $# -gt 0; then
    case "$1" in
        --help)
            usage
            exit 0
        ;;
        --high)
            shift
            ResType="high"
        ;;
        --low)
            shift
            ResType="low"
        ;;
    esac
fi

# paths


if [ -z "${NDK_ROOT+aaa}" ];then
    echo "please define NDK_ROOT"
    exit 1
fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# ... use paths relative to current directory

APP_ROOT="$DIR/.."
APP_ANDROID_ROOT="$DIR"

echo "NDK_ROOT = $NDK_ROOT"
echo "COCOS2DX_ROOT = $COCOS2DX_ROOT"
echo "APP_ROOT = $APP_ROOT"
echo "APP_ANDROID_ROOT = $APP_ANDROID_ROOT"
echo ""

# clear old assets
if [ -d "$APP_ANDROID_ROOT"/assets ]; then
    rm -rf "$APP_ANDROID_ROOT"/assets
fi

# make sure assets directory exists
mkdir "$APP_ANDROID_ROOT"/assets

# copy resources
cp -rf "$APP_ROOT"/Resources/base.lua "$APP_ANDROID_ROOT"/assets/
cp -rf "$APP_ROOT"/Resources/bmfont "$APP_ANDROID_ROOT"/assets/
cp -rf "$APP_ROOT"/Resources/particle "$APP_ANDROID_ROOT"/assets/
cp -rf "$APP_ROOT"/Resources/ttf "$APP_ANDROID_ROOT"/assets/
mkdir "$APP_ANDROID_ROOT"/assets/boot

if [ "$ResType" = "low" ]; then
    echo "* Using LOW resolution resources"
    cp -rf "$APP_ROOT"/Resources/boot "$APP_ANDROID_ROOT"/assets/
    cp -rf "$APP_ROOT"/Resources/sound/SD/ "$APP_ANDROID_ROOT"/assets/boot/sound  # always use SD boot res
    #cp -rf "$APP_ROOT"/Resources/iphone.Head.tar "$APP_ANDROID_ROOT"/assets/
    cp "$APP_ANDROID_ROOT"/../server/static/sync/iphone.Head.tar "$APP_ANDROID_ROOT"/assets/
else if [ "$ResType" = "high" ]; then
    echo "* Using HIGH resolution resources"
    #cp -rf "$APP_ROOT"/Resources/iphonehd.Head.tar "$APP_ANDROID_ROOT"/assets/
    cp -rf "$APP_ROOT"/Resources/boot "$APP_ANDROID_ROOT"/assets/ # always use SD boot res
    cp -rf "$APP_ROOT"/Resources/sound/SD/ "$APP_ANDROID_ROOT"/assets/boot/sound  # always use SD boot res
    cp "$APP_ANDROID_ROOT"/../server/static/sync/iphonehd.Head.tar "$APP_ANDROID_ROOT"/assets/
    fi
fi

echo "* Using prebuilt thirdparty libraries"
"$NDK_ROOT"/ndk-build -j7 -C "$APP_ANDROID_ROOT" $* \
    "NDK_MODULE_PATH=${COCOS2DX_ROOT}:${COCOS2DX_ROOT}/cocos2dx/platform/third_party/android/prebuilt"


for arg in ${BASH_ARGV[*]} ; do
    if [ "$arg" = "DISTRO=OURPALM_JSCMCC" ]; then
        echo "copy 3rdparty/armeabi/libmegjb.so -> libs/armeabi/"
        cp -rf "$APP_ANDROID_ROOT"/3rdparty/armeabi/libmegjb.so "$APP_ANDROID_ROOT"/libs/armeabi/
    fi
done
