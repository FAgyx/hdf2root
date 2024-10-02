#ifndef WAVEEDGETIME
#define WAVEEDGETIME

#include <iostream>

#include "TTree.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TFile.h"
#include "TF1.h"
#include "TMath.h"



class WaveEdgeTime {
public:
  WaveEdgeTime(TFile* p_input_rootfile, std::vector<int> chnls, TString outFileFolder);
  void find_first_edge_time(long entries, std::vector<double> vths);
  std::vector<TBranch*> pb_TH1s_in;
  std::vector<TBranch*> pb_result_out;
  std::vector<TH1D*> p_wave;
  std::vector<int> _chnls;
  TH1D* p_wave_template;
  TString outFolder;
  TTree* wave_tree_in; 
  TTree* wave_tree_out; 
  
private:
  

}; //class WaveAttr


#endif
