CC := x86_64-w64-mingw32-gcc-win32
RAYLIB_HEADERS := /usr/local/include
RAYLIB_WIN_PWD := .
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
target := shroom_ventures.exe

CFLAGS := -std=gnu99
CFLAGS += -L$(RAYLIB_WIN_PWD)
CFLAGS += -I$(RAYLIB_HEADERS)
CFLAGS += -I../includes
CFLAGS += -O2
cfiles := $(wildcard ../*.c)
objs := $(patsubst ../%.c, %.o, $(cfiles))
.PHONY: run

run: $(target)
	-@wine $(target)

$(target): $(objs)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: ../%.c
	$(CC) $(CFLAGS) -c $<


