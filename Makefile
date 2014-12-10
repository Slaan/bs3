# project name (generate executable with this name)
TARGET   = run

CC       = gcc
# compiling flags here
CFLAGS   = -std=c99 -Wall -I.

LINKER   = gcc -o
# linking flags here
LFLAGS   = -Wall -lm -pthread

# change these to set the proper directories where each files shoould be
SRCDIR   = src
INCDIR	 = -I./include
OBJDIR   = build/obj
BINDIR   = build

SOURCES  := $(wildcard $(SRCDIR)/*.c)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f


$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(INCDIR) $(CFLAGS) -c $< -o $@

.PHONEY: clean
clean: 
	@$(rm) $(OBJECTS)
	@$(rm) $(BINDIR)/$(TARGET)

.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
