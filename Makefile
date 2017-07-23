CFLAGS=`sdl2-config --libs --cflags` -lSDL2_image -lm

EXEC=stomp

CMD=gcc -o $(EXEC) stomp.o entity.o $(CFLAGS)

all: stomp.o entity.o
	$(CMD)

run: stomp.o entity.o
	$(CMD) && ./$(EXEC)
