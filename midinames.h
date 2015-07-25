// include this file only ONCE per executable..

#ifndef midinames_h
#define midinames_h


const char* timbreName[128] =
{
  // Piano Timbres
  "Acoustic Grand Piano", // 1
  "Bright Acoustic Piano", // 2
  "Electric Grand Piano", // 3
  "Honky-tonk Piano", // 4
  "Rhodes Piano", // 5
  "Chorused Piano", // 6
  "Harpsichord", // 7
  "Clavinet", // 8
  
  // Chromatic Timbres
  "Celesta", // 9  
  "Glockenspiel",
  "Music Box",
  "Vibraphone",
  "Marimba",
  "Xylophone",
  "Tubular Bells",
  "Dulcimer", // 16
  
  // Organ Timbers
  "Hammond Organ", // 17
  "Percussive Organ",
  "Rock Organ",
  "Church Organ",
  "Reed Organ",
  "Accordion",
  "Harmonica",
  "Tango Accordion",
  
  // Guitar Timbres:
  "Acoustic Nylon Guitar", // 25
  "Acoustic Steel Guitar", // 26
  "Electric Jazz Guitar", // 27
  "Electric Clean Guitar", // 28
  "Electric Muted Guitar", // 29
  "Overdriven Guitar", // 30
  "Distortion Guitar", // 31
  "Guitar Harmonics", // 32
  
  // Bass Timbres:
  "Acoustic Bass", // 33
  "Fingered Electric Bass", // 34
  "Plucked Electric Bass", // 35
  "Fretless Bass", // 36
  "Slap Bass 1",
  "Slap Bass 2",
  "Synth Bass 1",
  "Synth Bass 2",
  
  // String Timbres:
  "Violin",
  "Viola",
  "Cello",
  "Contrabass",
  "Tremolo Strings",
  "Pizzicato Strings",
  "Orchestral Harp",
  "Timpani",
 
  // Ensemble Timbres:
  "String Ensemble 1",
  "String Ensemble 2",
  "Synth Strings 1",
  "Synth Strings 2",
  "Choir \"Aah\"",
  "Choir \"Ooh\"",
  "Synth Voice",
  "Orchestral Hit",
  
  // Brass Timbres:
  "Trumpet",
  "Trombone",
  "Tuba",
  "Muted Trumpet",
  "French Horn",
  "Brass Section",
  "Synth Brass 1",
  "Synth Brass 2",

  // Reed Timbres:
  "Soprano Sax",
  "Alto Sax",
  "Tenor Sax",
  "Baritone Sax",
  "Oboe",
  "English Horn",
  "Bassoon",
  "Clarinet",
 
  // Pipe Timbres:
  "Piccolo",
  "Flute",
  "Recorder",
  "Pan Flute",
  "Bottle Blow",
  "Shakuhachi",
  "Whistle",
  "Ocarina",
  
  // Synth Lead:
  "Square Wave Lead",
  "Sawtooth Wave Lead",
  "Calliope Lead",
  "Chiff Lead",
  "Charang Lead",
  "Voice Lead",
  "Fifths Lead",
  "Bass Lead",
  
  // Synth Pad:
  "New Age Pad",
  "Warm Pad",
  "Polysynth Pad",
  "Choir Pad",
  "Bowed Pad",
  "Metallic Pad",
  "Halo Pad",
  "Sweep Pad",
 
  // Synth Effects:
  "Rain Effect", // 97
  "Soundtrack Effect",
  "Crystal Effect",
  "Atmosphere Effect",
  "Brightness Effect",
  "Goblins Effect",
  "Echoes Effect",
  "Sci-Fi Effect", // 104
  
  // Ethnic Timbres:
  "Sitar",
  "Banjo",
  "Shamisen",
  "Koto",
  "Kalimba",
  "Bagpipe",
  "Fiddle",
  "Shanai",
  
  // Sound Effects:
  "Tinkle Bell", // 113
  "Agogo",
  "Steel Drums",
  "Woodblock",
  "Taiko Drum",
  "Melodic Tom",
  "Synth Drum",
  "Reverse Cymbal",
  
  "Guitar Fret Noise",
  "Breath Noise", 
  "Seashore",
  "Bird Tweet",
 
  "Telephone Ring",
  "Helicopter",
  "Applause",
  "Gun Shot"
};


