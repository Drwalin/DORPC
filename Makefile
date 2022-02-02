
include MakefileSTD/MakefileSTD

AR = ar
CXX = g++
CXXFLAGS = -flto -pipe -std=c++2a -pedantic -Wall -IuSockets/src -lpthread
LIBS=
LIBFILE=libdorpc.a
USOCKETSFLAGS=

ifeq ($(DEBUG),1)
	CXXFLAGS += -ggdb -g -pg
else
	CXXFLAGS += -O4 -s
endif

ifeq ($(platform),win)
	LIBS += -lzlib
	LIBS += -lgcc_s_seh-1 -lmfc100
	LIBS += -lmfc100u -lmsvcp100 -lmsvcr100 -lmsvcr100_clr0400
else
	LIBS += -L/usr/lib -ldl -lz
	CXXFLAGS += -fPIC
	CXXFLAGS += -I/usr/include
	LIBS += -luv
	USOCKETSFLAGS += USE_LIBUV=1
endif
LIBS += -lpthread -lssl -lcrypto

INCLUDE = -I./src/ -I./Concurrent -I./uSockets/include
CXXFLAGS += $(INCLUDE)
OBJECTS = bin/Debug.o
OBJECTS += bin/networking/Buffer.o bin/networking/Socket.o
OBJECTS += bin/networking/Context.o bin/networking/Loop.o
OBJECTS += bin/networking/Event.o
OBJECTS += bin/rpc/FunctionBase.o bin/rpc/FunctionRegistry.o
OBJECTS += bin/rpc/Context.o

all: tests $(LIBFILE)

$(LIBFILE): $(OBJECTS)
	$(AR) rvs $@ $^

# tests:

TESTS = tests/networking_test.exe tests/serialization_test.exe
TESTS += tests/function_register_test.exe tests/function_register_2_test.exe
TESTS += tests/rpc_1_test.exe
tests: $(TESTS)

tests/%.exe: bin/tests/%.o $(LIBFILE) uSockets/uSockets.a
	$(CXX) -o $@ $^ $(CXXFLAGS)

run: $(TESTS)
	@echo ""
	@echo ""
	@echo Testing
	@echo ""
	tests/rpc_1_test.exe
	@echo ""
	tests/function_register_2_test.exe
	@echo ""
	tests/function_register_test.exe
	@echo ""
	tests/serialization_test.exe
	@echo ""
	tests/networking_test.exe

# uSockets:

uSockets/uSockets.a:
	cd uSockets ; make $(MFLAGS) $(USOCKETSFLAGS)


# objects:
bin/tests/%.o: tests/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)
#bin/networking/%.o: src/networking/%.cpp src/networking/%.hpp
#	$(CXX) -c -o $@ $< $(CXXFLAGS)
bin/%.o: src/%.cpp src/%.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

.PHONY: clean
clean:
	(cd uSockets ; make clean)
	$(RM) $(OBJECTS) $(LIBFILE) $(TESTS) bin/tests/*

