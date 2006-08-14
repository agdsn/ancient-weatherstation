CC      = gcc
LD      = gcc
RM	= rm
CFLAGS  = -c
LDFLAS  = -o
INCL	= -I$$(pg_config --includedir)

BIN_NAME = weatherdeamon
OBJS	 = main.o config.o input.o process.o write.o


all: $(BIN_NAME)
#	$(MAKE) cleanup


$(BIN_NAME): $(OBJS)
	@ echo Linke: $(LD) $(DEBUG) $(NOLOG) -L$$(pg_config --libdir)/pgsql -lpq $(LDFLAS) $(BIN_NAME) $(OBJS)
	@ $(LD) $(DEBUG) $(NOLOG) -L$$(pg_config --libdir)/pgsql -lpq $(LDFLAS) $(BIN_NAME) $(OBJS)
	@ echo Binary $(BIN_NAME) ist fertig!

# Abhängigkeiten
main.o: main.c main.h definitions.h config.h input.h
config.o: config.c config.h definitions.h
input.o: input.c input.h main.h config.h definitions.h process.h
process.o: process.c process.h main.h config.h definitions.h write.h
write.o: write.c write.h main.h definitions.h

# Compillieren
$(OBJS):
	@ echo "Kompilliere: "$(CC) $(DEBUG) $(NOLOG) $(INCL) $(CFLAGS) $*.c
	@ $(CC) $(DEBUG) $(NOLOG) $(INCL) $(CFLAGS) $*.c

# Programm mit debug-ausgabe bauen
debug: 
	$(MAKE) all DEBUG=-DDEBUG

# Programm ohne Log-ausgabe bauen
nolog:
	$(MAKE) all NOLOG=-DNO_LOGING

# Aufräumnen (alle Object-Files löschen)
cleanup:
	@ echo "Räume auf..."
	@ echo "...entferne Object-Files:"
	@ echo "  " $(OBJS)
	@ $(RM) -f $(OBJS)

clean: cleanup
	@ echo "...lösche binary:"
	@ echo "  " $(BIN_NAME)
	@ rm -f $(BIN_NAME)
