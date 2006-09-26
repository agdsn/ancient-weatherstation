CC      = gcc
LD      = gcc
RM	= rm
CFLAGS  = -c
LDFLAS  = -o
LD_LIBS = -lm -lpq -lgd -lpng -lz -lfreetype
#INCL	= -I$$(pg_config --includedir)

BIN_NAME = weather_chart
OBJS	 = chart.o config.o common.o image_file/image_file.o image_file/image_config.o image_file/image_draw.o image_file/image_common.o image_file/image_data.o

CONF_NAME = chart.conf

INSTDIR  = /usr/bin/
CONFDIR  = /etc/weatherstation/
DOCDIR   = /usr/share/doc/weatherstation-chart/
INSTGRP  = losinski
INSTUSR  = losinski

#DESTDIR  = /home/losinski

# Alles bauen
all: $(BIN_NAME)
#	$(MAKE) cleanup


# Binary Linken
$(BIN_NAME): $(OBJS)
	@ echo Linke: $(LD) $(DEBUG) $(NOLOG) -L$$(pg_config --libdir)/pgsql $(LD_LIBS) $(LDFLAS) $(BIN_NAME) $(OBJS)
	@ $(LD) $(DEBUG) $(NOLOG) -L$$(pg_config --libdir)/pgsql $(LD_LIBS) $(LDFLAS) $(BIN_NAME) $(OBJS)
	@ echo Binary $(BIN_NAME) ist fertig!

# Abhängigkeiten
chart.o: chart.c \
	definitions.h \
	config.h \
	chart.h \
	common.h \
	image_file/image_file.h
common.o: common.c \
	common.h \
	definitions.h
config.o: config.c \
	config.h \
	definitions.h \
	common.h
image_file/image_common.o: image_file/image_common.c \
	image_file/image_common.h
image_file/image_config.o: image_file/image_config.c \
	common.h \
	definitions.h \
	config.h \
	image_file/image_common.h
image_file/image_data.o: image_file/image_data.c \
	image_file/image_data.h \
	image_file/image_common.h \
	common.h \
	definitions.h
image_file/image_draw.o: image_file/image_draw.c \
	definitions.h \
	image_file/image_draw.h \
	image_file/image_data.h \
	image_file/image_common.h
image_file/image_file.o: image_file/image_file.c \
	definitions.h \
	common.h \
	image_file/image_common.h \
	image_file/image_file.h \
	image_file/image_config.h \
	image_file/image_draw.h

# Compillieren
$(OBJS):
	@ echo "Kompilliere: "$(CC) -o $@ $(DEBUG) $(NOLOG) $(INCL) $(CFLAGS) $*.c
	@ $(CC) -o $@ $(DEBUG) $(NOLOG) $(INCL) $(CFLAGS) $*.c

# Programm mit debug-ausgabe bauen
debug: 
	@ echo "baue Version mit Debugoutput ..."
	@ $(MAKE) all DEBUG="-DDEBUG -g"

# Installieren
install:
	@ echo "kopiere $(BIN_NAME) nach $(DESTDIR)$(INSTDIR)"
	@ install -d $(DESTDIR)$(INSTDIR); 
	@ install $(BIN_NAME) $(DESTDIR)$(INSTDIR)
	@ echo "kopiere $(CONF_NAME) nach $(DESTDIR)$(CONFDIR)"
	@ install -d $(DESTDIR)$(CONFDIR); 
	@ install -m644 $(CONF_NAME) $(DESTDIR)$(CONFDIR)
	@ echo "Lege Verzeichnis fuer Subconfigs an und kopiere Beispiel-Configs"
	@ install -d $(DESTDIR)$(CONFDIR)/image_conf; 
	@ install -m644 image_conf/* $(DESTDIR)$(CONFDIR)/image_conf
	@ install -d $(DESTDIR)$(DOCDIR)  
	@ install COPYING $(DESTDIR)$(DOCDIR)


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
