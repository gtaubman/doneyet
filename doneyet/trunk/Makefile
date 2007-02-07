EXECUTABLE=todo
OBJECTS = main project
DEBUGFLAGS = -g
FASTFLAGS = -O3
COMPILEFLAGS =$(DEBUGFLAGS)

OFILES =	$(OBJECTS:%=%.o)
CCC	= g++
IFLAGS = -I.
LFLAGS =
LIBS	= -lncurses -lcdk
OPTIMIZATIONFLAGS	= -O3

all	: $(EXECUTABLE)

# the executable depends on all of the o-files being up-to-date
$(EXECUTABLE): $(OFILES)
	@echo Linking...
	$(CCC) -o $(EXECUTABLE) $(OPTIMIZATIONFLAGS) $(COMPILEFLAGS) $(OFILES) $(LIBS) $(IFLAGS) 

#Define a rule for building .o from .C files
# the o-files depend on the respectively-named C-file
# the "$<" refers to the current .C file
%.o:	%.cc
	$(CCC) $(OPTIMIZATIONFLAGS) $(COMPILEFLAGS) $(IFLAGS) -c $< 


# All .cc files depend on their headers...
%.cc	: %.h

# Clean up after building. 
tidy: 
	$(RM) $(OFILES)
	$(RM) *.*~ \#*\#

# clean will tidy first
clean: tidy
	$(RM) $(EXECUTABLE)
