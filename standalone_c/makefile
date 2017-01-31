HEADERS = sweeper.h
OBJECTS = interface.o sweeper.o

all: interface

%.o: %.c $(HEADERS)
	gcc -c $< -o $@

interface: $(OBJECTS)
	gcc $(OBJECTS) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f interface

