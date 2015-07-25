
#include "getmidifiles.h"

#include <dirent.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>


// when given directory: returns vector of .mid files with full path+filename
// returns number of midifiles found (or zero in case of error)

int getDirectoryMidiFiles(const std::string& dirname, std::vector<std::string>& midiFiles)
{
  midiFiles.clear();
  
  DIR *dir = NULL;
  struct dirent *ent = NULL;
  
  if ((dir = opendir (dirname.c_str())) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
      const char* filename = ent->d_name;
      
      if(strlen(filename) > 4)
	if(strcmp(&(filename[strlen(filename)-4]),".mid") == 0  || 
	   strcmp(&(filename[strlen(filename)-4]),".MID") == 0)
	{
	  std::string relpath = dirname + "/" + filename;
	  char *p = realpath(relpath.c_str(), NULL);
	  
	  if(p != NULL){
	    std::string fullpath = p;
	    free(p);
	    midiFiles.push_back(fullpath);
	  }
	  else{
	    midiFiles.push_back(relpath);
	  }
	}
	  
    }
    closedir (dir);
  }
  else { /* could not open directory */
    return 0;
  }
  
  return midiFiles.size();
}

