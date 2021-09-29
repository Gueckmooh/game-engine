.SECONDEXPANSION:

CXXSTD=-std=c++17

CC=$(TARGET)gcc
CXX=$(TARGET)g++

DLLEXT ?= so

CXXFLAGS+=-I$(INCLUDEDIR)
CXXFLAGS+=$(CXXSTD)
LDFLAGS+=-L$(LIBDIR)
ifeq ($(DYNAMIC_LIB),1)
CXXFLAGS+=-fPIC
TOBUILD=$(LIBDIR)/$(BINNAME).$(DLLEXT)
endif
ifeq ($(EXECUTABLE),1)
TOBUILD=$(BINDIR)/$(BINNAME)
endif

ifeq ($(WINDOWS),1)
CXXFLAGS+=-D__USE_WINDOWS__
endif

ifeq ($(DEBUG),1)
	CXXFLAGS+=-g -O0
endif

CXXFLAGS+=$(EXTRACXXFLAGS)

LIBS=$(shell echo $(DEPLIBS) | sed 's/,/ /g')
LIBFLAGS=$(addprefix -l, $(LIBS))

LDFLAGS+=$(LIBFLAGS)

build: $(TOBUILD)

$(OBJDIR)/$(MODDIR)/%.o: $(SRCDIR)/$(MODDIR)/src/%.cpp $$(@D)/.f
	$(CXX) $(CXXFLAGS) -c -o $@ $<

ifneq ($(SRCFILES),)
SRCFILES:=$(addprefix $(SRCDIR)/$(MODDIR)/, $(SRCFILES))
endif

SRCFILES?=$(shell find $(SRCDIR)/$(MODDIR)/src/ -regextype posix-awk -regex '.*\.cpp' -print)
OBJFILENAMES=$(SRCFILES:.cpp=.o)
OBJFILES=$(subst $(SRCDIR)/$(MODDIR)/src/, $(OBJDIR)/$(MODDIR)/, $(OBJFILENAMES))

.SECONDARY: $(OBJFILES)

$(BINDIR)/%: $(OBJFILES)
	$(CXX) $(LDFLAGS) -o $@ $^

$(LIBDIR)/%.$(DLLEXT): $(OBJFILES)
	$(CXX) -shared -o $@ $^ $(LDFLAGS)
