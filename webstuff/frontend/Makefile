PROGDIR = $(DESTDIR)/usr/share/weatherstation-frontend/
CONFDIR = $(DESTDIR)/etc/weatherstation/
DOCDIR  = $(DESTDIR)/usr/share/doc/weatherstation-frontend/

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
	      install -m444 $$i $(PROGDIR)/$(DIR)/; \
	    fi; \
	  done; \
	fi
	@ rm -f $(PROGDIR)/Makefile
	@ rm -rf $(PROGDIR)/images/chart
	@ install -d $(CONFDIR)
	@ install -m444 frontend.conf $(CONFDIR)
	@ install -d $(DOCDIR)
	@ install -m444 COPYING $(DOCDIR)
	@ install -m444 readme $(DOCDIR)
	@ echo "done"

clean:
	@ echo "nix zu tun"
