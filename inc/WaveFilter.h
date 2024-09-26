#ifndef WAVEFILTER
#define WAVEFILTER

#include <iostream>

#include "TTree.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TFile.h"
#include "TF1.h"
#include "TMath.h"





class WaveFilter {
public:
  WaveFilter(TTree* tree, std::vector<int> chnls);
  // void GetRawData(long entries, long draw_entries);
  // void Lowpass_FFT(long draw_entries, int lowpass_freq);
  std::vector<TBranch*> pb_TH1s_back;
  // TTree* waveTH1_tree = new TTree("waveTH1_tree","waveTH1_tree");
  // std::vector<TH1D*> p_waveform_TH1s;
  // std::vector<TH1D*> p_waveform_TH1s_back;
  
  
private:
  

}; //class WaveAttr


#endif
