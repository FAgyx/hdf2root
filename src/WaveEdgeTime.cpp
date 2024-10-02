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
    pb->Print();
    pb_TH1s_in.push_back(pb);    
  }
  wave_tree_out = new TTree("wave_result_tree","wave_result_tree");
  p_wave_template =(TH1D*)p_input_rootfile->Get("waveform");
  outFolder = outFileFolder;  
}


void WaveEdgeTime::find_first_edge_time(long entries, 
  std::vector<double> vths){

  
  int chnl_number = _chnls.size();
  if(entries==0||entries>pb_TH1s_in.at(0)->GetEntries())  
    entries=pb_TH1s_in.at(0)->GetEntries(); 

  TH1D* p_waveform;
  std::vector<double> edge_times;
  double edge_time;
  int total_bins;
  total_bins = p_wave_template->GetXaxis()->GetNbins();
  for (int i = 0; i < chnl_number; ++i){
    p_waveform = (TH1D*)p_wave_template->Clone();
    p_wave.push_back(p_waveform); 
    edge_times.push_back(edge_time);
  }
  std::cout<<"1"<<std::endl;
  for (int i = 0; i < chnl_number; ++i){    
    pb_TH1s_in.at(i)->SetAddress(&(p_wave.at(i)));//link p_waveform to input branch
    pb_result_out.push_back(wave_tree_out->Branch(("chnl"+std::to_string(_chnls.at(i))+
      "_firstedgetime").c_str(), 
      "double",&(edge_times.at(i))));  //create output branch and link same p_waveform to output branch
  }
  std::cout<<"2"<<std::endl;
  
  for (long j = 0; j < entries; ++j){  //loop by events
    wave_tree_in->GetEntry(j); 
    for (int i = 0; i < chnl_number; ++i){ 
      edge_times.at(i) = -999;
      for(int k = 5; k <total_bins;++k){
        if((p_wave.at(i)->GetBinContent(k-4)>vths.at(i))
          &&(p_wave.at(i)->GetBinContent(k-3)>vths.at(i))
          &&(p_wave.at(i)->GetBinContent(k-2)>vths.at(i))
          &&(p_wave.at(i)->GetBinContent(k-1)>vths.at(i)))
        {
          edge_times.at(i) = p_wave.at(i)->GetXaxis()->GetBinCenter(k-4);
          break;
        }     
      } //for k <total_bins
    } //for i < chnl_number
    wave_tree_out->Fill();
  } //for j < entries


}
