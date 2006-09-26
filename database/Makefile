Files = structure.sql reademe COPYING
Dest  = /usr/share/doc/weatherstation-database/

install:
	@ echo "erstelle Ordner $(Dest)"
	@ mkdir -p $(DESTDIR)$(Dest)
	@ echo "kopiere Files: $(Files)"
	@ for i in $(Files); do cp $$i $(DESTDIR)$(Dest); done

