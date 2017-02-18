
#include <stdio.h>
#include <iostream>
#include <dinrhiw/dinrhiw.h>



int main(int argc, char** argv)
{
  printf("RBM MIDI NOTES MODEL\n");

  // number of hidden nodes in RBM networks
  const int HIDDEN = 10; // was 50

  
  // loads examples database
  whiteice::dataset<> db;
  
  if(db.load("models/midinotes.ds") == false){
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

  printf("%d training samples loaded\n", db.size(0));

  std::vector<unsigned int> inputArch, outputArch;

  inputArch.push_back(db.dimension(0));
  inputArch.push_back(HIDDEN); // db.dimension(0)*2);
  inputArch.push_back(HIDDEN);

  outputArch.push_back(db.dimension(1));
  outputArch.push_back(HIDDEN); // db.dimension(1)*2);
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

  if(inputRBM.save("temp/midi-input-nnetwork.conf") == false){
    printf("ERROR: saving RBM network failed.\n");
  }

  printf("OUTPUT architecture: %dx%dx%d\n",
	 outputArch[0], outputArch[1], outputArch[2]);
  
  printf("Learning RBM weights (gradient descent data maximum likelihood)..\n");
  fflush(stdout);

  outputRBM.learnWeights(output_samples, dW, true);

  if(outputRBM.save("temp/midi-output-nnetwork.conf") == false){
    printf("ERROR: saving RBM network failed.\n");
  }

  
  return 0;
}
