.SECONDEXPANSION:

-include $(shell find $(DEPS_DIR) -name "*.d" -print 2>/dev/null)

############################## CONFIG ##############################
### COMPUTE OPTIONS
CC:=$(TARGET_ARCH)gcc
CXX:=$(TARGET_ARCH)g++
AR:=ar

CXXSTD=-std=c++20

INCLUDE_FLAGS+=-I$(INCLUDE_DIR) -I$(MODULE_SRC_PATH)
WARNING_FLAGS?=-Wall -Wextra
CXXFLAGS+=$(INCLUDE_FLAGS) $(WARNING_FLAGS)
CXXFLAGS+=$(CXXSTD)

ifeq ($(MODULE_TARGET_KIND),shared_library)
ifeq ($(LIBRARY_KIND),shared)
CXXFLAGS+=-D__TARGET_SHARED_LIBRARY__
endif
ifeq ($(LIBRARY_KIND),static)
CXXFLAGS+=-D__TARGET_STATIC_LIBRARY__
endif
endif

ifeq ($(TARGET_OS),linux)
CXXFLAGS+=-D__TARGET_LINUX__
else ifeq ($(TARGET_OS),windows)
CXXFLAGS+=-D__TARGET_WINDOWS__
endif


ifeq ($(DEBUG),1)
CXXFLAGS+=-g -O0
else
CXXFLAGS+=-O3
endif

ifeq ($(MODULE_TARGET_KIND),shared_library)
ifeq ($(LIBRARY_KIND),shared)
CXXFLAGS+=-fPIC									#@todo change that
endif
endif

LDFLAGS+=$(addprefix -L,$(LIB_DIR)) $(addprefix -l,$(MODULE_LIB_DEPENDENCIES))

CXX_MSG=$(ECHO) "\tCXX\t$(shell realpath --relative-to="$(ROOT)" $@)"
CXXLD_MSG=$(ECHO) "\tCXXLD\t$(shell realpath --relative-to="$(ROOT)" $@)"
AR_MSG=$(ECHO) "\tAR\t$(shell realpath --relative-to="$(ROOT)" $@)"
GEN_MSG=$(ECHO) "\tGEN\t$(shell realpath --relative-to="$(ROOT)" $@)"

### COMPUTE FILES
ifneq ($(MODULE_TARGET_KIND),headers_only)
SOURCE_FILES?=$(shell find $(MODULE_SOURCE_PATH) -name "*.cpp" -print)
SOURCE_FILES:=$(subst $(MODULE_SOURCE_PATH)/,,$(SOURCE_FILES))
else
SOURCE_FILES?=
endif
MODULE_DEPS_PATH:=$(DEPS_DIR)/$(MODULE_BASE_DIR)
MODULE_OBJS_PATH:=$(OBJS_DIR)/$(MODULE_BASE_DIR)
DEP_FILES:=$(addprefix $(MODULE_DEPS_PATH)/, $(SOURCE_FILES:.cpp=.d))
OBJ_FILES:=$(addprefix $(MODULE_OBJS_PATH)/, $(SOURCE_FILES:.cpp=.o))

ifneq ($(MODULE_TARGET_KIND),executable)
HEADER_FILES?=$(shell find $(MODULE_HEADERS_PATH) -type f -print)
HEADER_FILES:=$(subst $(MODULE_HEADERS_PATH)/,,$(HEADER_FILES))
EXPORTED_HEADER_FILES:=$(addprefix $(HEADERS_EXPORT_PATH)/, $(HEADER_FILES))
endif


############################## BUILD TARGETS ##############################
### EXPORT HEADER FILES
.PHONY: export_headers
export_headers: $(EXPORTED_HEADER_FILES)

$(HEADERS_EXPORT_PATH)/%: $(MODULE_HEADERS_PATH)/% $$(@D)/.f
	$(GEN_MSG)
	$(QAT)$(SCRIPTS_DIR)/export_header $< $@

### DEPS FILES
.PHONY: dep_files
dep_files: $(DEP_FILES)

$(MODULE_DEPS_PATH)/%.d: $(MODULE_SOURCE_PATH)/%.cpp $$(@D)/.f
	$(GEN_MSG)
	$(QAT)$(CXX) $(CXXFLAGS) -MM -MT '$(subst $(MODULE_DEPS_PATH),$(MODULE_OBJS_PATH),$(@:.d=.o))' $< -o $@
	$(QAT)awk -i inplace -f $(SCRIPTS_DIR)/sanitize_deps.awk $@

### OBJECT FILES
.PHONY: object_files
object_files: $(OBJ_FILES)

$(MODULE_OBJS_PATH)/%.o: $(MODULE_SOURCE_PATH)/%.cpp $$(@D)/.f
	$(CXX_MSG)
	$(QAT)$(CXX) $(CXXFLAGS) $< -c -o $@

### TARGET FILE
ifeq ($(MODULE_TARGET_KIND),executable)
EXEC_NAME:=$(MODULE_TARGET)
$(BIN_DIR)/$(EXEC_NAME): $(OBJ_FILES) $$(@D)/.f
	$(CXXLD_MSG)
	$(QAT)$(CXX) -o $@ $(OBJ_FILES) $(LDFLAGS)
endif

$(LIB_DIR)/%.so: $(OBJ_FILES) $$(@D)/.f
	$(CXXLD_MSG)
	$(QAT)$(CXX) -shared -o $@ $(OBJ_FILES) $(LDFLAGS)

$(LIB_DIR)/%.a: $(OBJ_FILES) $$(@D)/.f
	$(AR_MSG)
	$(QAT)$(AR) rcs $@ $(OBJ_FILES)

##### BUILD TARGETS
.PHONY: prebuild
prebuild: dep_files export_headers

.PHONY: build_dependencies
build_dependencies: ;

.PHONY: build
build:: build_dependencies
build:: real_build

ifneq ($(POST_BUILD_HOOKS),)
build:: $(POST_BUILD_HOOKS)
endif

.PHONY: real_build
ifeq ($(MODULE_TARGET_KIND),executable)
real_build: $(BIN_DIR)/$(MODULE_TARGET)
endif
ifeq ($(MODULE_TARGET_KIND),shared_library)
ifeq ($(LIBRARY_KIND),shared)
real_build: $(LIB_DIR)/$(MODULE_TARGET).so
endif
ifeq ($(LIBRARY_KIND),static)
real_build: $(LIB_DIR)/$(MODULE_TARGET).a
endif
endif
