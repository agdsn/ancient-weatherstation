CC      = gcc
LD      = gcc
RM	= rm
CFLAGS  = -c
LDFLAS  = -o
INCL	= -I$$(pg_config --includedir)

BIN_NAME = checksensor
OBJS	 = checksensor.o config.o mailer.o

CONF_NAME = checksensor.conf

INSTDIR  = /usr/bin/
CONFDIR  = /etc/
INSTGRP  = losinski
INSTUSR  = losinski

DESTDIR  = /home/losinski

# Alles bauen
all: $(BIN_NAME)
#	$(MAKE) cleanup


# Binary Linken
$(BIN_NAME): $(OBJS)
	@ echo Linke: $(LD) $(DEBUG) $(NOLOG) -L$$(pg_config --libdir)/pgsql -lesmtp -lssl -lpq $(LDFLAS) $(BIN_NAME) $(OBJS)
	@ $(LD) $(DEBUG) $(NOLOG) -L$$(pg_config --libdir)/pgsql -lesmtp -lssl -lpq $(LDFLAS) $(BIN_NAME) $(OBJS)
	@ echo Binary $(BIN_NAME) ist fertig!

# Abh�ngigkeiten
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
	@ mkdir -p $(DESTDIR)$(INSTDIR); \
	  cp $(BIN_NAME) $(DESTDIR)$(INSTDIR)
	@ echo "setze Rechte auf $(BIN_NAME)"
	@ cd $(DESTDIR)$(INSTDIR); \
	  chmod 755 $(BIN_NAME); \
	  chgrp $(INSTGRP) $(BIN_NAME); \
	  chown $(INSTUSR) $(BIN_NAME)
	@ echo "kopiere $(CONF_NAME) nach $(DESTDIR)$(CONFDIR)"
	@ mkdir -p $(DESTDIR)$(CONFDIR); \
	  cp $(CONF_NAME) $(DESTDIR)$(CONFDIR)
	@ echo "setze Rechte auf $(CONF_NAME)"
	@ cd $(DESTDIR)$(CONFDIR); \
	  chmod 755 $(CONF_NAME); \
	  chgrp $(CONFGRP) $(CONF_NAME); \
	  chown $(CONFUSR) $(CONF_NAME)


# Aufr�umnen (alle Object-Files l�schen)
cleanup:
	@ echo "R�ume auf..."
	@ echo "...entferne Object-Files:"
	@ echo "  " $(OBJS)
	@ $(RM) -f $(OBJS)

clean: cleanup
	@ echo "...l�sche binary:"
	@ echo "  " $(BIN_NAME)
	@ rm -f $(BIN_NAME)