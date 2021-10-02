.SECONDEXPANSION:

QUIET ?= 1

ifeq ($(QUIET),1)
QAT=@
NQAT=
else
QAT=
endif

SHELL=/bin/bash
ECHO:=$(QAT)echo -e
PWD:=$(shell pwd)

# ROOT:=/home/brignone/dev/sandbox/ge-build
CONFIG_DIR:=$(ROOT)/config
SCRIPTS_DIR:=$(ROOT)/scripts
SRC_DIR:=$(ROOT)/src
BUILD_DIR:=$(ROOT)/build
MAKERULES_DIR:=$(CONFIG_DIR)/makerules
MAKEFILES_DIR:=$(CONFIG_DIR)/makefiles
LIB_DIR:=$(BUILD_DIR)/lib
BIN_DIR:=$(BUILD_DIR)/bin
OBJS_DIR:=$(BUILD_DIR)/objs
DEPS_DIR:=$(BUILD_DIR)/deps
INCLUDE_DIR:=$(BUILD_DIR)/include

include $(MAKEFILES_DIR)/$(MODULE).mk

MODULE_PATH:=$(SRC_DIR)/$(MODULE_DIR)
MODULE_SRC_PATH:=$(MODULE_PATH)/src
MODULE_HEADERS_PATH:=$(MODULE_PATH)/include
HEADERS_EXPORT_PATH:=$(INCLUDE_DIR)/$(HEADERS_EXPORT_DIR)

ifeq ($(OS),windows)
include $(MAKERULES_DIR)/windows.mk
endif

ifeq ($(COMPONENT_TYPE),cpp)
include $(MAKERULES_DIR)/cpp.mk
endif
ifeq ($(COMPONENT_TYPE),headers)
include $(MAKERULES_DIR)/headers.mk
endif

.PRECIOUS: %/.f
%/.f:
	$(QAT)mkdir -p $(dir $@)
	$(QAT)touch $@
