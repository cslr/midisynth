
#include <stdio.h>
#include <iostream>
#include <dinrhiw/dinrhiw.h>
#include "MidiFile.h"
#include "playmidi.h"
#include "getmidifiles.h"
#include "loadmidifile.h"



int main(int argc, char** argv)
{
  std::cout << "GENERATE NOTE SAMPLES FROM MIDI FILES (*.MID)" << std::endl;
  
  std::vector<std::string> midiFiles;
  whiteice::math::matrix<> midi;
  
  getDirectoryMidiFiles("good", midiFiles); // tests getting of .mid filenames
  
  const double TICK_LENGTH = 0.050; // 50ms
  
  const int TICKS_PER_SECOND = (int)(1.0/TICK_LENGTH);
  
  printf("TICKS_PER_SECOND: %d\n", TICKS_PER_SECOND);
  
  whiteice::dataset<> db;  
  
  unsigned int filesProcessed = 0;
  unsigned int datapoints = 0;

  if(midiFiles.size() > 5)
    midiFiles.resize(5);

  for(auto& file : midiFiles){
    if(loadMIDIFile(file, midi, TICK_LENGTH) == false){
      std::cout << "ERROR: Couldn't load file: " << file << std::endl;
      continue;
    }

    printf("Processing file %s (%d ticks)...\n", file.c_str(), midi.xsize());

    bool midiHasData = false;

    std::vector< whiteice::math::vertex<> > timeserie;
    
    for(int i=0;i<midi.xsize();i++){ // TODO: use 5 ticks long time-series
      whiteice::math::vertex<> notes;
      notes.resize(midi.ysize());
      notes.zero();

      for(unsigned int k=0;k<midi.ysize();k++){
	// DISCRETIZES NOTES TO 0,1 VALUED NOTES [> 25% of max volume means note on]
	if(midi(k, i) >= 0.25)
	  notes[k] = 1.0;
	else
	  notes[k] = 0.0;
      }

      timeserie.push_back(notes);

      datapoints++;
      midiHasData = true;
    }
    
    if(midiHasData){
      db.createCluster(file, midi.ysize());
      db.add(filesProcessed, timeserie);
      
      filesProcessed++;
    }
  }
  
  printf("%d MIDI files processed. Total %d datapoints.\n",
	 filesProcessed, datapoints);
  
  // now we have processed all midi files and save the (input,output) pairs to disk
  if(db.save("models/midinotes.ds") == true)
    printf("Database saved successfully.\n");
  else
    printf("Saving database FAILED.\n");
  
  
  
  return 0;  
}

