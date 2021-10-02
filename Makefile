.SECONDEXPANSION:

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


.PHONY: prebuild
prebuild: modules_makefiles



$(GEN_MODULE):
	$(MAKE) -C $(TOOLS_DIR)

.PHONY: modules_makefiles
modules_makefiles: $(GEN_MODULE) $(MK_MODULE_FILES)


$(MK_MODULE_DIR)/%.mk: $(MODULES_DIR)/%.xml $$(@D)/.f
	$(GEN_MODULE) $< $@

.PRECIOUS: %/.f
%/.f:
	$(QAT)mkdir -p $(dir $@)
	$(QAT)touch $@

