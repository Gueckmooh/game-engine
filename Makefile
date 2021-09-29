.PHONY: build
build:
	sbmake MTB=main -build-upstream

MODULE=

-include $(MAKEDIR)/common.mk
.SECONDEXPANSION:
