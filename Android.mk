INNER_SAVED_LOCAL_PATH := $(LOCAL_PATH)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := dtex2

LOCAL_C_INCLUDES  := \
	${DTEX2_SRC_PATH}/include \
	${DTEX2_SRC_PATH}/include/dtex2 \
	${SHADERLAB_SRC_PATH}/include/shaderlab \
	${PS_SRC_PATH} \
	${CU_SRC_PATH} \
	${TASKS_SRC_PATH} \
	${TEXPACK_SRC_PATH} \
	${LOGGER_SRC_PATH} \
	${GIMG_SRC_PATH} \
	${UNIRENDER_SRC_PATH}/include \

LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,,$(shell find $(LOCAL_PATH)/source/ -name "*.cpp" -print)) \

LOCAL_STATIC_LIBRARIES := \
	tasks \

include $(BUILD_STATIC_LIBRARY)

LOCAL_PATH := $(INNER_SAVED_LOCAL_PATH)