
#include "loadmidifile.h"
#include "midinames.h"


/*
 * loads midi filename into midi notes matrix (piano roll) 
 * (128 NOTE VALUES (C-0 - G-10), K ticks with tick length 100ms
 */
bool loadMIDIFile(const std::string& filename,
		  whiteice::math::matrix<>& midi,
		  const double tickLength)
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
  midifile.sortTracks();
  
  const bool verbose = false;
  
  // 100ms accuracy in MIDI notes matrix [BAD but ok until we get enough memory]
  const double TICK_LENGTH = tickLength; 
  const int NUMNOTES = 128; // A-2 - G-7
  std::vector< std::vector< double > > notes; // notes[tick][note]
  
  int numNotes = 0;
  
 
  for(unsigned int t=0;t<midifile.getTrackCount();t++){
    MidiEventList& track = midifile[t];
    
    int timbre = 0;
    bool isDrumTrack = false;
    if(t+1 == 10) isDrumTrack = true; // in General MIDI track 10 is drum track..
    
    for(unsigned int i=0;i<track.getSize();i++){
      MidiEvent& event = track[i];

      // event.seconds = midifile.getTimeInSeconds(event.tick);

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
	  
	  notes[startTick][keynumber+81] += startStrength*velocity;
	  notes[endTick][keynumber+81]   += endStrength*velocity;
	  
	  for(int i=startTick+1;i<endTick;i++){
	    if(notes[i].size() < NUMNOTES){
	      notes[i].resize(NUMNOTES);
	      for(unsigned int n=0;n<NUMNOTES;n++)
		notes[i][n] = 0.0;
	    }
	    
	    notes[i][keynumber+81] += velocity;
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
