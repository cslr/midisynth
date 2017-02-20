
#ifndef playmidi_h
#define playmidi_h

#include <dinrhiw/dinrhiw.h>

// plays midi matrix (for testing) using piano (and bass)
void playMIDIMatrix(const whiteice::math::matrix<>& midi,
		    const double tick_length = 0.100);

// generates midi matrix (piano roll) using trained RNN_RBM
bool synthesizeMIDI(whiteice::RNN_RBM<>& rbm,
		    whiteice::math::matrix<>& midi,
		    const double midi_length = 60.0, // in seconds
		    const double tick_length = 0.100); // in seconds (100ms)


#endif
