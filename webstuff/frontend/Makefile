PROGDIR = $(DESTDIR)/usr/share/weather-frontend
CONFDIR = /etc/weatherstation/

DIR=./

install:
	@ echo -n "Frontend wird installiert"
	@ install -d $(PROGDIR)
	for i in $(DIR)/*; do \
	  if [ -d $$i ]; then \
	    install -d $(PROGDIR)/$$i \
	    $(MAKE) DIR=$$i; \
	  else \
	    install $$i $(PROGDIR)/$(DIR); \
	  fi; \
	done
	@ install -d $(CONFDIR)
	@ install frontend.conf $(CONFDIR)
	@ echo "done"

