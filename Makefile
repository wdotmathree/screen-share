CC=g++
INCLUDE=-Iinclude
LIBS=
CFLAGS:=${CXXFLAGS} -Wall -Wextra -Wpedantic -fpermissive -fno-trapping-math -fno-math-errno -fno-signed-zeros -march=native -falign-functions=16
SRCS=$(wildcard *.c)
HDRS=$(wildcard include/*.h)

ifeq ($(OS),Windows_NT)
	ARCH = windows
	CFLAGS += -DWINDOWS
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		ARCH = linux
		CFLAGS += -DLINUX
		LIBS += -lX11
	else
		$(error Unknown OS)
	endif
endif
SRCS += $(wildcard $(ARCH)/*)

OBJS=$(addsuffix .o, $(SRCS))

.PHONY: debug release clean

debug: CFLAGS += -g
debug: a.out
	./a.out

release: CFLAGS += -O2
release: a.out
	strip -s a.out

a.out: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(LIBS)

%.c.o: %.c $(HDRS) Makefile
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)

%.asm.o: %.asm $(HDRS) Makefile
	$(ASM) -f elf64 $< -o $@

clean:
	rm -f $(OBJS) a.out
