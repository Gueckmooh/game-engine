MODULE_NAME=loop_editor/main_watcher

include config.mk

include $(MAKE_INCLUDE_DIR)/base.mk

ifeq ($(LIBRARY_KIND),shared)
CXXFLAGS+=-D__MAIN_LOOP_SHARED_LIBRARY__
endif
ifeq ($(LIBRARY_KIND),static)
CXXFLAGS+=-D__MAIN_LOOP_STATIC_LIBRARY__
endif

LDFLAGS+=-ldl

include $(MAKE_INCLUDE_DIR)/build_module.mk
