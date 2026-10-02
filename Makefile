CC = gcc
AR = ar
CFLAGS = -Wall -Werror -std=c99
CPPFLAGS = -I include -D ZL_BUILD_DLL

TARGET = bin/libzl.a
OBJECT = bin/zl.o
SOURCES = src/zl.c $(wildcard src/io/*.c src/ds/*.c src/std/*.c)
HEADERS = $(wildcard include/zl/*.h)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECT)
	$(AR) rcs $@ $^

$(OBJECT): src/zl.c $(SOURCES) $(HEADERS) | bin
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

bin:
	mkdir -p $@

clean:
	$(RM) $(OBJECT) $(TARGET)