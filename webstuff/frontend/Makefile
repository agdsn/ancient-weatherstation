PROGDIR = $(DESTDIR)/usr/share/weather-frontend
CONFDIR = $(DESTDIR)/etc/weatherstation/

DIR=./

install:
	@ echo "Frontend wird installiert ..."
	@ install -d $(PROGDIR)
	@ if [ `ls -Al $(DIR) | wc -l` -gt 1 ]; then \
	  for i in $(DIR)/*; do \
	    if [ -d $$i ]; then \
	      echo "lege $(PROGDIR)/$$i an"; \
	      install -d $(PROGDIR)/$$i; \
	      $(MAKE) DIR=$$i/; \
	    else \
	      echo "installiere $$i nach $(PROGDIR)/$$i"; \
	      install $$i $(PROGDIR)/$(DIR)/; \
	    fi; \
	  done; \
	fi
	@ rm -f $(PROGDIR)/Makefile
	@ install -d $(CONFDIR)
	@ install frontend.conf $(CONFDIR)
	@ echo "done"

clean:
	@ echo "nix zu tun"
