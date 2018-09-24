#!/bin/bash
CXX=g++
CXXFLAGS=-g
LIBS=-lsfml-graphics -lsfml-window -lsfml-system -lcurl

all: main.o scribble.o box.o
	$(CXX) $(CXXFLAGS) main.o scribble.o box.o $(LIBS) -o scribble

main.o: main.cpp scribble.o box.o
	$(CXX) $(CXXFLAGS) -c main.cpp

scribble.o: box.cpp box.h scribble.cpp scribble.h
	$(CXX) $(CXXFLAGS) -c scribble.cpp 

box.o: box.cpp box.h 
	$(CXX) $(CXXFLAGS) -c box.cpp 

sub: pad.cpp
	$(CXX) -g $? $(LIBS) -o $@ 	

clean: 
	rm -f *.o scribble

#$? marks the files being monitored and $? gives the target name