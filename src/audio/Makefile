MODULE_NAME=audio

include config.mk

include $(MAKE_INCLUDE_DIR)/base.mk

ifeq ($(TARGET_OS),linux)
SOURCE_FILES:=$(shell find $(MODULE_SOURCE_PATH) -maxdepth 1 -name "*.cpp" -print)
SOURCE_FILES+=$(shell find $(MODULE_SOURCE_PATH)/backends/pulse -maxdepth 1 -name "*.cpp" -print)
# LDFLAGS+=-lpulse -lpthread
else ifeq ($(TARGET_OS),windows)
SOURCE_FILES:=$(shell find $(MODULE_SOURCE_PATH) -maxdepth 1 -name "*.cpp" -print)
SOURCE_FILES+=$(shell find $(MODULE_SOURCE_PATH)/backends/dsound -maxdepth 1 -name "*.cpp" -print)
# LDFLAGS=-L/usr/x86_64-w64-mingw32/lib
# LDFLAGS+=-lgdi32 -lxinput -lm
endif


include $(MAKE_INCLUDE_DIR)/build_module.mk
