# project name (generate executable with this name)
MMANAGER   = mmanage
VMAPPL	   = vmappl

CC       = gcc
# compiling flags here
CFLAGS   = -std=c99 -Wall -I. -D_XOPEN_SOURCE=600
CFLAGS   += -DDEBUG_MESSAGES
CFLAGS   += -DVEM_ALGO=2 #0=FIFO, 1=LRU, 2=CLOCK


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


#$(BINDIR)/$(TARGET): $(OBJECTS)
#	@$(LINKER) $@ $(LFLAGS) $(OBJECTS)
#
#$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
#	@$(CC) $(INCDIR) $(CFLAGS) -c $< -o $@

all: $(BINDIR)/$(VMAPPL) $(BINDIR)/$(MMANAGER)

.PHONEY: clean
clean: 
	@$(rm) $(OBJECTS)
	@$(rm) $(BINDIR)/$(VMAPPL)
	@$(rm) $(BINDIR)/$(MMANAGER)
	@$(rm) $(BINDIR)/*.txt
	@$(rm) $(BINDIR)/*.bin
	
.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(VMAPPL)
	@$(rm) $(BINDIR)/$(MMANAGER)

# VMAPPL COMPILIEREN
$(OBJDIR)/vmappl.o: $(SRCDIR)/vmappl.c 
	$(CC) $(CFLAGS) $(INCDIR) -c -o $(OBJDIR)/vmappl.o $(SRCDIR)/vmappl.c
	
$(OBJDIR)/vmaccess.o: $(SRCDIR)/vmaccess.c 
	$(CC) $(CFLAGS) $(INCDIR) -c -o $(OBJDIR)/vmaccess.o $(SRCDIR)/vmaccess.c 
	
$(BINDIR)/$(VMAPPL): $(OBJDIR)/vmaccess.o $(OBJDIR)/vmappl.o
	$(CC) $(CFLAGS) $(LFLAGS) -o $(BINDIR)/vmappl $(OBJDIR)/vmaccess.o $(OBJDIR)/vmappl.o

# MMANAGE COMPILIEREN
$(OBJDIR)/mmanage.o: $(SRCDIR)/mmanage.c 
	$(CC) $(CFLAGS) $(INCDIR) -c -o $(OBJDIR)/mmanage.o $(SRCDIR)/mmanage.c

$(BINDIR)/$(MMANAGER): $(OBJDIR)/mmanage.o
	$(CC) $(CFLAGS) $(LFLAGS) -o $(BINDIR)/mmanage $(OBJDIR)/mmanage.o
