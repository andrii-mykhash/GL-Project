.Phony: all build clear

build: mkdir src/field.cpp dot.o field.o
	g++ -g -Wall src/test.cpp -o test build/dot.o build/field.o
	mv test build/

mkdir: 
	mkdir -p build

dot.o: src/dot.cpp 
	g++ -g -Wall -c src/dot.cpp
	mv dot.o build/

field.o: src/field.cpp dot.o
	g++ -g -Wall -c src/field.cpp 
	mv field.o build/

clear: 
	rm -rf ./build