# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CFLAGS  = -std=c++2a -g3 -Wall -w 

# The build target 
TARGET = main

names := $(shell find $(modPath) -iname "*.hpp")
names += $(shell find $(modPath) -iname "*.cpp")
names += $(shell find $(modPath) -iname "*.h")

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(names) -pthread -lpthread -Wl,--no-as-needed

clean:
	$(RM) $(TARGET)
	
run: main
	./main

# all: main

# # Linking
# main: $(names)
# 	$(CC) $(CFLAGS) -o main $^

# # Rule for main file
# $(tmpDir)/main.o: main.cpp
# 	@mkdir -p $(tmpDir)
# 	$(CC) $(CFLAGS) -c main.cpp -o $@ -I "$(modPath)"

# # rules for module files
# $(tmpDir)/%.o: $(modPath)/%.cpp
# 	mkdir -p $(dir $@)
# 	$(CC) $(CFLAGS) -c $< -o $@


# all: $(TARGET)

# $(TARGET): $(TARGET).cpp
# 	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp json.hpp pch.h simulate.cpp state.h

# clean:
# 	$(RM) $(TARGET)
	
# run: main
# 	./main