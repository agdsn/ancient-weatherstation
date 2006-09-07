CC      = gcc
LD      = gcc
RM	= rm
CFLAGS  = -c
LDFLAS  = -o
#INCL	= -I$$(pg_config --includedir)

BIN_NAME = chart
OBJS	 = chart.o config.o common.o image_file/image_file.o image_file/image_config.o

CONF_NAME = chart.conf

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
	@ echo Linke: $(LD) $(DEBUG) $(NOLOG) -L$$(pg_config --libdir)/pgsql -lpq $(LDFLAS) $(BIN_NAME) $(OBJS)
	@ $(LD) $(DEBUG) $(NOLOG) -L$$(pg_config --libdir)/pgsql -lpq $(LDFLAS) $(BIN_NAME) $(OBJS)
	@ echo Binary $(BIN_NAME) ist fertig!

# Abhängigkeiten
chart.o: chart.c \
	definitions.h \
	config.h \
	chart.h \
	common.h \
	image_file/image_file.h
common.o: common.c \
	common.h
config.o: config.c \
	config.h \
	definitions.h \
	common.h
image_file/image_config.o: image_file/image_config.c \
	common.h \
	image_file/image_common.h \
	config.h
image_file/image_file.o: image_file/image_file.c \
	common.h \
	image_file/image_common.h \
	image_file/image_file.h \
	image_file/image_config.h

# Compillieren
$(OBJS):
	@ echo "Kompilliere: "$(CC) -o $@ $(DEBUG) $(NOLOG) $(INCL) $(CFLAGS) $*.c
	@ $(CC) -o $@ $(DEBUG) $(NOLOG) $(INCL) $(CFLAGS) $*.c

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
