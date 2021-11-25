.SECONDEXPANSION:

.PHONY: check
check:: build_test

UNITTEST_SOURCE_FILES?=$(shell find $(MODULE_UNITTEST_PATH) -name "*.cpp" -print)
UNITTEST_SOURCE_FILES:=$(subst $(MODULE_UNITTEST_PATH)/,,$(UNITTEST_SOURCE_FILES))
UNITTEST_EXEC_FILES:=$(addprefix $(TEST_DIR)/$(MODULE_NAME)/,$(UNITTEST_SOURCE_FILES:.cpp=))

UNITTEST_DEPS_FILES:=$(addprefix $(MODULE_SOURCE_PATH)/,$(SOURCE_FILES))
UNITTEST_DEPS_FILES+=$(addprefix $(MODULE_HEADERS_PATH)/,$(HEADER_FILES))

GTEST_FLAGS:=$(shell pkgconf gtest -cflags -libs)

RUNTEST_MSG=$(ECHO) "\tRUNTEST\t$(shell realpath --relative-to="$(TEST_DIR)" $(TEST_TO_RUN))"

$(TEST_DIR)/$(MODULE_NAME)/%: $(MODULE_UNITTEST_PATH)/%.cpp $(UNITTEST_DEPS_FILES) $$(@D)/.f
	$(CXX_MSG)
	$(QAT)$(CXX) $(CXXFLAGS) $(GTEST_FLAGS) -o $@ $< $(LDFLAGS) -l$(MODULE_LIB_NAME)

.PHONY: build_test
build_test: $(UNITTEST_EXEC_FILES)

check:: run_test

.PHONY: run_test_/%
run_test_/%: TEST_TO_RUN=$(subst run_test_/,,$@)
run_test_/%:
	$(RUNTEST_MSG)
	$(QAT)mkdir -p $(dir $(subst $(TEST_DIR),$(TEST_RESULTS_DIR),$(TEST_TO_RUN)))
	$(QAT)LD_LIBRARY_PATH=$(LIB_DIR) $(TEST_TO_RUN) |\
	tee $(subst $(TEST_DIR),$(TEST_RESULTS_DIR),$(TEST_TO_RUN).log)

.PHONY: run_test
run_test: $(addprefix run_test_/,$(UNITTEST_EXEC_FILES))
	@echo find test results in $(TEST_DIR)/$(MODULE_NAME)
