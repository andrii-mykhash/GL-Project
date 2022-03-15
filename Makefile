CXX_FLAGS = -std=c++20 -g -Wall -Iinc 
SRC = src
BUILD = build
TEST = test
CC = g++-10

.Phony: all build clear test

#------------ Build -----------

build: mkdir $(SRC)/field.cpp dot.o field.o
	$(CC) $(CXX_FLAGS) $(SRC)/main.cpp -o main $(BUILD)/dot.o $(BUILD)/field.o
	mv main build/

mkdir: 
	mkdir -p build

dot.o: $(SRC)/dot.cpp 
	$(CC) $(CXX_FLAGS) -c $(SRC)/dot.cpp
	mv dot.o $(BUILD)/

field.o: $(SRC)/field.cpp dot.o
	$(CC) $(CXX_FLAGS) -c $(SRC)/field.cpp 
	mv field.o $(BUILD)/

#------------ Test -----------
 
test: mkdir_test dot_test.o field_test.o json_wrapper_test.o $(BUILD)/field.o
	$(CC) $(CXX_FLAGS) $(BUILD)/dot.o $(BUILD)/field.o \
	$(BUILD)/$(TEST)/dot_test.o $(BUILD)/$(TEST)/field_test.o \
	$(BUILD)/$(TEST)/json_wrapper_test.o $(TEST)/main_test.cpp \
	-lcppunit -o main_test 
	mv main_test $(BUILD)/$(TEST)/

mkdir_test: 
	mkdir -p build/test

dot_test.o: $(TEST)/dot_test.cpp
	$(CC) $(CXX_FLAGS) -c $(TEST)/dot_test.cpp -lcppunit
	mv dot_test.o $(BUILD)/$(TEST)/

field_test.o: field.o  $(TEST)/field_test.cpp
	$(CC) $(CXX_FLAGS) -c $(TEST)/field_test.cpp -lcppunit
	mv field_test.o $(BUILD)/$(TEST)/


json_wrapper_test.o: dot.o $(TEST)/json_wrapper_test.cpp
	$(CC) $(CXX_FLAGS) -c $(TEST)/json_wrapper_test.cpp -lcppunit
	mv json_wrapper_test.o $(BUILD)/$(TEST)/

#------------ Clear -----------

clear: 
	rm -rf ./$(BUILD)