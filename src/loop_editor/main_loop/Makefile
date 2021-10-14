MODULE=loop_editor/main_loop

SRC_PATH=$(shell pwd)/src

POST_BUILD=copy-lib
copy-lib:
	cp $(TARGET_PATH) $(dir $(TARGET_PATH))new_$(TARGET)

HEADERS_EXPORT_DIR=main_loop

include $(MAKE_INCLUDE_DIR)/common.mk
