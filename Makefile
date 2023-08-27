CC=g++
INCLUDE=
LIBS=
CFLAGS:=${CXXFLAGS} -Wall -Wextra -Wpedantic -fpermissive -fno-trapping-math -fno-math-errno -fno-signed-zeros -march=native -falign-functions=16
SRCS=$(wildcard *.c)
HDRS=$(wildcard *.h)

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

SRCS += $(wildcard $(ARCH)/*.c)
HDRS += $(wildcard $(ARCH)/*.h)
INCLUDE += -I$(ARCH)

OBJS=$(SRCS:.c=.o)

.PHONY: debug release clean

defug: CFLAGS += -g
debug: a.out
	./a.out

release: CFLAGS += -O2
release: a.out
	strip -s a.out

a.out: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(LIBS)

%.o: %.c $(HDRS) Makefile
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)

clean:
	rm -f $(OBJS) a.out
