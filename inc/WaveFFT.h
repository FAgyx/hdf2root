#ifndef WAVEFFT
#define WAVEFFT

#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime> 

#include "TTree.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TFile.h"
#include "TVirtualFFT.h"
#include "TF1.h"
#include "TMath.h"
#include "TPaveLabel.h"
#include "TKey.h"
#include "TLegend.h"

#include "inc/methods.h"


class WaveFFT {
public:
  WaveFFT(TFile* p_input_rootfile, std::vector<int> chnls, TString outFileFolder);
  void Lowpass_FFT_filter(long entries, long draw_entries, double sampling_rate, 
  double lowpass_freq, std::vector<double> chnl_offsets, int filter_chnl);
  bool FFT_filtering(TH1D* p_waveform_in, TH1D* p_waveform_out, TH1D* p_waveform_offset,
  TString channel_name, double sampling_rate, double lowpass_freq, long entry, long draw_entries);
  std::vector<TBranch*> pb_TH1s;
  std::vector<TBranch*> pb_TH1s_back;
  TH1D* p_wave_template;
  TString outFolder;
  TTree* wave_tree_in;
  TTree* wave_back_tree; 
  int draw_count;
  
private:
  

}; //class WaveAttr


#endif
