#ifndef WAVEDATA
#define WAVEDATA

#include <iostream>
#include <chrono>
#include <ctime> 
#include <iomanip>
#include "TTree.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TFile.h"
#include "TVirtualFFT.h"
#include "TF1.h"
#include "TMath.h"

#include "inc/WaveAttr.h"
#include "inc/methods.h"


class WaveData {
public:
  WaveData(TTree* tree, WaveAttr* waveattr, std::vector<int> chnls, TString outFileFolder);
  void GetRawData(long entries, long draw_entries);
  void Lowpass_FFT(long draw_entries, int lowpass_freq);
  std::vector<TBranch*> pb_data;
  std::vector<TBranch*> pb_TH1s;
  std::vector<TBranch*> pb_TH1s_back;
  WaveAttr *attr;
  TString outFolder;
  TTree* waveTH1_tree;
  std::vector<TH1D*> p_waveform_TH1s;
  std::vector<TH1D*> p_waveform_TH1s_back;
  
  
private:
  

}; //class WaveAttr


#endif
