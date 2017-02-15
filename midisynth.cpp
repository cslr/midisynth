
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
  std::cout << "PLAY MIDI MATRIX" << std::endl;
  
  if(argc != 2) return -1;
  std::string filename = "MissionImpossible.mid";
  filename = argv[1];
  
  whiteice::math::matrix<> midi;
  
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
