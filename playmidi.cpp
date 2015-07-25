
#include <stdio.h>
#include <dinrhiw/dinrhiw.h>

#include "playmidi.h"
#include "fluidsynth.h"



// plays midi matrix (for testing) using piano and bass
void playMIDIMatrix(const whiteice::math::matrix<>& midi)
{
  fluid_settings_t* settings = 0;
  fluid_synth_t* synth = 0;
  fluid_audio_driver_t* adriver = 0;
  int soundfont = FLUID_FAILED;
  
  settings = new_fluid_settings();
  fluid_settings_setstr(settings, "audio.driver", "pulseaudio");
  fluid_settings_setint(settings, "synth.midi-channels", 60);
  fluid_settings_setnum(settings, "synth.gain", 4.0);
  
  synth = new_fluid_synth(settings);  
  adriver = new_fluid_audio_driver(settings, synth);
  
  // loads soundfont
  soundfont = fluid_synth_sfload(synth, "/usr/share/sounds/sf2/FluidR3_GM.sf2", 0);
  
  bool ok = true;
  
  for(unsigned int i=0;i<60;i++){
    // could use 33 as a soundfont for bass..
    if(fluid_synth_program_select(synth, i, soundfont, 0, 0) != FLUID_OK){
      printf("Setting synth channel sound failed (channel %d).\n", i+1);
      ok = false;
    }
  }
  
  if(synth && adriver && soundfont != FLUID_FAILED && ok == true){
    
    // plays MIDI matrix
    for(unsigned int tick=0;tick<midi.xsize();){
      unsigned int channel = 0;
      
      unsigned long long startTime = 
      std::chrono::duration_cast<std::chrono::milliseconds>
      (std::chrono::system_clock::now().time_since_epoch()).count();
      
      // starts playback of new elements
      for(unsigned int note=0;note<midi.ysize();note++){
	double prevvolume = 0.0;
	if(tick >= 1) prevvolume = midi(note, tick-1).c[0];
	double volume = midi(note, tick).c[0];
	
	if(volume > 0.0 && prevvolume <= 0.0){ // we have something new to play
	  // converts note to MIDI note
	  int midinote = note+81-24;
	  int velocity = (int)(volume*127.0);
	  int channel  = note;
	  
	  // printf("T%d NOTE ON: %d %d %d\n", note, channel, midinote, velocity);
	  
	  if(velocity  > 0){
	    if(fluid_synth_noteon(synth, channel, midinote, velocity) != FLUID_OK){
	      // printf("FLUID NOTE ON FAILED: %d %d\n", note, velocity);
	      fflush(stdout);
	    }
	    else{
	      // printf("FLUID NOTE ON: %d %d\n", note, velocity);
	      // fflush(stdout);
	    }
	  }
	  
	  if(channel == 10) channel++; // we skip channel 10
	}
	else if(volume <= 0.0 && prevvolume > 0.0){
	  // we need to stop playing previous note
	  int midinote = note+81-24;
	  int channel  = note;
	  int velocity = (int)(prevvolume*127.0);
	  
	  if(velocity > 0){
	    if(fluid_synth_noteoff(synth, channel, midinote) != FLUID_OK){
	      // printf("FLUID NOTE OFF FAILED: %d\n", note);
	      // fflush(stdout);
	    }
	    else{
	      // printf("FLUID NOTE OFF: %d\n", note);
	      // fflush(stdout);
	    }
	  }
	}
      }
      
      unsigned long long endTime = 
      std::chrono::duration_cast<std::chrono::milliseconds>
      (std::chrono::system_clock::now().time_since_epoch()).count();

      auto delta = endTime - startTime;
      
      // sleep for the next tick (if loop took longer than 25msecs warn and continue)
      if(delta < 100){
	std::chrono::milliseconds duration(100 - delta);
	std::this_thread::sleep_for(duration);
	tick++;
      }
      else{
	printf("WARNING: FLUIDSYNTH MIDI PLAYING TOO SLOW OUT OF SYNC: %d MSECS PER LOOP\n", delta);
	tick += delta/100;
      }
    }
    
    
    
  }
  
  if(adriver) delete_fluid_audio_driver(adriver);
  if(soundfont != FLUID_FAILED) fluid_synth_sfunload(synth, soundfont, 1);
  if(synth) delete_fluid_synth(synth);
  if(settings) delete_fluid_settings(settings);
}
