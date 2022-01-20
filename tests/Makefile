
ARGS=-I../src -I../Concurrent -I../uSockets/src -std=c++2a -pedantic -Wall ../src/networking/*.cpp ../uSockets/uSockets.a -lpthread -ggdb3

networking: networking_test.cpp
	g++ networking_test.cpp $(ARGS)
