
include MakefileSTD/MakefileSTD

AR = ar
CXX = g++
CXXFLAGS = -flto -pipe -std=c++20 -pedantic -Wall -IuSockets/src
LIBS=
LIBFILE=libdorpc.a

ifeq ($(DEBUG),1)
	CXXFLAGS += -ggdb -g -pg
else
	CXXFLAGS += -Ofast -s
endif

ifeq ($(platform),win)
	LIBS += -lzlib
	LIBS += -lgcc_s_seh-1 -lmfc100
	LIBS += -lmfc100u -lmsvcp100 -lmsvcr100 -lmsvcr100_clr0400
else
	LIBS += -L/usr/lib -ldl -lz
	CXXFLAGS += -fPIC
	CXXFLAGS += -I/usr/include
endif
LIBS += -lpthread -lssl -lcrypto

INCLUDE = -I./src/ -I./Concurrent -I./uSockets/include -I./msgpack-c/include
CXXFLAGS += $(INCLUDE)
OBJECTS = bin/MethodRepository.o bin/Cluster.o
OBJECTS += bin/networking/Buffer.o bin/networking/Socket.o
OBJECTS += bin/networking/Context.o bin/networking/Loop.o

all: $(LIBFILE)

$(LIBFILE): $(OBJECTS) uSockets/uSockets.a msgpack-c/libmsgpackc.a
	$(AR) rvs $@ $^

# uSockets:

uSockets/uSockets.a:
	make uSockets/Makefile

# msgpack-c:

msgpack-c/libmsgpackc.a: msgpack-c/include/msgpack/sysdep.h $(MSGPACK_FILES)
	(cd msgpack-c ; cmake . ; make)

# objects:
bin/%.o: src/%.cpp src/%.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

.PHONY: clean
clean:
	(cd uSockets ; make clean)
	$(RM) $(OBJECTS) $(LIBFILE) $(MSGPACK_FILES) msgpack-c/src/msgpac-c.a

