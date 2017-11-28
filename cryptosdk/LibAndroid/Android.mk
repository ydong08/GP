LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := DD_Crypto_static
LOCAL_MODULE_FILENAME := DDCrypto
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libDDCrypt.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_STATIC_LIBRARIES += ssl_crypto_static
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := ssl_crypto_static
LOCAL_MODULE_FILENAME := crypto
LOCAL_SRC_FILES := openssl/$(TARGET_ARCH_ABI)/libcrypto.a
include $(PREBUILT_STATIC_LIBRARY)

