include config.mk

CC := gcc
CFLAGS := -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wpedantic -Wvla
ifeq ($(PLATFORM),MSYS)
    CFLAGS += -static
endif
CFLAGS += -std=c11
CFLAGS += -L$(RAYLIB_INSTALL)/lib
CFLAGS += -I$(RAYLIB_INSTALL)/include
CFLAGS += -I./includes
# -MD automatically creates make rules for each header dependency
CFLAGS += -MD
CFLAGS += $(CONFIG)
CFILES := $(wildcard ./src/*.c)
OBJS := $(patsubst ./src/%.c, ./bin/%.o, $(CFILES))
DEPS := $(patsubst ./src/%.c, ./bin/%.d, $(CFILES))

export LSAN_OPTIONS := suppressions=.suppr_lsan
.PHONY: all release clean debug debug_target run

all: debug_target

release: CFLAGS += -O2
release: clean $(TARGET)

ifeq ($(PLATFORM),LINUX)
debug: clean debug_target
debug_target: CFLAGS += -Wconversion -Werror -Wno-error=conversion -g -O0 -D DEBUG -fsanitize=address,undefined
endif
debug_target: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

./bin/%.o: ./src/%.c config.mk
	$(CC) $(CFLAGS) -o $@ -c $<

# Import those header make rules so that we rebuild the correct things if we modify headers
-include $(DEPS)

clean: 
	-@rm -f $(TARGET) $(OBJS) $(DEPS)

run: debug_target
	@$(TARGET)

# Target for devs only, compiles for a windows target and runs on a linux host. Requires
# some kind windows runtime (the default is wine), a linux-host, a windows-target cross
# compiler (default is mingw-w64), and a windows-target compiled raylib.
winrun:
	$(MAKE) -C wintest -f winrun.mk run

winclean:
	$(MAKE) -C wintest -f winrun.mk clean

