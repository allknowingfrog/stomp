CFLAGS=`sdl2-config --libs --cflags` -lSDL2_image -lm

EXEC=stomp

CMD=gcc -o $(EXEC) stomp.o $(CFLAGS)

all: stomp.o
	$(CMD)

run: stomp.o
	$(CMD) && ./$(EXEC)
