#ifndef WAVEDATA
#define WAVEDATA

#include <iostream>

#include "TTree.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TFile.h"
#include "TVirtualFFT.h"
#include "TF1.h"
#include "TMath.h"

#include "inc/WaveAttr.h"




class WaveData {
public:
  WaveData(TTree* tree, WaveAttr* waveattr, std::vector<int> chnls);
  void GetRawData(long entries);
  void Lowpass_FFT();
  std::vector<TBranch*> pb_data;
  std::vector<TBranch*> pb_TH1s;
  std::vector<TBranch*> pb_TH1s_back;
  WaveAttr *attr;
  TTree* waveTH1_tree = new TTree("waveTH1_tree","waveTH1_tree");
  std::vector<TH1D*> p_waveform_TH1s;
  std::vector<TH1D*> p_waveform_TH1s_back;
  
  
private:
  

}; //class WaveAttr


#endif
