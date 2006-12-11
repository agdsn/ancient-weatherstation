CC      = gcc
LD      = gcc
RM	= rm
CFLAGS  = -c
LDFLAS  = -o
INCL	= -I$$(pg_config --includedir)

BIN_NAME = weather_checksensor
OBJS	 = checksensor.o config.o mailer.o

CONF_NAME = checksensor.conf

INSTDIR  = /usr/bin/
CONFDIR  = /etc/weatherstation/
DOCDIR   = /usr/share/doc/weatherstation-checksensor/
INSTGRP  = losinski
INSTUSR  = losinski


# Alles bauen
all: $(BIN_NAME)
#	$(MAKE) cleanup


# Binary Linken
$(BIN_NAME): $(OBJS)
	@ echo Linke: $(LD) $(DEBUG) $(NOLOG) -L$$(pg_config --libdir)/pgsql -lesmtp -lssl -lpq $(LDFLAS) $(BIN_NAME) $(OBJS)
	@ $(LD) $(DEBUG) $(NOLOG) -L$$(pg_config --libdir)/pgsql -lesmtp -lssl -lpq $(LDFLAS) $(BIN_NAME) $(OBJS)
	@ echo Binary $(BIN_NAME) ist fertig!

# Abhaengigkeiten
checksensor.o: checksensor.c definitions.h config.h checksensor.h mailer.h
config.o: config.c config.h definitions.h checksensor.h mailer.h
mailer.o: mailer.c mailer.h

# Compillieren
$(OBJS):
	@ echo "Kompilliere: "$(CC) $(DEBUG) $(NOLOG) $(INCL) $(CFLAGS) $*.c
	@ $(CC) $(DEBUG) $(NOLOG) $(INCL) $(CFLAGS) $*.c

# Programm mit debug-ausgabe bauen
debug: 
	@ echo "baue Version mit Debugoutput ..."
	@ $(MAKE) all DEBUG=-DDEBUG

# Installieren
install:
	@ echo "kopiere $(BIN_NAME) nach $(DESTDIR)$(INSTDIR)"
	@ install -d $(DESTDIR)$(INSTDIR); \
	  install $(BIN_NAME) $(DESTDIR)$(INSTDIR)
	@ echo "kopiere $(CONF_NAME) nach $(DESTDIR)$(CONFDIR)"
	@ install -d $(DESTDIR)$(CONFDIR); \
	  install -m644 $(CONF_NAME) $(DESTDIR)$(CONFDIR)
	@ install -d $(DESTDIR)$(DOCDIR)  
	@ install -m644 COPYING $(DESTDIR)$(DOCDIR)


# Aufraeumnen (alle Object-Files loeschen)
cleanup:
	@ echo "Raeume auf..."
	@ echo "...entferne Object-Files:"
	@ echo "  " $(OBJS)
	@ $(RM) -f $(OBJS)

clean: cleanup
	@ echo "...loesche binary:"
	@ echo "  " $(BIN_NAME)
	@ rm -f $(BIN_NAME)
