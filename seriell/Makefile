CC      = gcc
LD      = gcc
RM	= rm
DEBUG   = -DDEBUG
FLAGS   =

#binary: 
#	#make clean
#	# Compile
#	$(CC) $(FLAGS) -o main.o -c main.c
#	$(CC) $(FLAGS) -o input.o -c input.c
#	$(CC) $(FLAGS) -o process.o -c process.c
#	$(CC) $(FLAGS) -I/usr/include/postgresql -o write.o -c write.c
#	$(CC) $(FLAGS) -o config.o -c config.c
#	
#	# Link
#	$(LD) $(FLAGS) -L/usr/lib/pgsql -lpq -o weatherdeamon main.o input.o process.o write.o config.o
#
#debug:
#	#make clean
#	# Compile
#	$(CC) -DDEBUG -o main.o -c main.c
#	$(CC) -DDEBUG -o input.o -c input.c
#	$(CC) -DDEBUG -o process.o -c process.c
##	$(CC) -DDEBUG -I/usr/include/postgresql -o write.o -c write.c
#	$(CC) -DDEBUG -o config.o -c config.c
#
#	# Link
#	$(LD) -DDEBUG -L/usr/lib/pgsql -lpq -o weatherdeamon main.o input.o process.o write.o config.o
#	 
#
#partclean:
#	#remove *.o
#	$(RM) main.o input.o process.o
#
#clean:
#	#remove all output
#	$(RM) main.o input.o process.o write.o config.o weatherdeamon
#
#
# Die postgres-pfade bekommt man mit pg_config raus!


# Neue Version nach Make-Buch:

weatherdaemon: main.o config.o input.o process.o write.o
	$(LD) -DDEBUG -L/usr/lib/pgsql -lpq -o weatherdeamon main.o input.o process.o write.o config.o

main.o: main.c main.h definitions.h config.h input.h
	$(CC) -DDEBUG -o main.o -c main.c

config.o: config.c config.h definitions.h
	$(CC) -DDEBUG -o config.o -c config.c

input.o: input.c input.h main.h config.h definitions.h process.h
	$(CC) -DDEBUG -o input.o -c input.c

process.o: process.c process.h main.h config.h definitions.h write.h
	$(CC) -DDEBUG -o process.o -c process.c

write.o: write.c write.h main.h definitions.h
	$(CC) -DDEBUG -I/usr/include/postgresql -o write.o -c write.c

cleanup:
	echo "Räume auf...entferne:"
	echo "  " *.o
	$(RM) -f *.o
