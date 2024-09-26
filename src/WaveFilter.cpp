#include "inc/WaveFilter.h"


WaveFilter::WaveFilter(TTree* tree, std::vector<int> chnls){
  //Get branches to pb_data
  
  for (auto chnl : chnls){
    TBranch *pb = tree->
      GetBranch(("channel"+std::to_string(chnl)+"_back").c_str());
    pb_TH1s_back.push_back(pb);    
  }
}


// void WaveData::GetRawData(long entries, long draw_entries=0){ // entries==0 get all data
//   //calculate voltage and time and save to TH1D

//   char channel_array[WaveAttr::n_points]; 
//   int vindex=0;
//   TH1D* p_waveform = new TH1D("waveform", "waveform", 
//       WaveAttr::n_points,WaveAttr::time_range_left*1e9,WaveAttr::time_range_right*1e9);

//   TCanvas *p_output_canvas = new TCanvas("waveform", "waveform");
//   p_output_canvas->SetRightMargin(0.1);

//   for(auto p_branch : pb_data){  //create output branches by channels
//     p_branch->SetAddress(channel_array);    
//     p_waveform->GetXaxis()->SetTitle("Time (ns)");
//     p_waveform->GetYaxis()->SetTitle("Voltage (V)");
//     pb_TH1s.push_back(waveTH1_tree->Branch("waveTH1_"+p_branch->GetFullName(), 
//       "TH1D",&p_waveform));
//       //create one output branch for each input branch    

//     if(entries==0||entries>p_branch->GetEntries())  
//         entries=p_branch->GetEntries();
//     //all input branches should have same entries

//     for (long j = 0; j < entries; ++j){  //loop by events
//       p_branch->GetEntry(j);   //load channel_array from root file
//       for (int i = 0; i < WaveAttr::n_points; ++i){  //calculate real waveform from WaveAttr
//         p_waveform->SetBinContent(i+1,
//           channel_array[i]*attr->vertical_gains.at(vindex)
//           -attr->vertical_offsets.at(vindex));
//       }
//       if(j<draw_entries){
//         p_output_canvas->cd();
//         p_waveform->Draw();
//         p_output_canvas->SaveAs(("output/a/event"+std::to_string(j)+"_"+p_branch->GetFullName()+".png"));
//       }

//       if(vindex==0)waveTH1_tree->Fill();   
//       //Can not use TBranch::Fill() when the tree has no entry
//       else pb_TH1s.at(vindex)->Fill();
//       //TBranch::Fill() works when adding a new branch when the tree has exsisting t
      
//     } // for j < entries
//         // TCanvas *p_output_canvas = new TCanvas("waveform", "waveform");
//         // p_output_canvas->cd(); 
//     ++vindex;   
//   } //for(auto p_branch : pb_data)
  
//   // p_waveform->Draw();
//   // p_output_canvas->SaveAs("test.png");
// }

// void WaveData::Lowpass_FFT(long draw_entries, int lowpass_freq){
//   TH1D* p_waveform = new TH1D("waveform", "waveform", 
//     WaveAttr::n_points,WaveAttr::time_range_left*1e9,WaveAttr::time_range_right*1e9);
//   TH1D* p_waveform_back = new TH1D("waveform_back", "waveform_back", 
//     WaveAttr::n_points,WaveAttr::time_range_left*1e9,WaveAttr::time_range_right*1e9);

//   p_waveform_back->GetXaxis()->SetTitle("Time (ns)");
//   p_waveform_back->GetYaxis()->SetTitle("Voltage (V)");

//   TH1 *hm = nullptr;
//   TH1 *hb = nullptr;
//   TVirtualFFT::SetTransform(nullptr);
//   Double_t *re_full = new Double_t[WaveAttr::n_points];
//   Double_t *im_full = new Double_t[WaveAttr::n_points];
//   TVirtualFFT *fft = nullptr;
//   TVirtualFFT *fft_back = nullptr;

//   TCanvas *p_output_canvas = new TCanvas("waveform", "waveform");
//   p_output_canvas->SetRightMargin(0.1);
//   int vindex = 0;
//   for(auto p_branch : pb_TH1s){  //create output branches of FFT back result   
//     // p_waveform_TH1s_back.push_back(p_waveform_back);
//     pb_TH1s_back.push_back(waveTH1_tree->Branch(p_branch->GetFullName()+"_back", 
//       "TH1D",&p_waveform_back));
//       //create one fft back branch for each channel

//     for(long j = 0; j < p_branch->GetEntries(); ++j){
//       p_branch->SetAddress(&p_waveform);
//       p_branch->GetEntry(j);  //load entry to p_waveform
//       hm = p_waveform->FFT(hm, "MAG");  //load FFT result to hm
//       // if(j<draw_entries){
//       //   hm->SetTitle("Magnitude of the 1st transform");
//       //   hm->SetBins(WaveAttr::n_points,0,2.5e9);
//       //   hm->Draw();
//       //   p_output_canvas->SetLogy();
//       //   p_output_canvas->SaveAs(("output/a/event"+std::to_string(j)+"_"+pb_data.at(vindex)->GetFullName()+"_fft.png"));
//       // //NOTE: for "real" frequencies you have to divide the x-axes range with the range of your function
//       //  //(in this case 4*Pi); y-axes has to be rescaled by a factor of 1/SQRT(n) to be right: this is not done automatically!

//       // }


//       //Use the following method to get the full output:
//       fft = TVirtualFFT::GetCurrentTransform();


//       fft->GetPointsComplex(re_full,im_full);
//       for (int freq_index = lowpass_freq; 
//         freq_index<WaveAttr::n_points-lowpass_freq; ++freq_index)
//       {
//         re_full[freq_index] = 0;
//         im_full[freq_index] = 0;
//       }
//       fft_back = TVirtualFFT::FFT(1, &WaveAttr::n_points, "C2R M K");
//       fft_back->SetPointsComplex(re_full,im_full);
//       fft_back->Transform();

//       TH1::TransformHisto(fft_back,p_waveform_back,"Re");
//       p_waveform_back->Scale(1.0/WaveAttr::n_points);
//       pb_TH1s_back.at(vindex)->Fill();

//       if(j<draw_entries){
//         p_waveform_back->Draw("HIST");
//         p_output_canvas->SetLogy(0);
//         p_output_canvas->SaveAs(("output/a/event"+std::to_string(j)+"_"+pb_data.at(vindex)->GetFullName()+"_fftback.png"));
//       }
//       // hb->SetBins(WaveAttr::n_points,WaveAttr::time_range_left,WaveAttr::time_range_right);
//     }
//     ++vindex;
//   }

// }
