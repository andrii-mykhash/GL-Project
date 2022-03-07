CXX_FLAGS = -std=c++20 -g -Wall -Iinc 
SRC = src
BUILD = build
CC = g++-10

.Phony: all build clear

build: mkdir $(SRC)/field.cpp dot.o field.o
	$(CC) $(CXX_FLAGS) $(SRC)/main.cpp -o main $(BUILD)/dot.o $(BUILD)/field.o
	mv main build/

mkdir: 
	mkdir -p build

dot.o: $(SRC)/dot.cpp 
	$(CC) $(CXX_FLAGS)  -c $(SRC)/dot.cpp
	mv dot.o $(BUILD)/

field.o: $(SRC)/field.cpp dot.o
	$(CC) $(CXX_FLAGS) -c $(SRC)/field.cpp 
	mv field.o $(BUILD)/

clear: 
	rm -rf ./$(BUILD)