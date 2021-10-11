.SECONDEXPANSION:

MODULE:=window

SRC_PATH=$(shell pwd)/src

ifneq ($(OS),windows)
SOURCE_FILES:=$(shell find $(SRC_PATH) -maxdepth 1 -name "*.cpp" -print)
SOURCE_FILES+=$(shell find $(SRC_PATH)/backends/linux -maxdepth 1 -name "*.cpp" -print)
LDFLAGS:=-lxcb -lxcb-xkb -lxkbcommon -lxkbcommon-x11 -lxcb-image -lm
else
SOURCE_FILES:=$(shell find $(SRC_PATH) -maxdepth 1 -name "*.cpp" -print)
SOURCE_FILES+=$(shell find $(SRC_PATH)/backends/windows -maxdepth 1 -name "*.cpp" -print)
LDFLAGS=-L/usr/x86_64-w64-mingw32/lib
LDFLAGS+=-lgdi32 -lxinput -lm
endif

HEADERS_EXPORT_DIR=window

include $(MAKE_INCLUDE_DIR)/common.mk
