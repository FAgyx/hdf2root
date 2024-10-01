#ifndef WAVEFFT
#define WAVEFFT

#include <iostream>
#include <chrono>
#include <ctime> 

#include "TTree.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TFile.h"
#include "TVirtualFFT.h"
#include "TF1.h"
#include "TMath.h"

#include "inc/methods.h"


class WaveFFT {
public:
  WaveFFT(TFile* p_input_rootfile, std::vector<int> chnls, TString outFileFolder);
  void Lowpass_FFT(long entries, long draw_entries, int lowpass_freq);
  std::vector<TBranch*> pb_TH1s;
  std::vector<TBranch*> pb_TH1s_back;
  TH1D* p_wave_template;
  TString outFolder;
  TTree* wave_back_tree; 
  
private:
  

}; //class WaveAttr


#endif
