QUIET ?= 1

ifeq ($(QUIET),1)
QAT=@
NQAT=
else
QAT=
endif

SHELL=/bin/bash
ECHO:=$(QAT)echo -e
PWD:=$(shell pwd)

.PRECIOUS: %/.f
%/.f:
	$(QAT)mkdir -p $(dir $@)
	$(QAT)touch $@
