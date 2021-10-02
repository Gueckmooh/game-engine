.SECONDEXPANSION:

MODULE:=window

SRC_PATH=$(shell pwd)/src

SOURCE_FILES:=$(shell find $(SRC_PATH) -maxdepth 1 -name "*.cpp" -print)
SOURCE_FILES+=$(shell find $(SRC_PATH)/impl/linux -maxdepth 1 -name "*.cpp" -print)
LDFLAGS:=-lxcb -lxcb-xkb -lxkbcommon -lxkbcommon-x11 -lxcb-image -lm

HEADERS_EXPORT_DIR=window

include /home/brignone/dev/sandbox/ge-build/config/makerules/common.mk
