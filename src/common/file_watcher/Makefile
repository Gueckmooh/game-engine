MODULE_NAME=common/file_watcher

include config.mk

include $(MAKE_INCLUDE_DIR)/base.mk

ifneq ($(OS),windows)
	CXXFLAGS+=-D__FILE_WATCHER_USE_INOTIFY__
endif

include $(MAKE_INCLUDE_DIR)/build_module.mk
