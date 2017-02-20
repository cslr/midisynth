
#include <stdio.h>
#include <iostream>
#include <dinrhiw/dinrhiw.h>

#include <fenv.h>


int main(int argc, char** argv)
{
  printf("NN PREDICTION MODEL\n");

#if 1
  {
    // FE_UNDERFLOW | FE_OVERFLOW  | FE_INEXACT
    feenableexcept(FE_INVALID | FE_DIVBYZERO);
  }
#endif
  
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

  whiteice::DBN<> inputRBM;
  whiteice::DBN<> outputRBM;
  
  std::vector< whiteice::math::vertex<> > input_samples;
  std::vector< whiteice::math::vertex<> > output_samples;
  
  for(unsigned int i=0;i<db.size(0);i++){
    input_samples.push_back(db.access(0,i));
    output_samples.push_back(db.access(1,i));
  }
  
  if(inputRBM.load("models/midi-input-nnetwork.conf") == false){
    printf("ERROR: loading RBM model (input) failed\n");
    return -1;
  }

  if(outputRBM.load("models/midi-output-nnetwork.conf") == false){
    printf("ERROR: loading RBM model (output) failed\n");
    return -1;
  }

  // calculate RBM feature vectors from inputs
  std::vector< whiteice::math::vertex<> > input_features;
  std::vector< whiteice::math::vertex<> > output_features;

  whiteice::GBRBM<>& input  = inputRBM.getInputGBRBM();
  whiteice::GBRBM<>& output = outputRBM.getInputGBRBM();

  for(unsigned int i=0;i<db.size(0);i++){
    const unsigned int index = i;

    {
      if(input.setVisible(db.access(0, index)) == false){
	printf("ERROR: input dimension mismatch when setting GBRBM input\n");
	return -1;
      }
      
      if(input.reconstructDataHidden(1) == false){
	printf("ERROR: GBRBM reconstruct failed\n");
	return -1;
      }
      
      whiteice::math::vertex<> h;
      input.getHidden(h);

      input_features.push_back(h);
    }

    {
      if(output.setVisible(db.access(1, index)) == false){
	printf("ERROR: input dimension mismatch when setting GBRBM input\n");
	return -1;
      }
      
      if(output.reconstructDataHidden(1) == false){
	printf("ERROR: GBRBM reconstruct failed\n");
	return -1;
      }
      
      whiteice::math::vertex<> h;
      output.getHidden(h);

      output_features.push_back(h);
    }
  }
  
  
  //////////////////////////////////////////////////////////////////////
  // now we learn neural network model for predicting then next step

  std::vector<unsigned int> arch;

  // 2 layer network
  arch.push_back(input.getHiddenNodes()); 
  arch.push_back(1000);
  arch.push_back(output.getHiddenNodes());

  whiteice::nnetwork<> net;
  // net.setArchitecture(arch, whiteice::nnetwork<>::halfLinear);
  // net.setNonlinearity(arch.size()-2, whiteice::nnetwork<>::pureLinear);

  // net.randomize();

  whiteice::dataset<> data;

  // add data to dataset
  {
    data.createCluster("input-features", input.getHiddenNodes());
    data.createCluster("output-features", output.getHiddenNodes());

    data.add(0, input_features);
    data.add(1, output_features);

    // data.preprocess(0, whiteice::dataset<>::dnMeanVarianceNormalization);
    data.preprocess(1, whiteice::dataset<>::dnMeanVarianceNormalization);
  }

  printf("INPUT  DATA: %d samples\n", data.size(0));
  printf("OUTPUT DATA: %d samples\n", data.size(1));

  // pretrain using BB-RBM
  {
    std::vector<unsigned int> a;
    a.push_back(arch[0]);
    a.push_back(arch[1]);
    
    whiteice::DBN<> dbn(a, true); // BB-RBM network
    whiteice::math::blas_real<float> delta = 0.01; // 0.001

    std::vector< whiteice::math::vertex<> > samples;

    data.getData(0, samples);

    for(const auto& s : samples){
      int size = s.size();
      if(size > 10) size = 10;
      if(size > 0){
	printf("IN  %d", (int)s[0].c[0]);
	for(unsigned int i=1;i<size;i++)
	  printf(" %d", (int)s[i].c[0]);
	printf("\n");
      }
    }

    dbn.learnWeights(samples, delta, true);

    // check reconstruction.. 
    dbn.reconstructData(samples);

    for(const auto& s : samples){
      int size = s.size();
      if(size > 10) size = 10;
      if(size > 0){
	printf("OUT %d", (int)s[0].c[0]);
	for(unsigned int i=1;i<size;i++)
	  printf(" %d", (int)s[i].c[0]);
	printf("\n");
      }
    }

    whiteice::nnetwork<>* nn = nullptr;

    dbn.convertToNNetwork(data, nn);

    net = *nn;

    delete nn;

    // disable frozen flags
    {
      std::vector<bool> frozen;
      net.getFrozen(frozen);

      for(unsigned int i=0;i<frozen.size();i++)
	frozen[i] = false;

      net.setFrozen(frozen);
    }

    net.setNonlinearity(whiteice::nnetwork<>::sigmoid);
    net.setNonlinearity(net.getLayers()-1, whiteice::nnetwork<>::pureLinear);
  }

  
  printf("%d", arch[0]);
  for(unsigned int i=1;i<arch.size();i++)
    printf("x%d", arch[i]);
  printf(" NNETWORK\n");
  

  whiteice::math::NNGradDescent<> grad;
  // whiteice::LBFGS_nnetwork<> lbfgs(net, data, true);

  // whiteice::math::vertex<> w0;
  // net.exportdata(w0);
  {
    std::vector<whiteice::nnetwork<>::nonLinearity> nls;
    net.getNonlinearity(nls);
    printf("NNETWORK NON-LINEARITIES\n");
    for(unsigned int i=0;i<nls.size();i++)
      printf("%d ", (int)nls[i]);
    printf("\n");
  }

  grad.startOptimize(data, net, 2, 5000);
  // lbfgs.minimize(w0);
  
  unsigned int iters = 0;

  while(grad.isRunning()){
    // while(lbfgs.isRunning()){
    sleep(1);
    whiteice::math::blas_real<float> error;
    const unsigned int olditers = iters;
    
    if(grad.getSolution(net, error, iters)){
      // if(lbfgs.getSolution(w0, error, iters)){
      if(olditers != iters){
	printf("NNETWORK OPTIMIZER %d ITER %f ERROR\n", iters, error.c[0]);
      }
    }   
  }

  printf("NNETWORK OPTIMIZATION STOPPED\n");
  
  {
    whiteice::math::blas_real<float> error;

    if(grad.getSolution(net, error, iters) == false){
      //if(lbfgs.getSolution(w0, error, iters) == false){
      printf("ERROR: getting optimization results failed\n");
      return -1;
    }

    // net.importdata(w0);
  }


  // saves resulting nnetwork
  {
    data.clearData(0);
    data.clearData(1);
    
    if(data.save("models/data.dat") == false){
      printf("ERROR: saving features dataset & preprocessing information failed\n");
      return -1;
    }

    if(net.save("models/midisynth.conf") == false){
      printf("ERROR: saving midi synth prediction failed\n");
      return -1;
    }
  }
  
  
  return 0;
}
