EXECUTABLE=todo
OBJECTS = main project task dialog-box utils hierarchical-list file-manager \
          serializer date filter-predicate list-chooser
DEBUGFLAGS = -g
FASTFLAGS = -O3
COMPILEFLAGS =$(DEBUGFLAGS) $(FASTFLAGS)
OFILES = $(OBJECTS:%=%.o)
CCC	= g++
IFLAGS = -I.
LFLAGS =
LIBS	= -lform -lmenu -lpanel -lncurses

all	: $(EXECUTABLE)

# the executable depends on all of the o-files being up-to-date
$(EXECUTABLE): $(OFILES)
	@echo Linking...
	$(CCC) -o $(EXECUTABLE) $(COMPILEFLAGS) $(OFILES) $(LIBS) $(IFLAGS) 

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

project.o: /usr/include/ncurses.h /usr/include/ncurses_dll.h
project.o: /usr/include/stdio.h /usr/include/_types.h
project.o: /usr/include/sys/_types.h /usr/include/sys/cdefs.h
project.o: /usr/include/machine/_types.h /usr/include/ppc/_types.h
project.o: /usr/include/unctrl.h /usr/include/curses.h /usr/include/stdarg.h
project.o: /usr/include/stdbool.h /usr/include/panel.h task.h
project.o: hierarchical-list.h serializer.h date.h filter-predicate.h
task.o: /usr/include/ncurses.h /usr/include/ncurses_dll.h
task.o: /usr/include/stdio.h /usr/include/_types.h /usr/include/sys/_types.h
task.o: /usr/include/sys/cdefs.h /usr/include/machine/_types.h
task.o: /usr/include/ppc/_types.h /usr/include/unctrl.h /usr/include/curses.h
task.o: /usr/include/stdarg.h /usr/include/stdbool.h hierarchical-list.h
task.o: serializer.h date.h
dialog-box.o: /usr/include/ncurses.h /usr/include/ncurses_dll.h
dialog-box.o: /usr/include/stdio.h /usr/include/_types.h
dialog-box.o: /usr/include/sys/_types.h /usr/include/sys/cdefs.h
dialog-box.o: /usr/include/machine/_types.h /usr/include/ppc/_types.h
dialog-box.o: /usr/include/unctrl.h /usr/include/curses.h
dialog-box.o: /usr/include/stdarg.h /usr/include/stdbool.h
dialog-box.o: /usr/include/form.h /usr/include/eti.h /usr/include/math.h
dialog-box.o: /usr/include/architecture/ppc/math.h utils.h
utils.o: /usr/include/ncurses.h /usr/include/ncurses_dll.h
utils.o: /usr/include/stdio.h /usr/include/_types.h /usr/include/sys/_types.h
utils.o: /usr/include/sys/cdefs.h /usr/include/machine/_types.h
utils.o: /usr/include/ppc/_types.h /usr/include/unctrl.h
utils.o: /usr/include/curses.h /usr/include/stdarg.h /usr/include/stdbool.h
hierarchical-list.o: /usr/include/ncurses.h /usr/include/ncurses_dll.h
hierarchical-list.o: /usr/include/stdio.h /usr/include/_types.h
hierarchical-list.o: /usr/include/sys/_types.h /usr/include/sys/cdefs.h
hierarchical-list.o: /usr/include/machine/_types.h /usr/include/ppc/_types.h
hierarchical-list.o: /usr/include/unctrl.h /usr/include/curses.h
hierarchical-list.o: /usr/include/stdarg.h /usr/include/stdbool.h
date.o: serializer.h
