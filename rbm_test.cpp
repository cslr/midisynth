
#include <stdio.h>
#include <iostream>
#include <dinrhiw/dinrhiw.h>



int main(int argc, char** argv)
{
  printf("GB-RBM MIDI NOTES TEST\n");
  
  const int HIDDEN = 10; // number of hidden nodes in GB-RBM network

  
  // loads examples database
  whiteice::dataset<> db;
  
  if(db.load("midinotes.ds") == false){
    printf("ERROR: Couldn't load database.");
    return -1;
  }
  
  if(db.getNumberOfClusters() != 2){
    printf("ERROR: Bad database.\n");
    return -1;
  }
  
  if(db.size(0) != db.size(1) || db.size(0) == 0){
    printf("ERROR: Bad database.\n");
    return -1;
  }
  
  whiteice::GBRBM<> gbrbm;
  
  if(gbrbm.resize(db.dimension(0)+db.dimension(1), HIDDEN) == false){
    printf("ERROR: GBRBM internal error.\n");
    return -1;
  }
  
  std::vector< whiteice::math::vertex<> > samples;
  
  for(unsigned int i=0;i<db.size(0);i++){
    whiteice::math::vertex<> v(db.dimension(0)+db.dimension(1));
    if(v.write_subvertex(db.access(0, i), 0) == false || 
       v.write_subvertex(db.access(1, i), db.dimension(0)) == false){
      printf("Exporting datapoint %d/%d failed.\n", i, db.size(0));
      continue;
    }
    
    samples.push_back(v);
  }
  
  if(gbrbm.initializeWeights() == false){
    printf("ERROR: GBRBM internal error.\n");
    return -1;
  }
  
  //////////////////////////////////////////////////////////////////////
  // now we learn GB-RBM weights
  
  printf("GBRBM architecture: %dx%d\n", gbrbm.getVisibleNodes(), gbrbm.getHiddenNodes());
  printf("Direct learning GBRBM weights (gradient descent data maximum likelihood)..\n");
  fflush(stdout);
  
  auto error = gbrbm.learnWeights(samples, 500, true, true);
  
  printf("GBRBM optimization finished. Reconstruction error: %f\n", error.c[0]);
  fflush(stdout);
  
  return 0;
}
