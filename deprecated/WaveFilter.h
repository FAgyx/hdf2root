#ifndef WAVEFILTER
#define WAVEFILTER

#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime> 

#include "TTree.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TFile.h"
#include "TF1.h"
#include "TMath.h"

#include "inc/methods.h"


class WaveFilter {
public:
  WaveFilter(TFile* p_input_rootfile, std::vector<int> chnls, TString outFileFolder);
  void filter_by_amplitude(long entries, long draw_entries, int filter_chnl, 
  	double filter_amp, double base_line);
  std::vector<TBranch*> pb_TH1s_in;
  std::vector<TBranch*> pb_TH1s_out;
  std::vector<TH1D*> p_wave;
  TH1D* p_wave_template;
  TString outFolder;
  TTree* wave_tree_in; 
  TTree* wave_tree_out; 
  
private:
  

}; //class WaveAttr


#endif
