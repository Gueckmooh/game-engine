.SECONDEXPANSION:

MODULE:=common/file_watcher

HEADERS_EXPORT_DIR=file_watcher

SRC_PATH=$(shell pwd)/src

ifneq ($(OS),windows)
CXXFLAGS+=-D__FILE_WATCHER_USE_INOTIFY__
endif

# ifneq ($(OS),windows)
# SOURCE_FILES:=$(shell find $(SRC_PATH) -maxdepth 1 -name "*.cpp" -print)
# SOURCE_FILES+=$(shell find $(SRC_PATH)/call_stack_backends/gcc -maxdepth 1 -name "*.cpp" -print)
# LDFLAGS+=-lpulse -lpthread
# else
# SOURCE_FILES:=$(shell find $(SRC_PATH) -maxdepth 1 -name "*.cpp" -print)
# SOURCE_FILES+=$(shell find $(SRC_PATH)/call_stack_backends/mingw -maxdepth 1 -name "*.cpp" -print)
# # LDFLAGS=-L/usr/x86_64-w64-mingw32/lib
# # LDFLAGS+=-lgdi32 -lxinput -lm
# endif


include $(MAKE_INCLUDE_DIR)/common.mk
