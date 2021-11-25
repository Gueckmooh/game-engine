MODULE_NAME=common/logging

include config.mk

include $(MAKE_INCLUDE_DIR)/base.mk

ifneq ($(OS),windows)
SOURCE_FILES:=$(shell find $(MODULE_SOURCE_PATH) -maxdepth 1 -name "*.cpp" -print)
SOURCE_FILES+=$(shell find $(MODULE_SOURCE_PATH)/call_stack_backends/gcc -maxdepth 1 -name "*.cpp" -print)
# LDFLAGS+=-lpulse -lpthread
else
SOURCE_FILES:=$(shell find $(MODULE_SOURCE_PATH) -maxdepth 1 -name "*.cpp" -print)
SOURCE_FILES+=$(shell find $(MODULE_SOURCE_PATH)/call_stack_backends/mingw -maxdepth 1 -name "*.cpp" -print)
# LDFLAGS=-L/usr/x86_64-w64-mingw32/lib
# LDFLAGS+=-lgdi32 -lxinput -lm
endif

include $(MAKE_INCLUDE_DIR)/build_module.mk
