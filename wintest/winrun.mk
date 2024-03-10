include ../config.mk

CC := x86_64-w64-mingw32-gcc-win32
RAYLIB_HEADERS := /usr/local/include
RAYLIB_WIN_PWD := .
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
target := shroom_ventures.exe

CFLAGS := -std=gnu99
CFLAGS += -L$(RAYLIB_WIN_PWD)
CFLAGS += -I$(RAYLIB_HEADERS)
CFLAGS += -I../includes
CFLAGS += $(CONFIG)
CFLAGS += -O2
cfiles := $(wildcard ../src/*.c)
objs := $(patsubst ../src/%.c, %.o, $(cfiles))
.PHONY: run clean

run: $(target)
	-@wine $(target)

$(target): $(objs)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: ../src/%.c
	$(CC) $(CFLAGS) -c $<

clean:
	-@rm -f $(target) $(objs)
