BUILD_UPSTREAM?=0

UPSTREAM_TARGETS=$(addsuffix _upstream,$(MODULE_DEPENDENCIES))

ifneq ($(BUILD_UPSTREAM),0)
build_dependencies: $(UPSTREAM_TARGETS)

INCLUDE_DEPENDENCY=1
include $(addsuffix .mk,$(addprefix $(MAKEFILES_DIR)/, $(MODULE_DEPENDENCIES)))

endif
