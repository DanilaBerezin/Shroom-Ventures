include config.mk

target := shroom_ventures 
CC := gcc
CFLAGS := -Wall -Wextra -Wconversion -Wno-unused-function -Wno-unused-parameter
CFLAGS += -std=gnu99
CFLAGS += -L$(RAYLIB_INSTALL)/lib 
CFLAGS += -I$(RAYLIB_INSTALL)/include
CFLAGS += -I./includes
LIBS := -lGL -lraylib -lm -lpthread -ldl -lrt -lX11 -lc -latomic
cfiles := $(wildcard *.c)
objs := $(cfiles:.c=.o)
.PHONY: all release clean debug run

all: CFLAGS += -O2
all: $(target)	
release: clean $(target)

debug: CFLAGS += -Werror -g -O0 -D DEBUG
debug: clean $(target)

$(target): $(objs)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean: 
	-@rm -f $(target) $(objs)

run: $(target)
	@./$(target)
