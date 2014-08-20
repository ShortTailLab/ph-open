# macros to include all files with same extension

define all-files-under
$(patsubst ./%,%, \
  $(shell cd $(LOCAL_PATH) ; \
          find $(1) -name "$(2)" -and -not -name ".*") \
 )
endef

define all-cpp-files-under
$(call all-files-under,$(1),*.cpp)
endef

# module

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

COCOS2DX_ROOT=$(LOCAL_PATH)/..
LOCAL_MODULE := cocos2dx-common
LOCAL_CFLAGS += -DANDROID
LOCAL_SRC_FILES := $(call all-cpp-files-under,src) \
	$(call all-cpp-files-under,aosp)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include \
	$(COCOS2DX_ROOT)/cocos2dx/platform/android \
	$(COCOS2DX_ROOT)/cocos2dx/platform/android/jni
LOCAL_EXPORT_LDLIBS := -llog
LOCAL_C_INCLUDES := $(LOCAL_EXPORT_C_INCLUDES) \
	$(COCOS2DX_ROOT)/cocos2dx/support/tinyxml2 \
	$(LOCAL_PATH)/aosp
LOCAL_LDLIBS := $(LOCAL_EXPORT_LDLIBS)
LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static

include $(BUILD_STATIC_LIBRARY)

$(call import-module,cocos2dx)
