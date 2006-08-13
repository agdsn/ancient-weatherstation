CC      = gcc
LD      = gcc
RM	= rm
#DEBUG   = 
CFLAGS  = -c
LDFLAS  = -o

BIN_NAME = weatherdeamon

# Die postgres-pfade bekommt man mit pg_config raus!

# Neue Version nach Make-Buch:

all: $(BIN_NAME)
#	$(MAKE) cleanup


$(BIN_NAME): main.o config.o input.o process.o write.o
	$(LD) $(DEBUG) $(NOLOG) -L/usr/lib/pgsql -lpq $(LDFLAS) $(BIN_NAME) main.o input.o process.o write.o config.o

main.o: main.c main.h definitions.h config.h input.h
	$(CC) $(DEBUG) $(NOLOG) $(CFLAGS) main.c

config.o: config.c config.h definitions.h
	$(CC) $(DEBUG) $(NOLOG) $(CFLAGS) config.c

input.o: input.c input.h main.h config.h definitions.h process.h
	$(CC) $(DEBUG) $(NOLOG) $(CFLAGS) input.c

process.o: process.c process.h main.h config.h definitions.h write.h
	$(CC) $(DEBUG) $(NOLOG) $(CFLAGS) process.c

write.o: write.c write.h main.h definitions.h
	$(CC) $(DEBUG) $(NOLOG) -I/usr/include/postgresql $(CFLAGS) write.c

# Programm mit debug-ausgabe bauen
debug: 
	$(MAKE) all DEBUG=-DDEBUG

# Programm ohne Log-ausgabe bauen
nolog:
	$(MAKE) all NOLOG=-DNO_LOGING

# Aufräumnen (alle Object-Files löschen)
cleanup:
	echo "Räume auf...entferne:"
	echo "  " *.o
	$(RM) -f *.o
