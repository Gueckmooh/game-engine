
MODULE=window

SRCFILES=$(shell find src/ -maxdepth 1 -regextype posix-awk -regex '.*\.cpp' -print)

ifeq ($(WINDOWS),1)
SRCFILES+=$(shell find src/impl/windows -maxdepth 1 -regextype posix-awk -regex '.*\.cpp' -print)
LDFLAGS=-L/usr/x86_64-w64-mingw32/lib
LDFLAGS+=-lgdi32
else
SRCFILES+=$(shell find src/impl/linux -maxdepth 1 -regextype posix-awk -regex '.*\.cpp' -print)
endif

-include $(MAKEDIR)/common.mk
.SECONDEXPANSION:
