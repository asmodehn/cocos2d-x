LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := cocos_lua_static

LOCAL_MODULE_FILENAME := liblua

LOCAL_SRC_FILES := manual/CCLuaBridge.cpp \
          manual/CCLuaEngine.cpp \
          manual/CCLuaStack.cpp \
          manual/lua_debugger.c \
          manual/CCLuaValue.cpp \
          manual/Cocos2dxLuaLoader.cpp \
          manual/CCBProxy.cpp \
          manual/Lua_web_socket.cpp \
          manual/LuaOpengl.cpp \
          manual/LuaScriptHandlerMgr.cpp \
          manual/LuaBasicConversions.cpp \
          manual/LuaSkeletonAnimation.cpp \
          manual/lua_cocos2dx_manual.cpp \
          manual/lua_cocos2dx_extension_manual.cpp \
          manual/lua_cocos2dx_coco_studio_manual.cpp \
          manual/lua_cocos2dx_ui_manual.cpp \
          manual/lua_cocos2dx_spine_manual.cpp \
          manual/lua_cocos2dx_physics_manual.cpp \
          manual/lua_cocos2dx_experimental_manual.cpp \
          manual/lua_cocos2dx_experimental_video_manual.cpp \
          manual/lua_cocos2dx_deprecated.cpp \
          manual/lua_xml_http_request.cpp \
          manual/platform/android/CCLuaJavaBridge.cpp \
          manual/platform/android/jni/Java_org_cocos2dx_lib_Cocos2dxLuaJavaBridge.cpp \
          manual/tolua_fix.cpp \
          manual/lua_extensions.c \
          auto/lua_cocos2dx_auto.cpp \
          auto/lua_cocos2dx_extension_auto.cpp \
          auto/lua_cocos2dx_studio_auto.cpp \
          auto/lua_cocos2dx_ui_auto.cpp \
          auto/lua_cocos2dx_spine_auto.cpp \
          auto/lua_cocos2dx_physics_auto.cpp \
          auto/lua_cocos2dx_experimental_auto.cpp \
          auto/lua_cocos2dx_experimental_video_auto.cpp \
          ../../../external-bin/lua/tolua/tolua_event.c \
          ../../../external-bin/lua/tolua/tolua_is.c \
          ../../../external-bin/lua/tolua/tolua_map.c \
          ../../../external-bin/lua/tolua/tolua_push.c \
          ../../../external-bin/lua/tolua/tolua_to.c \
          ../../../external-bin/lua/luasocket/auxiliar.c \
          ../../../external-bin/lua/luasocket/buffer.c \
          ../../../external-bin/lua/luasocket/except.c \
          ../../../external-bin/lua/luasocket/inet.c \
          ../../../external-bin/lua/luasocket/io.c \
          ../../../external-bin/lua/luasocket/luasocket.c \
          ../../../external-bin/lua/luasocket/mime.c \
          ../../../external-bin/lua/luasocket/options.c \
          ../../../external-bin/lua/luasocket/select.c \
          ../../../external-bin/lua/luasocket/serial.c \
          ../../../external-bin/lua/luasocket/tcp.c \
          ../../../external-bin/lua/luasocket/timeout.c \
          ../../../external-bin/lua/luasocket/udp.c \
          ../../../external-bin/lua/luasocket/unix.c \
          ../../../external-bin/lua/luasocket/usocket.c \
          ../../../external/xxtea/xxtea.cpp


LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../external-bin/lua/tolua \
                    $(LOCAL_PATH)/../../../external-bin/lua/luajit/include \
                    $(LOCAL_PATH)/../../../external-bin/lua \
                    $(LOCAL_PATH)/../../../extensions \
                    $(LOCAL_PATH)/../../editor-support/spine \
                    $(LOCAL_PATH)/../../editor-support/cocosbuilder \
                    $(LOCAL_PATH)/../../editor-support/cocostudio \
                    $(LOCAL_PATH)/../../ui \
                    $(LOCAL_PATH)/../../2d \
                    $(LOCAL_PATH)/../../3d \
                    $(LOCAL_PATH)/auto \
                    $(LOCAL_PATH)/manual \
                    $(LOCAL_PATH)/manual/platform/android \
                    $(LOCAL_PATH)/manual/platform/android/jni \
                    $(LOCAL_PATH)/../../../external/xxtea


LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../external-bin/lua/tolua \
                           $(LOCAL_PATH)/../../../external-bin/lua/luajit/include \
                           $(LOCAL_PATH)/../external \
                           $(LOCAL_PATH)/../external-bin \
                           $(LOCAL_PATH)/auto \
                           $(LOCAL_PATH)/manual


LOCAL_WHOLE_STATIC_LIBRARIES := luajit_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += websockets_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_network_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosbuilder_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocostudio_static
LOCAL_WHOLE_STATIC_LIBRARIES += spine_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static

LOCAL_CFLAGS += -Wno-psabi
LOCAL_EXPORT_CFLAGS += -Wno-psabi

include $(BUILD_STATIC_LIBRARY)

$(call import-module,lua/luajit/prebuilt/android)
$(call import-module,extensions)
$(call import-module,.)
$(call import-module,websockets/prebuilt/android)
$(call import-module,network)
$(call import-module,editor-support/cocostudio)
$(call import-module,editor-support/cocosbuilder)
$(call import-module,editor-support/spine)
$(call import-module,audio/android)
