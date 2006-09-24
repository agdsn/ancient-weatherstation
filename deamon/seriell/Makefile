CC      = gcc
LD      = gcc
RM	= rm
CFLAGS  = -c
LDFLAS  = -o
INCL	= -I$$(pg_config --includedir)

BIN_NAME = weatherdeamon
OBJS	 = main.o config.o input.o process.o write.o

CONF_NAME = weatherd.conf

INSTDIR  = /usr/bin/
CONFDIR  = /etc/weatherstation/
INSTGRP  = losinski
INSTUSR  = losinski

DESTDIR  = /home/losinski

# Alles bauen
all: $(BIN_NAME)
#	$(MAKE) cleanup


# Binary Linken
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
	@ echo "baue Version mit Debugoutput ..."
	@ $(MAKE) all DEBUG=-DDEBUG

# Programm ohne Log-ausgabe bauen
nolog:
	@ echo "baue Version ohne Logoutput ..."
	@ $(MAKE) all NOLOG=-DNO_LOGING

# Installieren
install:
	@ echo "kopiere $(BIN_NAME) nach $(DESTDIR)$(INSTDIR)"
	@ mkdir -p $(DESTDIR)$(INSTDIR); \
	  cp $(BIN_NAME) $(DESTDIR)$(INSTDIR)
#	@ echo "setze Rechte auf $(BIN_NAME)"
#	@ cd $(DESTDIR)$(INSTDIR); \
#	  chmod 755 $(BIN_NAME); \
#	  chgrp $(INSTGRP) $(BIN_NAME); \
#	  chown $(INSTUSR) $(BIN_NAME)
	@ echo "kopiere $(CONF_NAME) nach $(DESTDIR)$(CONFDIR)"
	@ mkdir -p $(DESTDIR)$(CONFDIR); \
	  cp $(CONF_NAME) $(DESTDIR)$(CONFDIR)
#	@ echo "setze Rechte auf $(CONF_NAME)"
#	@ cd $(DESTDIR)$(CONFDIR); \
#	  chmod 755 $(CONF_NAME); \
#	  chgrp $(CONFGRP) $(CONF_NAME); \
#	  chown $(CONFUSR) $(CONF_NAME)


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
