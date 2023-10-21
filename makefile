include config.mk

CC := gcc
CFLAGS := -Wall -Wextra  -Wno-unused-function -Wno-unused-parameter
CFLAGS += -std=gnu99
CFLAGS += -L$(RAYLIB_INSTALL)/lib 
CFLAGS += -I$(RAYLIB_INSTALL)/include
CFLAGS += -I./includes
cfiles := $(wildcard *.c)
objs := $(cfiles:.c=.o)
.PHONY: all release clean debug run

all: debug_target
release: CFLAGS += -O2
release: clean $(target)

debug: clean debug_target
debug_target: CFLAGS += -Wconversion -Werror -Wno-error=conversion -g -O0 -D DEBUG
debug_target: $(target)

$(target): $(objs)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean: 
	-@rm -f $(target) $(objs)

run: $(target)
	@./$(target)
