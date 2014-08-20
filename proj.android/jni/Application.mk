APP_STL := gnustl_static
APP_CFLAGS := -std=c99 -DPH_DISTRO_$(DISTRO)=1

APP_CPPFLAGS := -std=c++11 -frtti -fexceptions# -Wc++11-narrowing
#NDK_TOOLCHAIN_VERSION=clang
NDK_TOOLCHAIN_VERSION=4.7

# to minimize build time, comment armeabi-v7a abi
APP_OPTIM := release
#APP_OPTIM := debug

ifeq ($(APP_OPTIM),debug)
	APP_CFLAGS += -g
	APP_CPPFLAGS += -g
	APP_LDFLAGS += -g
else
	APP_CFLAGS += -O3 -Os --fast-math -fstrict-aliasing
	APP_CPPFLAGS += -O3 -Os --fast-math -fstrict-aliasing

	# Even smaller binary size, may invalidate some inline functions
	#APP_CPPFLAGS += -ffunction-sections -fdata-sections
	#APP_LDFLAGS += -Wl,--gc-sections
endif
APP_ABI := armeabi #armeabi-v7a
APP_PLATFORM := android-9
