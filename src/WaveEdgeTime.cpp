#include "inc/WaveEdgeTime.h"


WaveEdgeTime::WaveEdgeTime(TFile* p_input_rootfile, std::vector<int> chnls, 
  TString outFileFolder){
  p_input_rootfile->GetObject("wave_sel_tree", wave_tree_in);
  wave_tree_in->ls();
  wave_tree_in->Print();

  for (auto chnl : chnls){
    _chnls.push_back(chnl);
    TBranch *pb = wave_tree_in->
      GetBranch(("waveTH1_channel"+std::to_string(chnl)+"_back_sel").c_str());
    pb_TH1s_in.push_back(pb);    
  }
  wave_tree_out = new TTree("wave_result_tree","wave_result_tree");
  p_wave_template =(TH1D*)p_input_rootfile->Get("waveform");
  outFolder = outFileFolder;  
}


void WaveEdgeTime::find_result(long entries, 
  std::vector<double> vths){

  
  int chnl_number = _chnls.size();
  if(entries==0||entries>pb_TH1s_in.at(0)->GetEntries())  
    entries=pb_TH1s_in.at(0)->GetEntries(); 

  TH1D* p_waveform;
  double edge_time;
  std::vector<double> edge_times;
  double integral;
  std::vector<double> integrals;

  
  int total_bins;
  total_bins = p_wave_template->GetXaxis()->GetNbins();
  for (int i = 0; i < chnl_number; ++i){
    p_waveform = (TH1D*)p_wave_template->Clone();
    p_wave.push_back(p_waveform); 
    edge_times.push_back(edge_time);
    integrals.push_back(integral);
  }
  for (int i = 0; i < chnl_number; ++i){    
    pb_TH1s_in.at(i)->SetAddress(&(p_wave.at(i)));//link p_waveform to input branch
    pb_result_out.push_back(wave_tree_out->Branch(("chnl"+std::to_string(_chnls.at(i))+
      "_firstedgetime").c_str(), 
      &(edge_times.at(i))));  
    pb_result_out.push_back(wave_tree_out->Branch(("chnl"+std::to_string(_chnls.at(i))+
      "_integral").c_str(), 
      &(integrals.at(i)))); 

  }
  
  for (long j = 0; j < entries; ++j){  //loop by events
    wave_tree_in->GetEntry(j); 
    for (int i = 0; i < chnl_number; ++i){ 
      edge_times.at(i) = find_first_edge_time(p_wave.at(i),vths.at(i));
      integrals.at(i) = p_wave.at(i)->GetSumOfWeights();
      
    } //for i < chnl_number
    wave_tree_out->Fill();
  } //for j < entries


}

double WaveEdgeTime::find_first_edge_time(TH1D* p_waveform, double vth){
  int total_bins = p_waveform->GetXaxis()->GetNbins();
  double edge_time=-999;
  for(int k = 5; k <total_bins;++k){
    if((p_waveform->GetBinContent(k-4)>vth)
      &&(p_waveform->GetBinContent(k-3)>vth)
      &&(p_waveform->GetBinContent(k-2)>vth)
      &&(p_waveform->GetBinContent(k-1)>vth))
    {
      edge_time = p_waveform->GetXaxis()->GetBinCenter(k-4);
      break;
    }     
  } //for k <total_bins
  return edge_time;
}

// double WaveEdgeTime::find_first_edge_time(TH1D* p_waveform, double vth){
