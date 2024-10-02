#include "inc/WaveFilter.h"


WaveFilter::WaveFilter(TFile* p_input_rootfile, std::vector<int> chnls, TString outFileFolder){
  p_input_rootfile->GetObject("wave_back_tree", wave_tree_in);
  wave_tree_in->ls();
  wave_tree_in->Print();

  for (auto chnl : chnls){
    TBranch *pb = wave_tree_in->
      GetBranch(("waveTH1_channel"+std::to_string(chnl)+"_back").c_str());
    pb_TH1s_in.push_back(pb);    
  }
  wave_tree_out = new TTree("wave_sel_tree","wave_sel_tree");
  p_wave_template =(TH1D*)p_input_rootfile->Get("waveform");
  
  outFolder = outFileFolder;  
}


void WaveFilter::filter_by_amplitude(long entries, long draw_entries, int filter_chnl,
  double filter_amp, double base_line){
  int chnl_number = pb_TH1s_in.size();
  if(entries==0||entries>pb_TH1s_in.at(0)->GetEntries())  
    entries=pb_TH1s_in.at(0)->GetEntries(); 
  long event_print = 100;
  long draw_count = 0;
  TH1D* p_waveform;
  for (int i = 0; i < chnl_number; ++i){
    p_waveform = (TH1D*)p_wave_template->Clone();
    p_wave.push_back(p_waveform); 
    

    //push_back must be in a seperate for loop, otherwise segment fault
  }


  for (int i = 0; i < chnl_number; ++i){    
    pb_TH1s_in.at(i)->SetAddress(&(p_wave.at(i)));//link p_waveform to input branch
    pb_TH1s_out.push_back(wave_tree_out->Branch(pb_TH1s_in.at(i)->GetFullName()+"_sel", 
      "TH1D",&(p_wave.at(i))));  //create output branch and link same p_waveform to output branch
  }

  TCanvas *p_output_canvas = new TCanvas("waveform", "waveform");
  p_output_canvas->SetRightMargin(0.1);
  p_wave_template->Write();

  for (long j = 0; j < entries; ++j){  //loop by events

    if (j % event_print == 0) {
      std::time_t current_time_date = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());
      auto gmt_time = gmtime(&current_time_date);
      auto timestamp = std::put_time(gmt_time, "%Y-%m-%d %H:%M:%S");
      std::cout << "Check amp for event " << j <<" at "<<
        timestamp<<", VM="<<getValueVM()<<" MB, PM="<<getValuePM()<<" MB"<<std::endl;
      if (TMath::Floor(TMath::Log10(j)) > TMath::Floor(TMath::Log10(event_print))) event_print*=10;
    }
    wave_tree_in->GetEntry(j); 

    if((p_wave.at(filter_chnl)->GetMaximum())>filter_amp
      && p_wave.at(filter_chnl)->GetMinimum()>base_line
      ||(p_wave.at(filter_chnl)->GetMaximum()>0.1)){
      //require 40 mV amplitude
      for (int i = 0; i < chnl_number; ++i){    
        if(draw_count<draw_entries){
          p_wave.at(i)->Draw("HIST");   
          p_output_canvas->SaveAs((outFolder+"/event"+std::to_string(j)+"_"+
            pb_TH1s_in.at(i)->GetFullName().ReplaceAll("waveTH1_","")+".png"));
        }
      }  
      ++draw_count;
      wave_tree_out->Fill();
      
    }     

  } //j < entries;

}
