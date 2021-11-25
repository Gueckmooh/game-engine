UNITTEST_USING_GTEST?=1

ifneq ($(NOBUILDTESTS),1)

MODULE_UNITTEST_PATH?=$(MODULE_PATH)/unittest
UNITTEST_DIR_EXISTS=$(shell test -d $(MODULE_UNITTEST_PATH) && echo 1 || echo 0)
ifeq ($(UNITTEST_DIR_EXISTS),1)

ifeq ($(UNITTEST_USING_GTEST),1)
include $(MAKERULES_DIR)/gtest.mk
endif

else

.PHONY: check
check: ;

endif
else

.PHONY: check
check: ;

endif
