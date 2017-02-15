
#include <stdio.h>
#include <iostream>
#include <dinrhiw/dinrhiw.h>
#include "MidiFile.h"
#include "playmidi.h"
#include "getmidifiles.h"
#include "midinames.h"

/*
 * loads midi filename into midi notes matrix (60 NOTE VALUES (A-2 - A-7), K ticks) with tick length 100ms
 */
bool loadMIDIFile(const std::string& filename, 
		  whiteice::math::matrix<>& midi, 
		  const double tickLength = 0.100);



int main(int argc, char** argv)
{
  std::cout << "GENERATE MIDI SAMPLES FROM MID-FILES" << std::endl;
  
  // if(argc != 2) return -1;
  std::string filename = "MissionImpossible.mid";
  // filename = argv[1];
  
  std::vector<std::string> midiFiles;
  whiteice::math::matrix<> midi;
  
  getDirectoryMidiFiles("good", midiFiles); // tests getting of .mid filenames
  
  const double TICK_LENGTH = 0.100; // 100ms
  
  const int TICKS_PER_SECOND = (int)(1.0/TICK_LENGTH);
  
  printf("TICKS_PER_SECOND: %d\n", TICKS_PER_SECOND);
  
  whiteice::dataset<> db;
  db.createCluster("input notes (4 secs)", 60*TICKS_PER_SECOND*4);
  db.createCluster("output notes (1 sec)", 60*TICKS_PER_SECOND*1);
  
  unsigned int filesProcessed = 0;
  unsigned int datapoints = 0;
  
  for(auto& file : midiFiles){
    if(loadMIDIFile(file, midi, TICK_LENGTH) == false){
      std::cout << "ERROR: Couldn't load file: " << file << std::endl;
      continue;
    }
    
    printf("Processing file %s (%d ticks)...\n", file.c_str(), midi.xsize());
    
    bool midiHasData = false;
    
    for(int i=0;i<midi.xsize();i+=TICKS_PER_SECOND){
      // now we take 5 seconds snapshots every one second until end of 
      // the midi file and store them to dataset
      whiteice::math::matrix<> input;
      whiteice::math::matrix<> output;
      
      input.resize(60, TICKS_PER_SECOND*4);
      output.resize(60, TICKS_PER_SECOND*1);
      
      input.zero();
      output.zero();
      
      // i is starting position of OUTPUT in ticks [we go backwards -4*TICKS_PER_SECOND ..if we can]
      if(i + TICKS_PER_SECOND > midi.xsize()) continue; // cannot will output fully
      
      for(int n=0;n<60;n++){
	for(int t=0;t<TICKS_PER_SECOND;t++){
	  output(n,t) = midi(n,i+t);
	}
      }
      
      int midi_tick_start = i - TICKS_PER_SECOND*4;
      int input_tick_start  = 0;
      
      if(midi_tick_start < 0){
	input_tick_start = -midi_tick_start;
	midi_tick_start  = 0;
      }

      for(int n=0;n<60;n++){
	int t2 = input_tick_start;
	for(int t=midi_tick_start;t<i;t++,t2++){
	  input(n,t2) = midi(n,t);
	}
      }
      
      // we now have 4 second input note track sequence and 1 second note ouput matrix
      whiteice::math::vertex<> in, out;
      if(input.save_to_vertex(in) == false || output.save_to_vertex(out) == false){
	printf("ERROR: Matrix operation internal error.\n");
	continue;
      }
      
      if(db.add(0, in) == false || db.add(1, out) == false){
	printf("ERROR: Storing note sequence to database failed.\n");
	continue;
      }
      
      datapoints++;
      midiHasData = true;
    }
    
    if(midiHasData)
      filesProcessed++;
  }
  
  printf("%d MIDI files processed. Total %d datapoints.\n", filesProcessed, datapoints);
  
  // now we have processed all midi files and save the (input,output) pairs to disk
  if(db.save("models/midinotes.ds") == true) printf(
					     "Database saved successfully.\n");
  else
    printf("Saving database FAILED.\n");
  
  
  
  return 0;
  
#if 1

  if(loadMIDIFile(filename, midi) == false){
    return -1;
  }
  
#else
  
  // now we convert MIDI notes vector into a matrix array
  const int NUMNOTES = 60;  
  
  midi.resize(NUMNOTES, 1000); // midi[note][timestep (100ms steps)] => 10 seconds
  
  for(unsigned int j=0;j<NUMNOTES;j++){
    for(unsigned int i=0;i<midi.xsize();i++){
      if((rand() & 7) != 0)
	midi(j, i) = 0.00;
      else
	midi(j, i) = 0.75;
    }
  }
  
#endif
  
  playMIDIMatrix(midi); // plays midi matrix (for testing) using piano and bass
  
  return 0;
}


/*
 * loads midi filename into midi notes matrix (60 NOTE VALUES (A-2 - A-7), K ticks) with tick length 100ms
 */
