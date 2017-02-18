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

CFLAGS=-std=c++1y -O3 -g -fopenmp -I. -I../../midifile.git/trunk/include `pkg-config dinrhiw --cflags` `pkg-config fluidsynth --cflags` -D_GLIBCXX_DEBUG
CXXFLAGS=$(CFLAGS)

OBJECTS=midisynth.o playmidi.o getmidifiles.o
MIDI_SOURCES=midisynth.cpp playmidi.cpp getmidifiles.cpp
TARGET=midiplay

DB_OBJECTS=main.o playmidi.o getmidifiles.o
DB_SOURCES=main.cpp playmidi.cpp getmidifiles.cpp
DB_TARGET=midi_database

LIBS=-L../../midifile.git/trunk/lib/ -lmidifile `pkg-config dinrhiw --libs` \
	`pkg-config fluidsynth --libs`

RBM_OBJECTS=rbm_test.o
RBM_SOURCES=rbm_test.cpp
RBM_TARGET=rbm_step

NN_OBJECTS=nn_step.o
NN_SOURCES=nn_step.cpp
NN_TARGET=nn_step

SOURCES=$(DB_SOURCES) $(RBM_SOURCES) $(MIDI_SOURCES) $(NN_SOURCES)

##################################################

all: $(OBJECTS) $(TARGET) $(RBM_TARGET) $(NN_TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

$(DB_TARGET): $(DB_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(DB_TARGET) $(DB_OBJECTS) $(LIBS)

$(RBM_TARGET): $(RBM_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(RBM_TARGET) $(RBM_OBJECTS) $(LIBS)

$(NN_TARGET): $(NN_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(NN_TARGET) $(NN_OBJECTS) $(LIBS)

clean:
	$(RM) $(OBJECTS) $(RBM_OBJECTS) $(NN_OBJECTS)
	$(RM) $(TARGET) $(RBM_TARGET)
	$(RM) *~

depend:
	$(CXX) $(CXXFLAGS) -MM $(SOURCES) $(RBM_SOURCES) > Makefile.depend


############################################################

include Makefile.depend
