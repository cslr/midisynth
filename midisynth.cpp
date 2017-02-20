
#include <stdio.h>
#include <iostream>
#include <dinrhiw/dinrhiw.h>
#include "MidiFile.h"
#include "playmidi.h"
#include "getmidifiles.h"
#include "loadmidifile.h"


int main(int argc, char** argv)
{
  std::cout << "PLAY MIDI MATRIX" << std::endl;

  std::string filename = "MissionImpossible.mid";
  
  if(argc >= 2)  
    filename = argv[1];
  
  whiteice::math::matrix<> midi;

  double ticklen = 0.050; // 50ms
  
  if(loadMIDIFile(filename, midi, ticklen) == false){
    return -1;
  }

  printf("MIDI PIANO ROLL SIZE %d x %d (NOTES x TICKS)\n",
	 midi.ysize(), midi.xsize());

  // plays midi matrix (for testing) using piano only
  playMIDIMatrix(midi, ticklen); 
  
  return 0;
}


