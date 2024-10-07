#include "inc/WaveData.h"

WaveData::WaveData(TTree* tree, WaveAttr* waveattr, std::vector<int> chnls, TString outFileFolder){
  //Get branches to pb_data
  
  for (auto chnl : chnls){
    TBranch *pb = tree->
      GetBranch(("channel"+std::to_string(chnl)).c_str());
    pb_data.push_back(pb);    
  }
  
  waveTH1_tree = new TTree("waveTH1_tree","waveTH1_tree");
  // waveTH1_tree->SetAutoFlush(1000);
  attr=waveattr;  //copy att
  outFolder = outFileFolder;
}


void WaveData::GetRawData(long entries, long draw_entries=0){ // entries==0 get all data
  //calculate voltage and time and save to TH1D

  char channel_array[WaveAttr::n_points]; 
  int vindex=0;
  
  TH1D* p_waveform = new TH1D("waveform", "waveform", 
      WaveAttr::n_points,WaveAttr::time_range_left*1e9,WaveAttr::time_range_right*1e9);
  p_waveform->GetXaxis()->SetTitle("Time (ns)");
  p_waveform->GetYaxis()->SetTitle("Voltage (V)");

  TCanvas *p_output_canvas = new TCanvas("waveform", "waveform");
  p_output_canvas->SetRightMargin(0.1);

  for(auto p_branch : pb_data){  //create output branches by channels
    p_branch->SetAddress(channel_array);    
    
    pb_TH1s.push_back(waveTH1_tree->Branch("waveTH1_"+p_branch->GetFullName(), 
      "TH1D",&p_waveform));
      //create one output branch for each input branch    

    if(entries==0||entries>p_branch->GetEntries())  
        entries=p_branch->GetEntries();

    long event_print = 100;

    for (long j = 0; j < entries; ++j){  //loop by events
      if (j % event_print == 0) {
        std::time_t current_time_date = std::chrono::system_clock::to_time_t(
          std::chrono::system_clock::now());
        auto gmt_time = gmtime(&current_time_date);
        auto timestamp = std::put_time(gmt_time, "%Y-%m-%d %H:%M:%S");
        std::cout << "GetRawData "<<p_branch->GetFullName()<<" event " << j <<" at "<<
          timestamp<<", VM="<<getValueVM()<<" MB, PM="<<getValuePM()<<" MB"<<std::endl;
        if (TMath::Floor(TMath::Log10(j)) > TMath::Floor(TMath::Log10(event_print))) event_print*=10;
      }
      p_branch->GetEntry(j);   //load channel_array from root file
      for (int i = 0; i < WaveAttr::n_points; ++i){  //calculate real waveform from WaveAttr
        p_waveform->SetBinContent(i+1,
          channel_array[i]*attr->vertical_gains.at(vindex)
          -attr->vertical_offsets.at(vindex));
      }
      if(j<draw_entries){
        p_output_canvas->cd();
        p_waveform->Draw();
        p_output_canvas->SaveAs((outFolder+"/event"+std::to_string(j)+"_"+p_branch->GetFullName()+".png"));
      }

      if(vindex==0)waveTH1_tree->Fill();   
      //Can not use TBranch::Fill() when the tree has no entry
      else pb_TH1s.at(vindex)->Fill();
      //TBranch::Fill() works when adding a new branch when the tree has exsisting t
      
    } // for j < entries 
    ++vindex;   
  } //for(auto p_branch : pb_data)
}

