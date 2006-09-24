MakeDirs    = seriell
InstallDirs = seriell init

binary:
	for i in $(MakeDirs); do $(MAKE) -C $$i ; done

debug:
	for i in $(MakeDirs); do $(MAKE) -C $$i debug; done

install:
	for i in $(InstallDirs); do $(MAKE) -C $$i install; done

clean:
	for i in $(MakeDirs); do $(MAKE) -C $$i clean; done
