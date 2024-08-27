# targets: 
# linux 	= x86_64-linux-gnu-gcc
# windows 	= x86_64-w64-mingw32-gcc

CC := clang
CFLAGS = -I. -O3 -W -Wall -Werror
LDFLAGS = 

IDIR = include
ODIR = obj
SDIR = src

DEPS = $(shell find $(IDIR) -name '*.h')
OBJS = $(shell find $(SDIR) -name '*.c' | sed 's/\.c/\.o/g' | sed 's/src/obj/g')

ifeq ($(CC), x86_64-w64-mingw32-gcc)
	LDFLAGS += -lws2_32
endif

obj/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJS)
	$(CC) -o server $(OBJS) -I. $(LDFLAGS)

linux:
	make server CC=x86_64-linux-gnu-gcc

windows:
	make server CC=x86_64-w64-mingw32-gcc

test:
	make && valgrind --leak-check=full ./server

.PHONY: clean
clean:
	rm -rf $(ODIR)/*.o server server.exe a.out