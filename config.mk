# Configure the platform type
PLATFORM ?= LINUX

# Configure the name of the executable
TARGET_NAME ?= shroom_ventures

# Compile time configuration options (pre processor defines)

CONFIG += -D DELTA_TIME=0.0167f#	# Around 1/60, appropriate delta time for 60 FPS
CONFIG += -D GAME_WIDTH=1600# 		# The width of the game screen on a render texture,
									# in pixels
CONFIG += -D GAME_HEIGHT=900#		# The height of the game screen on a render texture,
									# in pixels

# Each platform will have it's own set of platform-specific default config values.
# RAYLIB_INSTALL : the COMMON parent of the directory which contains the installation for raylib library and the directory which contains the installation for it's headers
# LIBS : all the libraries you want to link against, this is if you modify the original source code and want to use different libraries
# TARGET : the name of the executable to be generated, you should really only modify this if you want to have actually different names on different systems or something
#          otherwise you can just rename the TARGET_NAME variable above
ifeq ($(PLATFORM),LINUX)
    # Linux defaults
    RAYLIB_INSTALL ?= /usr/local
    LIBS ?= -lGL -lraylib -lm -lpthread -ldl -lrt -lX11 -lc -latomic
    TARGET ?= ./bin/$(TARGET_NAME)

else ifeq ($(PLATFORM),MSYS)
    # MSYS2 defaults
    RAYLIB_INSTALL ?= /mingw64
    LIBS ?= -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET ?= ./bin/$(TARGET_NAME).exe

else
    $(error only MSYS2 and LINUX builds are supported at the moment)
endif
