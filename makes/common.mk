BUILDDIR=$(BASE)/build
INCLUDEDIR=$(BUILDDIR)/include
SRCDIR=$(BASE)/src
OBJDIR=$(BUILDDIR)/objs
LIBDIR=$(BUILDDIR)/lib
BINDIR=$(BUILDDIR)/bin
QUIET=@

ifeq ($(WINDOWS),1)
-include $(MAKEDIR)/windows.mk
endif
-include $(MAKEDIR)/cpp.mk
.SECONDEXPANSION:

%/.f:
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)touch $@

.PRECIOUS: %/.f
