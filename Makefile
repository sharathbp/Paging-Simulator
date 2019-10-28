CXX = g++
CXXFLAGS = -lglui2 -lGL -lGLU -lglut 

IPATH1 = ./dependencies/freeglut/include
IPATH2 = ./dependencies/glui/include
LPATH1 = ./dependencies/freeglut/lib
LPATH2 = ./dependencies/glui/lib

all: Paging

Paging: main.o 
	$(CXX) "-I$(IPATH1)" "-I$(IPATH2)" "-L$(LPATH1)" "-L$(LPATH2)" -o ./bin/Paging ./bin/main.o $(CXXFLAGS)

main.o: ./src/main.cpp
	$(CXX) "-I$(IPATH1)" "-I$(IPATH2)" -c -o ./bin/main.o  ./src/main.cpp

clean:
	rm ./bin/*
