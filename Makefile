.SECONDEXPANSION:

QUIET ?= 1

ifeq ($(QUIET),1)
QAT=@
NQAT=
else
QAT=
endif

TOOLS_DIR=tools/modules
TOOLS_BIN_DIR=$(TOOLS_DIR)/bin

ROOT=$(shell pwd)
MODULES_DIR=$(ROOT)/modules
CONFIG_DIR=$(ROOT)/config

GEN_MODULE=$(TOOLS_BIN_DIR)/gen-module-makefile

XML_MODULE_FILES=$(shell find $(MODULES_DIR) -name "*.xml" -print)
XML_MODULE_FILES:=$(subst $(MODULES_DIR)/,,$(XML_MODULE_FILES))

MK_MODULE_DIR=$(CONFIG_DIR)/makefiles
MK_MODULE_FILES=$(addprefix $(MK_MODULE_DIR)/,$(XML_MODULE_FILES:.xml=.mk))

# FOR MODULE DEPENDANCIES
MK_MD_MODULE_DIR:=$(MK_MODULE_DIR)/md
MK_MD_MODULE_FILES=$(addprefix $(MK_MD_MODULE_DIR)/,$(XML_MODULE_FILES:.xml=.mk))

MAKE=sbmake

.PHONY: all
all: prebuild main

.PHONY: main
main: main-build

.PHONY: main-build
main-build: main-deps
	$(QAT)$(MAKE) --no-print-directory -C src/main build

# .PHONY: main-prebuild
# main-prebuild: main-deps
# 	$(QAT)$(MAKE) --no-print-directory -C src/main prebuild

.PHONY: main-deps
main-deps:
	$(QAT)$(MAKE) --no-print-directory -C src/main dependancies

.PHONY: prebuild
prebuild: modules_makefiles

$(GEN_MODULE):
	$(QAT)$(MAKE) --no-print-directory -C $(TOOLS_DIR)

.PHONY: modules_makefiles
modules_makefiles: $(GEN_MODULE) $(MK_MODULE_FILES) $(MK_MD_MODULE_FILES)


$(MK_MODULE_DIR)/%.mk: $(MODULES_DIR)/%.xml $$(@D)/.f
	$(GEN_MODULE) $< $@

$(MK_MD_MODULE_DIR)/%.mk: $(MODULES_DIR)/%.xml $$(@D)/.f
	$(GEN_MODULE) $< $@ -deps

.PRECIOUS: %/.f
%/.f:
	$(QAT)mkdir -p $(dir $@)
	$(QAT)touch $@

.PHONY: mrproper
mrproper:
	rm -rf build
