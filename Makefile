# MIDI notes generator
# GNU Makefile

CC = gcc
CXX = g++
MAKE = make
MKDIR = mkdir
AR = ar
CD = cd
RM = rm -f
MV = mv
CP = cp

CFLAGS=-std=c++1y -O3 -g -fopenmp -I. -I/home/rpls/src/midifile-head/include `pkg-config dinrhiw --cflags` -Ifluidsynth-headers
CXXFLAGS=$(CFLAGS)

OBJECTS=main.o playmidi.o getmidifiles.o
SOURCES=main.cpp playmidi.cpp getmidifiles.cpp
TARGET=midisynth

LIBS=-L/home/rpls/src/midifile-head/lib/ -lmidifile `pkg-config dinrhiw --libs` \
	/usr/lib/x86_64-linux-gnu/libfluidsynth.so.1

RBM_OBJECTS=rbm_test.o
RBM_SOURCES=rbm_test.cpp
RBM_TARGET=rbm_test

##################################################

all: $(OBJECTS) $(TARGET) $(RBM_TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

$(RBM_TARGET): $(RBM_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(RBM_TARGET) $(RBM_OBJECTS) $(LIBS)

clean:
	$(RM) $(OBJECTS) $(RBM_OBJECTS)
	$(RM) $(TARGET) $(RBM_TARGET)
	$(RM) *~

depend:
	$(CXX) $(CXXFLAGS) -MM $(SOURCES) $(RBM_SOURCES) > Makefile.depend


############################################################

include Makefile.depend
