# OBJS specifies which files to compile as part of the project
OBJS = main.cpp primitives.cpp camera_geometry/camera_geometry.cpp shaders/shaders.cpp

# CC specifies which compiler we're using
CC = g++ -std=c++11

# COMPILER_FLAGS specifies the additional compilation options we're using
# -Wall will turn on all standard warnings
COMPILER_FLAGS = -Wall

# LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lglut -lGLEW -lGL -lGLU -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor 

# OBJ_NAME specifies the name of our exectuable
OBJ_NAME = as7

#This is the target that compiles our executable
all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)