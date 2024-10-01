#include "inc/WaveFFT.h"


WaveFFT::WaveFFT(TFile* p_input_rootfile, std::vector<int> chnls, TString outFileFolder){
  TTree* tree;
  p_input_rootfile->GetObject("waveTH1_tree", tree);
  tree->ls();
  tree->Print();
  for (auto chnl : chnls){
    TBranch *pb = tree->
      GetBranch(("waveTH1_channel"+std::to_string(chnl)).c_str());
    pb_TH1s.push_back(pb);    
  }
  p_wave_template =(TH1D*)p_input_rootfile->Get("waveform");
  wave_back_tree = new TTree("wave_back_tree","wave_back_tree");
  outFolder = outFileFolder;  
}


void WaveFFT::Lowpass_FFT(long entries, long draw_entries, int lowpass_freq){
  TH1D* p_waveform = (TH1D*)p_wave_template->Clone();
  TH1D* p_waveform_back = (TH1D*)p_wave_template->Clone();
  int n_points = p_waveform->GetNbinsX();

  TH1 *hm = nullptr;
  TVirtualFFT::SetTransform(nullptr);
  Double_t *re_full = new Double_t[n_points];
  Double_t *im_full = new Double_t[n_points];
  TVirtualFFT *fft = nullptr;
  TVirtualFFT *fft_back = nullptr;

  TCanvas *p_output_canvas = new TCanvas("waveform", "waveform");
  p_output_canvas->SetRightMargin(0.1);
  int vindex = 0;
  for(auto p_branch : pb_TH1s){  //create output branches of FFT back result   
    p_branch->SetAddress(&p_waveform);

    pb_TH1s_back.push_back(wave_back_tree->Branch(p_branch->GetFullName()+"_back", 
      "TH1D",&p_waveform_back));
      //create one fft back branch for each channel
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
      p_branch->GetEntry(j);  //load entry to p_waveform

      hm = p_waveform->FFT(hm, "MAG");  //load FFT result to hm
      // if(j<draw_entries){
      //   hm->SetTitle("Magnitude of the 1st transform");
      //   hm->SetBins(n_points,0,2.5e9);
      //   hm->Draw();
      //   p_output_canvas->SetLogy();
      //   p_output_canvas->SaveAs(("output/a/event"+std::to_string(j)+"_"+pb_data.at(vindex)->GetFullName()+"_fft.png"));
      // //NOTE: for "real" frequencies you have to divide the x-axes range with the range of your function
      //  //(in this case 4*Pi); y-axes has to be rescaled by a factor of 1/SQRT(n) to be right: this is not done automatically!

      // }



      //Use the following method to get the full output:
      fft = TVirtualFFT::GetCurrentTransform();


      fft->GetPointsComplex(re_full,im_full);
      for (int freq_index = lowpass_freq; 
        freq_index<n_points-lowpass_freq; ++freq_index)
      {
        re_full[freq_index] = 0;
        im_full[freq_index] = 0;
      }
      fft_back = TVirtualFFT::FFT(1, &n_points, "C2R M K");
      fft_back->SetPointsComplex(re_full,im_full);
      fft_back->Transform();

      TH1::TransformHisto(fft_back,p_waveform_back,"Re");
      p_waveform_back->Scale(1.0/n_points);

      if(j<draw_entries){
        p_waveform_back->Draw("HIST");
        p_output_canvas->SetLogy(0);
        p_output_canvas->SaveAs((outFolder+"/event"+std::to_string(j)+"_"+p_branch->GetFullName()+"_fftback.png"));
      }
      if(vindex==0)wave_back_tree->Fill();   
      else pb_TH1s_back.at(vindex)->Fill();
      // std::cout<<"hm"<<std::endl;
      delete hm;
      // std::cout<<"fft"<<std::endl;
      delete fft;
      // std::cout<<"fft_back"<<std::endl;
      delete fft_back;
      // std::cout<<"delete finish"<<std::endl;

      hm = nullptr;
      fft = nullptr;
      fft_back = nullptr;
      
    }
    ++vindex;
  }

}
