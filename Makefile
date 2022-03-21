CXX_FLAGS = -std=c++20 -g -Wall -Iinc -pthread
SRC = src
BUILD = build
TEST = test
CC = g++-10

.Phony: all build clear test sc

all: build test sc

#------------ Build  -----------

build: mkdir dot.o field.o
	# $(CC) $(CXX_FLAGS) $(SRC)/main.cpp -o main $(BUILD)/dot.o $(BUILD)/field.o
	# mv main build/

mkdir: 
	mkdir -p build

dot.o: $(SRC)/dot.cpp 
	$(CC) $(CXX_FLAGS) -c $(SRC)/dot.cpp
	mv dot.o $(BUILD)/

field.o: $(SRC)/field.cpp dot.o
	$(CC) $(CXX_FLAGS) -c $(SRC)/field.cpp 
	mv field.o $(BUILD)/

#-----------ServerClient------

server.o: $(SRC)/server.cpp
	$(CC) $(CXX_FLAGS) $(SRC)/server.cpp -c
	mv server.o $(BUILD)/

server_main: field.o server.o $(SRC)/server_main.cpp
	$(CC) $(CXX_FLAGS) $(SRC)/server_main.cpp -o server_main \
	$(BUILD)/dot.o $(BUILD)/field.o $(BUILD)/server.o
	mv server_main $(BUILD)/

client.o: $(SRC)/client.cpp
	$(CC) $(CXX_FLAGS) $(SRC)/client.cpp -c
	mv client.o $(BUILD)/


client_main: field.o client.o $(SRC)/client_main.cpp
	$(CC) $(CXX_FLAGS) $(SRC)/client_main.cpp -o client_main \
	$(BUILD)/dot.o $(BUILD)/field.o $(BUILD)/client.o
	mv client_main $(BUILD)/

sc: client_main server_main

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