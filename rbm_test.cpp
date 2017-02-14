
#include <stdio.h>
#include <iostream>
#include <dinrhiw/dinrhiw.h>



int main(int argc, char** argv)
{
  printf("RBM MIDI NOTES TEST\n");
  
  const int HIDDEN = 10; // number of hidden nodes in RBM networks

  
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

  printf("%d samples training samples loaded\n", db.size(0));

  std::vector<unsigned int> inputArch, outputArch;

  inputArch.push_back(db.dimension(0));
  inputArch.push_back(db.dimension(0)*2);
  inputArch.push_back(HIDDEN);

  outputArch.push_back(db.dimension(1));
  outputArch.push_back(db.dimension(1)*2);
  outputArch.push_back(HIDDEN);
  
  whiteice::DBN<> inputRBM(inputArch);
  whiteice::DBN<> outputRBM(outputArch);
  
  std::vector< whiteice::math::vertex<> > input_samples;
  std::vector< whiteice::math::vertex<> > output_samples;
  
  for(unsigned int i=0;i<db.size(0);i++){
    input_samples.push_back(db.access(0,i));
    output_samples.push_back(db.access(1,i));
  }

  inputRBM.initializeWeights();
  outputRBM.initializeWeights();
  
  //////////////////////////////////////////////////////////////////////
  // now we learn GB-RBM weights
  
  printf("INPUT architecture: %dx%dx%d\n",
	 inputArch[0], inputArch[1], inputArch[2]);
  
  printf("Learning RBM weights (gradient descent data maximum likelihood)..\n");
  fflush(stdout);

  whiteice::math::blas_real<float> dW = 0.0001;

  inputRBM.learnWeights(input_samples, dW, true);

  whiteice::nnetwork<>* net = nullptr;

  if(inputRBM.convertToNNetwork(net) == true){
    if(net->save("midi-input-nnetwork.conf") == false){
      printf("ERROR: saving MIDI input RBM network failed\n");
    }
    
    delete net;
    net = nullptr;
  }
  else{
    printf("ERROR: converting RBM to nnetwork failed,\n");
  }

  printf("OUTPUT architecture: %dx%dx%d\n",
	 outputArch[0], outputArch[1], outputArch[2]);
  
  printf("Learning RBM weights (gradient descent data maximum likelihood)..\n");
  fflush(stdout);

  outputRBM.learnWeights(output_samples, dW, true);

  if(outputRBM.convertToNNetwork(net) == true){
    if(net->save("midi-output-nnetwork.conf") == false){
      printf("ERROR: saving MIDI output RBM network failed\n");
    }
    
    delete net;
    net = nullptr;
  }
  else{
    printf("ERROR: converting RBM to nnetwork failed,\n");
  }

  
  return 0;
}
