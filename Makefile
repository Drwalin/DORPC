
include MakefileSTD/MakefileSTD

AR = ar
CXX = g++
CXXFLAGS = -flto -pipe -std=c++20 -pedantic -Wall -IuSockets/src
LIBS=
LIBFILE=libicon4.a

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

INCLUDE = -I./src/
CXXFLAGS += $(INCLUDE)
OBJECTS = bin/MethodRepository.o bin/Cluster.o

all: $(LIBFILE)

$(LIBFILE): $(OBJECTS) uSockets/uSockets.a msgpack-c/msgpackc.a
	$(AR) rvs $@ $^

# uSockets:

uSockets/uSockets.a:
	make uSockets/Makefile

# msgpack-c:

MSGPACK_FILES = msgpack-c/src/object.o msgpack-c/src/unpack.o
MSGPACK_FILES += msgpack-c/src/version.o msgpack-c/src/vrefbuffer.o
MSGPACK_FILES += msgpack-c/src/zoneo.o

msgpack-c/msgpackc.a: $(MSGPACK_FILES)
	$(AR) rvs $@ $^

msgpack-c/src/%.o: msgpack-c/src/%.c
	$(CXX) -c -o $@ $< $(CXXFLAGS)

# objects:
bin/%.o: src/%.cpp src/%.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

.PHONY: clean
clean:
	(cd uSockets ; make clean)
	$(RM) $(OBJECTS) $(LIBFILE) $(MSGPACK_FILES) msgpack-c/src/msgpac-c.a

