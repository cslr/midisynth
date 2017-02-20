
#include "playmidi.h"

#include <stdio.h>




int main(int argc, char** argv)
{
  whiteice::RNN_RBM<> rbm;

  if(rbm.load("models/midi-rbm-rnn.conf") == false){
    printf("ERROR: loading RNN-RBM model failed.\n");
    return -1;
  }

  whiteice::math::matrix<> midi;

  if(synthesizeMIDI(rbm, midi, 60.0, 0.050) == false){
    printf("ERROR: synthesizing MIDI notes failed.\n");
    return -1;
  }

  // plays synthesized song

  playMIDIMatrix(midi, 0.050);

  return 0;
}
