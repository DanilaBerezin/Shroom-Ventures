# Configure the platform type
PLATFORM ?= LINUX

# Configure the name of the executable
target_name ?= shroom_ventures

# Each platform will have it's own set of platform-specific default config values.
# RAYLIB_INSTALL : the COMMON parent of the directory which contains the installation for raylib library and the directory which contains the installation for it's headers
# LIBS : all the libraries you want to link against, this is if you modify the original source code and want to use different libraries
# target : the name of the executable to be generated, you should really only modify this if you want to have actually different names on different systems or something
#          otherwise you can just rename the target_name variable above
ifeq ($(PLATFORM),LINUX)
    # Linux defaults
    RAYLIB_INSTALL ?= /usr/local
    LIBS ?= -lGL -lraylib -lm -lpthread -ldl -lrt -lX11 -lc -latomic
    target ?= ./bin/$(target_name)

else ifeq ($(PLATFORM),MSYS)
    # MSYS2 defaults
    RAYLIB_INSTALL ?= /mingw64
    LIBS ?= -lraylib -lopengl32 -lgdi32 -lwinmm
    target ?= ./bin/$(target_name).exe

else
    $(error only MSYS2 and LINUX builds are supported at the moment)
endif
