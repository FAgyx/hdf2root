#ifndef WAVEEDGETIME
#define WAVEEDGETIME

#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime> 

#include "TTree.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TF1.h"
#include "TMath.h"
#include "TAxis.h"

#include "inc/methods.h"



class WaveEdgeTime {
public:
  WaveEdgeTime(TFile* p_input_rootfile, std::vector<int> chnls, TString outFileFolder);
  void find_result(long entries, std::vector<double> vths);
  double find_first_edge_time(TH1D* p_wave_in, double vth);
  std::vector<TH1D*> firstEdgeTime;
  std::vector<TH2D*> firstEdgeTime_integration;

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
