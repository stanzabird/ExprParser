.phony: all clean debug
g++flags = -Wall -pedantic -g
all : parser

clean :
	rm -rf *~ parser

parser : parser.cc
	g++ -o parser $(g++flags) parser.cc

debug :
	ddd parser
