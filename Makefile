# Makefile for calc
# Author: Xiaoxiao Pu <i@xiaoxiao.im>

CC = i686-w64-mingw32-gcc
LD = i686-w64-mingw32-gcc
WINDRES = i686-w64-mingw32-windres

CFLAGS = -pipe -W -Wall -O2 -std=gnu99
LIBS = -lgdi32

.phony: all clean

all: calc.exe

clean:
	rm -f calc.exe *.o

calc.exe: button.c calc_exp.c global.c hpfloat.c main.c math_fun.c outbox.c paintboard.c textbox.c wnd_main.c resource.o
	$(CC)  $(CFLAGS)  -o $@  $^ $(LIBS)

resource.o: resource.rc
	$(WINDRES)  -o $@  $<
