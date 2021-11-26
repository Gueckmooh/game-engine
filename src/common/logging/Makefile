MODULE_NAME=common/logging

include config.mk

include $(MAKE_INCLUDE_DIR)/base.mk

ifeq ($(TARGET_OS),linux)
SOURCE_FILES:=$(shell find $(MODULE_SOURCE_PATH) -maxdepth 1 -name "*.cpp" -print)
SOURCE_FILES+=$(shell find $(MODULE_SOURCE_PATH)/call_stack_backends/gcc -maxdepth 1 -name "*.cpp" -print)
else ifeq ($(TARGET_OS),windows)
SOURCE_FILES:=$(shell find $(MODULE_SOURCE_PATH) -maxdepth 1 -name "*.cpp" -print)
SOURCE_FILES+=$(shell find $(MODULE_SOURCE_PATH)/call_stack_backends/mingw -maxdepth 1 -name "*.cpp" -print)
endif

include $(MAKE_INCLUDE_DIR)/build_module.mk
