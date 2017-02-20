
#ifndef loadmidifile_h
#define loadmidifile_h

#include <stdio.h>
#include <iostream>
#include <dinrhiw/dinrhiw.h>
#include "MidiFile.h"
#include "playmidi.h"
#include "getmidifiles.h"




bool loadMIDIFile(const std::string& filename,
		  whiteice::math::matrix<>& midi,
		  const double tickLength = 0.100);

#endif
