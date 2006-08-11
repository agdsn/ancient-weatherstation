CC      = gcc
LD      = gcc
RM	= rm
DEBUG   = -DDEBUG
FLAGS   =

binary: 
	#make clean
	# Compile
	$(CC) $(FLAGS) -o main.o -c main.c
	$(CC) $(FLAGS) -o input.o -c input.c
	$(CC) $(FLAGS) -o process.o -c process.c
	$(CC) $(FLAGS) -I/usr/include/postgresql -o write.o -c write.c
	$(CC) $(FLAGS) -o config.o -c config.c
	
	# Link
	$(LD) $(FLAGS) -L/usr/lib/pgsql -lpq -o weatherdeamon main.o input.o process.o write.o config.o

debug:
	#make clean
	# Compile
	$(CC) -DDEBUG -o main.o -c main.c
	$(CC) -DDEBUG -o input.o -c input.c
	$(CC) -DDEBUG -o process.o -c process.c
	$(CC) -DDEBUG -I/usr/include/postgresql -o write.o -c write.c
	$(CC) -DDEBUG -o config.o -c config.c

	# Link
	$(LD) -DDEBUG -L/usr/lib/pgsql -lpq -o weatherdeamon main.o input.o process.o write.o config.o
	 

partclean:
	#remove *.o
	$(RM) main.o input.o process.o

clean:
	#remove all output
	$(RM) main.o input.o process.o write.o config.o weatherdeamon


# Die postgres-pfade bekommt man mit pg_config raus!
