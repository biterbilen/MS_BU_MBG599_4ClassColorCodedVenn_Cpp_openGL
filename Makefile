MAINSRC = kubikVenn.cpp
#MAINLIB = -I/home/biter/PROGRAMS/Mesa/include -L/home/biter/PROGRAMS/Mesa/lib -lglut -lOSMesa 
MAINLIB = -L/home/biter/PROGRAMS/Mesa/lib -lglut -lOSMesa 

SRC = $(MAINSRC) texture.cpp

OBJ = $(SRC:.c=.o)

OPT = -O3
CC = gcc $(OPT) -Wall

# You might have to change GLU to MesaGLU depending on your Mesa version
GLLIBS = $(MAINLIB) -lGL -lGLU
LIBS = $(GLLIBS) -lm

all: kubikVenn

.c.o:
	$(CC) -c $<

kubikVenn: $(OBJ)
	gcc -o $@ $^ $(LIBS)

clean:
	rm -f kubikVenn *.o 
