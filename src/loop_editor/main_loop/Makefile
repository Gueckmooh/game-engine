MODULE_NAME=loop_editor/main_loop

include config.mk

include $(MAKE_INCLUDE_DIR)/base.mk

ifeq ($(LIBRARY_KIND),shared)
POST_BUILD_HOOKS+=copy_lib
.PHONY: copy_lib
copy_lib: $(LIB_DIR)/new_$(MODULE_TARGET).so
$(LIB_DIR)/new_$(MODULE_TARGET).so: $(LIB_DIR)/$(MODULE_TARGET).so
	@$(ECHO) "\tCOPYING $(LIB_DIR)/new_$(MODULE_TARGET).so"
	$(QAT)cp $(LIB_DIR)/$(MODULE_TARGET).so $(LIB_DIR)/new_$(MODULE_TARGET).so
endif

include $(MAKE_INCLUDE_DIR)/build_module.mk