bool loadMIDIFile(const std::string& filename, whiteice::math::matrix<>& midi, const double tickLength)
{
  MidiFile midifile(filename);
  
  if(midifile.status() == false){
    std::cout << "MIDI file loading failed: " << filename << std::endl;
    return false;
  }
  
  if(midifile.getTrackCount() <= 0){
    std::cout << "No MIDI tracks to process." << std::endl;
    return false;
  }
  
  // printf("%d tracks in file (%s)\n", midifile.getTrackCount(), filename.c_str());
  
  midifile.linkNotePairs();
  midifile.doTimeAnalysis();
  
  bool verbose = false;
  
  // 100ms accuracy in MIDI notes matrix [BAD but ok until we get enough memory]
  const double TICK_LENGTH = tickLength; 
  const int NUMNOTES = 60; // A-2 - G-7
  std::vector< std::vector< double > > notes; // notes[tick][note]
  
  int numNotes = 0;
  
 
  for(unsigned int t=0;t<midifile.getTrackCount();t++){
    MidiEventList& track = midifile[t];
    
    int timbre = 0;
    bool isDrumTrack = false;
    if(t+1 == 10) isDrumTrack = true; // in General MIDI track 10 is drum track..
    
    for(unsigned int i=0;i<track.getSize();i++){
      MidiEvent& event = track[i];
      
      if(event.isTimbre()){ // MIDI patch change [instrument]
	timbre = event.getP1(); // is this correct?? [
	if(verbose)
	  printf("TRACK %d: TIMBRE CHANGE %s (%d)\n", 
		 t+1, timbreName[timbre], timbre);
	// if track 10 has a timbre change then it cannot be drum track
	isDrumTrack = false;
      }
      
      if(event.isNoteOn()){
	
	numNotes++;
	
	if(isDrumTrack == false){ // not a drum track..

	  int keynumber = event.getKeyNumber(); // 60 is middle-c
	  
	  // will transform A-4 note to have value 0
	  keynumber -= 81;
	  
	  // we keep only range [A-2 .. A-6] (4*12 = 48 notes)
	  // [A-2 (-24) A-3 (-12) A-4 (0) A-5 (12) A-6 (24), A-7]
	  if(keynumber < -24 || keynumber >= +36)
	    continue; // ingore keys below A-2 and above G-7 (A-7 is upper limit)
	  
	  // converts keynumber to musical keys
	  int octave = 5 + (keynumber + 2) / 12;
	  int key    = abs((keynumber + 2) % 12);
	  
	  const char* keyToNote[12] = 
	    { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
	  
	  if(verbose)
	    printf("TRACK %d NOTE %s %s-%d: %.2f SECS: DURA: %.2f secs VELO: %.2f\n", 
		   t+1, timbreName[timbre], keyToNote[key], octave, 
		   event.seconds, event.getDurationInSeconds(),
		   ((double)event.getVelocity())/127.0);
	  
	  // we add notes to note matrix [startTick, endTick]
	  int startTick  = floor(event.seconds/TICK_LENGTH);
	  double endTime = event.seconds + event.getDurationInSeconds();
	  int endTick   = ceil(endTime/TICK_LENGTH);
	  double velocity = ((double)event.getVelocity())/127.0;

	  if(notes.size() <= endTick)
	    notes.resize(endTick+1);
	  
	  double startStrength = (event.seconds - startTick*TICK_LENGTH)/TICK_LENGTH;
	  double endStrength   = (endTick*TICK_LENGTH - endTime)/TICK_LENGTH;
	  
	  if(notes[startTick].size() < NUMNOTES){
	    notes[startTick].resize(NUMNOTES);
	    for(unsigned int i=0;i<NUMNOTES;i++)
	      notes[startTick][i] = 0.0;
	  }
	  
	  if(notes[endTick].size() < NUMNOTES){
	    notes[endTick].resize(NUMNOTES);
	    for(unsigned int i=0;i<NUMNOTES;i++)
	      notes[endTick][i] = 0.0;
	  }
	  
	  notes[startTick][keynumber+24] += startStrength*velocity;
	  notes[endTick][keynumber+24]   += endStrength*velocity;
	  
	  for(int i=startTick+1;i<endTick;i++){
	    if(notes[i].size() < NUMNOTES)
	      notes[i].resize(NUMNOTES);
	    for(unsigned int n=0;n<NUMNOTES;n++)
	      notes[i][n] = 0.0;
	    
	    notes[i][keynumber+24] += velocity;
	  }
	}
	else{
	  // General MIDI channel 10 is for drums..
	  
	  int keynumber = event.getKeyNumber(); // values are drum elements now..
	  
	  if(keynumber < 35 || keynumber > 81)
	    continue; // unknown drum element
	  
	  if(verbose)
	    printf("TRACK %d DRUM %s (%d): %.2f SECS: DURA: %.2f secs VELO: %.2f\n", 
		   t+1, keyToDrum[keynumber], keynumber,
		   event.seconds, event.getDurationInSeconds(),
		   ((double)event.getVelocity())/127.0);
	  
	}

      }
    }
    
  }
  
  
  if(numNotes == 0) // MIDI files contained no notes..
    return false;
  
  
  // now we convert MIDI notes vector into a matrix array
  
  midi.resize(NUMNOTES, notes.size()); // midi[note][timestep (100ms steps)]
  
  // printf("MIDI NOTE MATRIX SIZE: %d x %d\n", NUMNOTES, notes.size());
  
  for(unsigned int j=0;j<NUMNOTES;j++){
    for(unsigned int i=0;i<notes.size();i++){
      if(notes[i].size() < NUMNOTES){
	midi(j,i) = 0.0;
      }
      else{
	if(notes[i][j] < 0.0)
	  midi(j,i) = 0.0;
	else if(notes[i][j] > 1.0)
	  midi(j,i) = 1.0;
	else
	  midi(j,i) = notes[i][j];
      }
    }
  }
  
  // we have now full note matrix for the MIDI file
  // printf("MIDI NOTE MATRIX GENERATED\n");
  // fflush(stdout);  
  
  return true;
}
