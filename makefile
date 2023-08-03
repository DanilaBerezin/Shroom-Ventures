target := game
CC := gcc
CFLAGS := -Wall -Wextra -Wconversion -Wno-unused-function -Wno-unused-parameter
CFLAGS += -std=gnu99
CFLAGS += -I/home/danila/compiled/bin/raylib/include
CFLAGS += -I./includes
LIBS := -lGL -lraylib -lm -lpthread -ldl -lrt -lX11 -lc -latomic
cfiles := $(wildcard *.c)
objs := $(cfiles:.c=.o)
.PHONY: all clean debug run

all: CFLAGS += -O2
all: $(target)

debug: CFLAGS += -Werror -g -O0 -D DEBUG	
debug: clean $(target)

$(target): $(objs)
	$(CC) -L/home/danila/compiled/bin/raylib/lib $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean: 
	-@rm -f $(target) $(objs)

run: $(target)
	@./$(target)
