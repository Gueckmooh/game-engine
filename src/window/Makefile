MODULE_NAME=window

include config.mk

include $(MAKE_INCLUDE_DIR)/base.mk

ifneq ($(OS),windows)
SOURCE_FILES:=$(shell find $(MODULE_SOURCE_PATH) -maxdepth 1 -name "*.cpp" -print)
SOURCE_FILES+=$(shell find $(MODULE_SOURCE_PATH)/backends/linux -maxdepth 1 -name "*.cpp" -print)
LDFLAGS:=-lxcb -lxcb-xkb -lxkbcommon -lxkbcommon-x11 -lxcb-image -lm
else
SOURCE_FILES:=$(shell find $(MODULE_SOURCE_PATH) -maxdepth 1 -name "*.cpp" -print)
SOURCE_FILES+=$(shell find $(MODULE_SOURCE_PATH)/backends/windows -maxdepth 1 -name "*.cpp" -print)
LDFLAGS=-L/usr/x86_64-w64-mingw32/lib
LDFLAGS+=-lgdi32 -lxinput -lm
endif


include $(MAKE_INCLUDE_DIR)/build_module.mk
