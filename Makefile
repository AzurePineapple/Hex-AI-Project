# Which compiler to use
CC = g++ -g -Wall #-Wextra

# INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -I src/include

# LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -L src/lib

# Specifies libraries we're linking against
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image # -lSDL2_ttf

# Compiler flags
COMPILER_FLAGS = -I src/include/SDL2 -Wno-unused-variable -fopenmp

# Name of output file
OBJ_NAME = Hex

# Which files to compile
OBJS = main.cpp

all:
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
