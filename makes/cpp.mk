.SECONDEXPANSION:

CC=$(TARGET)gcc
CXX=$(TARGET)g++

DLLEXT ?= so

CXXFLAGS=-I$(INCLUDEDIR)
LDFLAGS=-L$(LIBDIR)
ifeq ($(DYNAMIC_LIB),1)
CXXFLAGS+=-fPIC
TOBUILD=$(LIBDIR)/$(BINNAME).$(DLLEXT)
$(info $(TOBUILD))
endif
ifeq ($(EXECUTABLE),1)
TOBUILD=$(BINDIR)/$(BINNAME)
endif

CXXFLAGS+=$(EXTRACXXFLAGS)

LIBS=$(shell echo $(DEPLIBS) | sed 's/,/ /g')
LIBFLAGS=$(addprefix -l, $(LIBS))

LDFLAGS+=$(LIBFLAGS)

build: $(TOBUILD)

$(OBJDIR)/$(MODDIR)/%.o: $(SRCDIR)/$(MODDIR)/src/%.cpp $$(@D)/.f
	$(CXX) $(CXXFLAGS) -c -o $@ $<

SRCFILES=$(wildcard $(SRCDIR)/$(MODDIR)/src/*.cpp)
OBJFILENAMES=$(notdir $(SRCFILES:.cpp=.o))
OBJFILES=$(addprefix $(OBJDIR)/$(MODDIR)/, $(OBJFILENAMES))

.SECONDARY: $(OBJFILES)

$(BINDIR)/%: $(OBJFILES)
	$(CXX) $(LDFLAGS) -o $@ $^

$(LIBDIR)/%.$(DLLEXT): $(OBJFILES)
	$(CXX) $(LDFLAGS) -shared -o $@ $^
