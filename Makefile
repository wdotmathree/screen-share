CC?=gcc
NVCC?=nvcc
LD=$(CC)
INCLUDE=-Iinclude $(shell sdl2-config --cflags)
LDFLAGS:=$(LDFLAGS) $(shell sdl2-config --libs)
CFLAGS:=$(CXXFLAGS) -Wall -Wextra -Wpedantic -fno-trapping-math -fno-math-errno -fno-signed-zeros -march=native -falign-functions=16
NVFLAGS=
SRCS=$(wildcard *.c)
HDRS=$(wildcard include/*.h)

ifeq ($(OS),Windows_NT)
	ARCH = windows
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		ARCH = linux
		LDFLAGS += -lX11
	else
		$(error Unknown OS)
	endif
endif
SRCS += $(wildcard $(ARCH)/*.c) $(wildcard $(ARCH)/*.asm)

ifneq ($(shell which $(NVCC)),)
	LD=$(NVCC)
	SRCS += $(wildcard cuda/*.cu)
	CFLAGS += -DUSE_CUDA
else
	SRCS += $(wildcard cuda/*.c)
endif

OBJS=$(addsuffix .o, $(SRCS))


.PHONY: debug release clean

debug: CFLAGS += -g # -fsanitize=address -fsanitize=undefined
debug: NVFLAGS += -g
debug: a.out
	./a.out

release: CFLAGS += -O2
release: NVFLAGS += -O2
release: a.out
	strip -s a.out

a.out: $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS)

%.c.o: %.c $(HDRS) Makefile
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)

%.cu.o: %.cu include/cuda.h Makefile
	$(NVCC) -c $< -o $@ $(NVFLAGS)

%.asm.o: %.asm $(HDRS) Makefile
	$(ASM) -f elf64 $< -o $@

clean:
	rm -f $(OBJS) a.out cuda/*.c{,u}.o
