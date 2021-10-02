.SECONDEXPANSION:

CXX_MSG=$(ECHO) "\tCXX\t$(shell realpath --relative-to="$(PWD)" $<)"
CXXLD_MSG=$(ECHO) "\tCXXLD\t$(shell realpath --relative-to="$(PWD)" $<)"
GEN_MSG=$(ECHO) "\tGEN\t$(shell realpath --relative-to="$(PWD)" $@)"

-include $(shell find $(DEPS_DIR) -name "*.d" -print 2>/dev/null)

CC:=$(TARGET_ARCH)gcc
CXX:=$(TARGET_ARCH)g++

CXXSTD=-std=c++17

DLLEXT ?= so


MODULE_DEPS_PATH:=$(DEPS_DIR)/$(MODULE_DIR)
MODULE_OBJS_PATH:=$(OBJS_DIR)/$(MODULE_DIR)
TARGET_PATH=$(LIB_DIR)/$(TARGET)

HEADER_FILES:=$(shell find $(MODULE_HEADERS_PATH) -type f -print)
HEADER_FILES:=$(subst $(MODULE_HEADERS_PATH)/,,$(HEADER_FILES))
EXPORTED_HEADER_FILES:=$(addprefix $(HEADERS_EXPORT_PATH)/, $(HEADER_FILES))

INCLUDE_FLAGS+=-I$(INCLUDE_DIR) -I$(MODULE_SRC_PATH)

CXXFLAGS+=$(INCLUDE_FLAGS)
CXXFLAGS+=$(CXXSTD)

ifeq ($(TARGET_KIND),shared_library)
CXXFLAGS+=-fPIC
endif

LDFLAGS+=

DLLEXT?=so

ifeq ($(TARGET_KIND),shared_library)
TARGET:=$(TARGET).$(DLLEXT)
endif

### BUILD TARGET
.PHONY: build
build: prebuild


### PREBUILD TARGET
.PHONY: prebuild
prebuild: export_headers

### EXPORT TARGET FILES
.PHONY: export_headers
export_headers: $(EXPORTED_HEADER_FILES)

$(HEADERS_EXPORT_PATH)/%: $(MODULE_HEADERS_PATH)/% $$(@D)/.f
	$(GEN_MSG)
	$(QAT)$(SCRIPTS_DIR)/export_header $< $@
