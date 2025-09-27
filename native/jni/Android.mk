LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := caca
LOCAL_SRC_FILES := caca.cpp

# Link against the C++ standard library (libc++)
LOCAL_LDLIBS := -lc++_shared -llog  # Add -llog to link against the Android logging library

include $(BUILD_SHARED_LIBRARY)
