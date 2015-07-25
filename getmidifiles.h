
#ifndef getmidifiles_h
#define getmidifiles_h

#include <vector>
#include <string>

// when given directory: returns vector of .mid files with full path+filename
// returns number of midifiles found (or zero in case of error)
int getDirectoryMidiFiles(const std::string& dir, std::vector<std::string>& midiFiles);


#endif
