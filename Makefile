EXECUTABLE=doneyet
OBJECTS = main project task info-box dialog-box utils hierarchical-list file-manager \
          serializer date filter-predicate list-chooser note curses-menu \
          workspace config-parser doneyet-config
DEBUGFLAGS = -g -Wall -Wno-sign-compare #-fprofile-arcs -ftest-coverage
FASTFLAGS = -O3
COMPILEFLAGS =$(DEBUGFLAGS) $(FASTFLAGS)
OFILES = $(OBJECTS:%=%.o)
CCC	= g++
IFLAGS = -I.
LFLAGS =
LIBS	= -lform -lmenu -lpanel -lncurses

PROPER_PKG_CONFIG = $(shell pkg-config --cflags ncurses >/dev/null 2>&1 && echo true || echo false)
ifeq ($(PROPER_PKG_CONFIG),true)
IFLAGS += $(shell pkg-config --cflags-only-I ncurses)
LFLAGS += $(shell pkg-config --libs-only-L ncurses)
endif

all	: $(EXECUTABLE)

# the executable depends on all of the o-files being up-to-date
$(EXECUTABLE): $(OFILES)
	@echo Linking...
	$(CCC) -o $(EXECUTABLE) $(COMPILEFLAGS) $(OFILES) $(LIBS) $(LFLAGS) 

#Define a rule for building .o from .C files
# the o-files depend on the respectively-named C-file
# the "$<" refers to the current .C file
%.o:	%.cc
	$(CCC) $(COMPILEFLAGS) $(IFLAGS) -c $< 


# All .cc files depend on their headers...
%.cc	: %.h

# Clean up after building. 
tidy: 
	$(RM) $(OFILES)
	$(RM) *.*~ \#*\# *.gch


# clean will tidy first
clean: tidy
	$(RM) $(EXECUTABLE)

# remake
remake: clean $(EXECUTABLE)

# dependencies
depend:
	makedepend -- $(OBJECTS:%=%.h)

# The suffixes is necessary to allow make to recompile what's changed
.SUFFIXES= .h .cc
# DO NOT DELETE THIS LINE -- make depend depends on it.
