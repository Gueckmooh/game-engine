.SECONDEXPANSION:

CXX_MSG=$(ECHO) "\tCXX\t$(shell realpath --relative-to="$(PWD)" $<)"
CXXLD_MSG=$(ECHO) "\tCXXLD\t$(shell realpath --relative-to="$(PWD)" $<)"
GEN_MSG=$(ECHO) "\tGEN\t$(shell realpath --relative-to="$(PWD)" $@)"

-include $(shell find $(DEPS_DIR) -name "*.d" -print 2>/dev/null)

CC:=$(TARGET_ARCH)gcc
CXX:=$(TARGET_ARCH)g++

CXXSTD=-std=c++17

DLLEXT ?= so


SOURCE_FILES?=$(shell find $(MODULE_SRC_PATH) -name "*.cpp" -print)
SOURCE_FILES:=$(subst $(MODULE_SRC_PATH)/,,$(SOURCE_FILES))
MODULE_DEPS_PATH:=$(DEPS_DIR)/$(MODULE_DIR)
MODULE_OBJS_PATH:=$(OBJS_DIR)/$(MODULE_DIR)
DEP_FILES:=$(addprefix $(MODULE_DEPS_PATH)/, $(SOURCE_FILES:.cpp=.d))
OBJ_FILES:=$(addprefix $(MODULE_OBJS_PATH)/, $(SOURCE_FILES:.cpp=.o))
ifeq ($(TARGET_KIND),shared_library)
TARGET_PATH=$(LIB_DIR)/$(TARGET)
endif
ifeq ($(TARGET_KIND),executable)
TARGET_PATH=$(BIN_DIR)/$(TARGET)
endif

ifneq ($(TARGET_KIND),executable)
HEADER_FILES:=$(shell find $(MODULE_HEADERS_PATH) -type f -print)
HEADER_FILES:=$(subst $(MODULE_HEADERS_PATH)/,,$(HEADER_FILES))
EXPORTED_HEADER_FILES:=$(addprefix $(HEADERS_EXPORT_PATH)/, $(HEADER_FILES))
endif

INCLUDE_FLAGS+=-I$(INCLUDE_DIR) -I$(MODULE_SRC_PATH)

WARNING_FLAGS?=-Wall -Wextra

CXXFLAGS+=$(INCLUDE_FLAGS) $(WARNING_FLAGS)
CXXFLAGS+=$(CXXSTD)

ifeq ($(DEBUG),1)
CXXFLAGS+=-g -O0
endif

ifeq ($(TARGET_KIND),shared_library)
CXXFLAGS+=-fPIC
endif

LDFLAGS+=-L$(LIB_DIR) $(addprefix -l,$(DEPENDANCIES))

DLLEXT?=so

ifeq ($(TARGET_KIND),shared_library)
TARGET:=$(TARGET).$(DLLEXT)
endif

.DEFAULT_GOAL := build

### BUILD TARGET
.PHONY: build
build: $(TARGET_PATH)


### PREBUILD TARGET
.PHONY: prebuild
ifneq ($(TARGET_KIND),executable)
prebuild: export_headers dep_files
else
prebuild: dep_files
endif

### EXPORT TARGET FILES
.PHONY: export_headers
export_headers: $(EXPORTED_HEADER_FILES)

$(HEADERS_EXPORT_PATH)/%: $(MODULE_HEADERS_PATH)/% $$(@D)/.f
	$(GEN_MSG)
	$(QAT)$(SCRIPTS_DIR)/export_header $< $@


### DEPS FILES
.PHONY: dep_files
dep_files: $(DEP_FILES)

$(MODULE_DEPS_PATH)/%.d: $(MODULE_SRC_PATH)/%.cpp $$(@D)/.f
	$(GEN_MSG)
	$(QAT)$(CXX) $(CXXFLAGS) -MM -MT '$(subst $(MODULE_DEPS_PATH),$(MODULE_OBJS_PATH),$(@:.d=.o))' $< -o $@
	$(QAT)awk -i inplace -f $(SCRIPTS_DIR)/sanitize_deps.awk $@


### OBJECT FILES
.PHONY: object_files
object_files: $(OBJ_FILES)

$(MODULE_OBJS_PATH)/%.o: $(MODULE_SRC_PATH)/%.cpp $$(@D)/.f
	$(CXX_MSG)
	$(QAT)$(CXX) $(CXXFLAGS) $< -c -o $@


### TARGET FILE
$(TARGET_PATH): $(OBJ_FILES) $$(@D)/.f
	$(CXXLD_MSG)
ifeq ($(TARGET_KIND),shared_library)
	$(QAT)$(CXX) -shared -o $@ $(OBJ_FILES) $(LDFLAGS)
endif
ifeq ($(TARGET_KIND),executable)
	$(QAT)$(CXX) -o $@ $(OBJ_FILES) $(LDFLAGS)
endif
