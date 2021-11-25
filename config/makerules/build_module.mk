.SECONDEXPANSION:

include $(MAKE_INCLUDE_DIR)/base.mk

ifneq ($(MODULE_TARGET_KIND),headers_only)
.DEFAULT_GOAL := build
else
.DEFAULT_GOAL := prebuild
endif

ifneq ($(MODULE_TARGET_KIND),headers_only)
all: prebuild build check
else
all: prebuild check
endif

ifeq ($(TARGET_OS),windows)
include $(MAKERULES_DIR)/windows.mk
endif

ifeq ($(MODULE_TYPE),cpp)
include $(MAKERULES_DIR)/cpp.mk
endif
ifeq ($(MODULE_TYPE),headers)
include $(MAKERULES_DIR)/headers.mk
endif

include $(MAKERULES_DIR)/build_upstream.mk

include $(MAKERULES_DIR)/unittest.mk
