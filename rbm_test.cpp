
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include <dinrhiw/dinrhiw.h>

#include <iostream>
#include <vector>

volatile bool stopsignal = false;
void install_signal_handler();


int main(int argc, char** argv)
{
  printf("RBM MIDI NOTES MODEL\n");

  install_signal_handler();

  whiteice::dataset<> db;

  if(db.load("models/midinotes.ds") == false){
    printf("ERROR: Couldn't load database\n");
    return -1;
  }

  printf("Converting data to timeseries format..\n");

  std::vector< std::vector< whiteice::math::vertex<> > > timeseries;
  timeseries.resize(db.getNumberOfClusters());

  unsigned int datapoints = 0;

  for(unsigned int i=0;i<db.getNumberOfClusters();i++){
    std::vector< whiteice::math::vertex<> >& ts = timeseries[i];

    for(unsigned int k=0;k<db.size(i);k++){
      // we convert full-range 128 MIDI notes
      // to use 3 octaves instead C-4, C-5 and C-6 (3*12 = 36) 48..83

      auto& note = db.access(i,k);
      whiteice::math::vertex<> n(36);

      for(unsigned int i=48;i<84;i++){
	n[i-48] = note[i];
      }
      
      ts.push_back(n);
      
      datapoints++;
    }
  }

  // whiteice::RNN_RBM<> rbm(128, 16, 2);
  whiteice::RNN_RBM<> rbm(36, 16, 2);

  bool hasRBM = false;

  if(argc > 1){
    if(strcmp(argv[1], "--load") == 0){
      if(rbm.load("models/midi-rbm-rnn.conf") == false){
	printf("ERROR: loading RNN-RBM failed.\n");
	return -1;
      }

      hasRBM = true;
      printf("Loaded RNN-RBM from disk.\n");
    }
  }

  printf("STARTING RNN-RBM (%d datapoints)...\n", datapoints);


  printf("RNN-RBM %dx%dx%d\n",
	 rbm.getVisibleDimensions(),
	 rbm.getHiddenDimensions(),
	 rbm.getRecurrentDimensions());

  if(rbm.startOptimize(timeseries, !hasRBM) == false){
    printf("ERROR: starting RNN-RBM optimizer FAILED.\n");
    return -1;
  }

  unsigned int iters = 0;
  
  while(rbm.isRunning() && !stopsignal){
    whiteice::math::blas_real<float> e;
    const unsigned int old_iters = iters;
    
    if(rbm.getOptimizeError(iters, e)){
      if(iters != old_iters){
	printf("%d ITER. ERROR: %f\n", iters, e.c[0]);
	fflush(stdout);
      }
    }
    else{
      printf("ERROR: RNN_RBM::getOptimizeError() failed.\n");
      return -1;
    }
  }

  printf("Stopping RNN-RBM optimization..\n");

  rbm.stopOptimize();

  if(rbm.save("models/midi-rbm-rnn.conf") == false){
    printf("ERROR: saving RNN-RBM FAILED.\n");
  }

  return 0;
}



void sigint_signal_handler(int s)
{
  stopsignal = true;
}


void install_signal_handler()
{
#ifndef WINOS
  struct sigaction sih;
  
  sih.sa_handler = sigint_signal_handler;
  sigemptyset(&sih.sa_mask);
  sih.sa_flags = 0;

  sigaction(SIGINT, &sih, NULL);
#endif
}



#if 0
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
#endif
