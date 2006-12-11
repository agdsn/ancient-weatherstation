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
IMGDIR	 = /var/cache/weatherstation-chart/
DOCDIR   = /usr/share/doc/weatherstation-chart/
FONTDIR  = /usr/share/fonts/truetype/freefont/
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

# Abhaengigkeiten
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
	@ echo "lege $(IMGDIR) an"
	@ install -d $(DESTDIR)$(IMGDIR)
	@ install -d $(DESTDIR)$(FONTDIR)
	@ install -m444 FreeSans.ttf $(DESTDIR)$(FONTDIR)
	@ install -d $(DESTDIR)$(DOCDIR)  
	@ install -m444 COPYING $(DESTDIR)$(DOCDIR)


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
