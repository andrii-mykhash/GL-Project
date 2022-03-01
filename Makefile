CXX_FLAGS = -std=c++2a -g -Wall -Iinc 
SRC = src
BUILD = build

.Phony: all build clear

build: mkdir $(SRC)/field.cpp dot.o field.o
	g++ $(CXX_FLAGS) $(SRC)/main.cpp -o main $(BUILD)/dot.o $(BUILD)/field.o
	mv main build/

mkdir: 
	mkdir -p build

dot.o: $(SRC)/dot.cpp 
	g++ $(CXX_FLAGS)  -c $(SRC)/dot.cpp
	mv dot.o $(BUILD)/

field.o: $(SRC)/field.cpp dot.o
	g++ $(CXX_FLAGS) -c $(SRC)/field.cpp 
	mv field.o $(BUILD)/

clear: 
	rm -rf ./$(BUILD)