// total of 47 different drum values in general midi
const char* keyToDrum[128] = 
{
  "UNKNOWN DRUM", // 0
  "UNKNOWN DRUM", // 1
  "UNKNOWN DRUM", // 2
  "UNKNOWN DRUM", // 3
  "UNKNOWN DRUM", // 4
  "UNKNOWN DRUM", // 5
  "UNKNOWN DRUM", // 6
  "UNKNOWN DRUM", // 7
  "UNKNOWN DRUM", // 8
  "UNKNOWN DRUM", // 9
  "UNKNOWN DRUM", // 10
  "UNKNOWN DRUM", // 11
  "UNKNOWN DRUM", // 12
  "UNKNOWN DRUM", // 13
  "UNKNOWN DRUM", // 14
  "UNKNOWN DRUM", // 15
  "UNKNOWN DRUM", // 16
  "UNKNOWN DRUM", // 17
  "UNKNOWN DRUM", // 18
  "UNKNOWN DRUM", // 19
  "UNKNOWN DRUM", // 20
  "UNKNOWN DRUM", // 21
  "UNKNOWN DRUM", // 22
  "UNKNOWN DRUM", // 23
  "UNKNOWN DRUM", // 24
  "UNKNOWN DRUM", // 25
  "UNKNOWN DRUM", // 26
  "UNKNOWN DRUM", // 27
  "UNKNOWN DRUM", // 28
  "UNKNOWN DRUM", // 29
  "UNKNOWN DRUM", // 30
  "UNKNOWN DRUM", // 31
  "UNKNOWN DRUM", // 32
  "UNKNOWN DRUM", // 33
  "UNKNOWN DRUM", // 34
  "Acoustic Bass Drum", // 35
  "Bass Drum 1", // 36
  "Side Stick", // 37
  "Acoustic Snare", // 38
  "Hand Clap", // 39
  "Electric Snare", // 40
  "Low Floor Tom", // 41
  "Closed High Hat", // 42
  "High Floor Tom", // 43
  "Pedal High Hat", // 44
  "Low Tom", // 45
  "Open High Hat", // 46
  "Low Mid Tom", // 47
  "High Mid Tom", // 48
  "Crash Cymbal 1", // 49
  "High Tom", // 50
  "Ride Cymbal 1", // 51
  "Chinese Cymbal", // 52
  "Ride Bell", // 53
  "Tambourine", // 54
  "Splash Cymbal", // 55
  "Cowbell", // 56
  "Crash Cymbal 2", // 57
  "Vibraslap", // 58
  "Ride Cymbal 2", // 59
  "High Bongo", // 60
  "Low Bongo", // 61
  "Mute High Conga", // 62
  "Open High Conga", // 63
  "Low Conga", // 64
  "High Timbale", // 65
  "Low Timbale", // 66
  "High Agogo", // 67
  "Low Agogo", // 68
  "Cabasa", // 69
  "Maracas", // 70
  "Short Whistle", // 71
  "Long Whistle", // 72
  "Short Guiro", // 73
  "Long Guiro", // 74
  "Claves", // 75
  "High Wood Block", // 76
  "Low Wood Block", // 77
  "Mute Cuica", // 78
  "Open Cuica", // 79
  "Mute Triangle", // 80
  "Open Triangle", // 81
  "UNKNOWN DRUM", // 82
  "UNKNOWN DRUM", // 83
  "UNKNOWN DRUM", // 84
  "UNKNOWN DRUM", // 85
  "UNKNOWN DRUM", // 86
  "UNKNOWN DRUM", // 87
  "UNKNOWN DRUM", // 88
  "UNKNOWN DRUM", // 89
  "UNKNOWN DRUM", // 90
  "UNKNOWN DRUM", // 91
  "UNKNOWN DRUM", // 92
  "UNKNOWN DRUM", // 93
  "UNKNOWN DRUM", // 94
  "UNKNOWN DRUM", // 95
  "UNKNOWN DRUM", // 96
  "UNKNOWN DRUM", // 97
  "UNKNOWN DRUM", // 98
  "UNKNOWN DRUM", // 99
  "UNKNOWN DRUM", // 100
  "UNKNOWN DRUM", // 101
  "UNKNOWN DRUM", // 102
  "UNKNOWN DRUM", // 103
  "UNKNOWN DRUM", // 104
  "UNKNOWN DRUM", // 105
  "UNKNOWN DRUM", // 106
  "UNKNOWN DRUM", // 107
  "UNKNOWN DRUM", // 108
  "UNKNOWN DRUM", // 109
  "UNKNOWN DRUM", // 110  
  "UNKNOWN DRUM", // 111
  "UNKNOWN DRUM", // 112
  "UNKNOWN DRUM", // 113
  "UNKNOWN DRUM", // 114
  "UNKNOWN DRUM", // 115
  "UNKNOWN DRUM", // 116
  "UNKNOWN DRUM", // 117
  "UNKNOWN DRUM", // 118
  "UNKNOWN DRUM", // 119
  "UNKNOWN DRUM", // 120
  "UNKNOWN DRUM", // 121
  "UNKNOWN DRUM", // 122
  "UNKNOWN DRUM", // 123
  "UNKNOWN DRUM", // 124
  "UNKNOWN DRUM", // 125
  "UNKNOWN DRUM", // 126
  "UNKNOWN DRUM"  // 127
};


#endif
