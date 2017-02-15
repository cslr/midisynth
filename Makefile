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

CFLAGS=-std=c++1y -O3 -g -fopenmp -I. -I../../midifile.git/trunk/include `pkg-config dinrhiw --cflags` `pkg-config fluidsynth --cflags`
CXXFLAGS=$(CFLAGS)

OBJECTS=main.o playmidi.o getmidifiles.o
SOURCES=main.cpp playmidi.cpp getmidifiles.cpp
TARGET=midi_database

LIBS=-L../../midifile.git/trunk/lib/ -lmidifile `pkg-config dinrhiw --libs` \
	`pkg-config fluidsynth --libs`

RBM_OBJECTS=rbm_test.o
RBM_SOURCES=rbm_test.cpp
RBM_TARGET=rbm_step

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
