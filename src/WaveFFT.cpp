#include "inc/WaveFFT.h"


WaveFFT::WaveFFT(TFile* p_input_rootfile, std::vector<int> chnls, TString outFileFolder){
  TIter next(p_input_rootfile->GetListOfKeys());
  TKey *key;
  while ((key = (TKey*)next())) {  //loop to find a TTree
    if (strstr(key->GetClassName(),"TTree")) {
      p_input_rootfile->GetObject(key->GetName(), wave_in_tree);
      break;
    }
  }
  wave_in_tree->ls();
  wave_in_tree->Print();

  TBranch *pb;
  TIter next_pb(wave_in_tree->GetListOfBranches());
  while ((pb = (TBranch*)next_pb())) {  //push all branches to pb_TH1s
      pb_TH1s.push_back(pb);
  }

  p_wave_template =(TH1D*)p_input_rootfile->Get("waveform");
  p_wave_template->Write();
  wave_back_tree = new TTree("wave_back_tree","wave_back_tree");
  outFolder = outFileFolder;  
}


void WaveFFT::Lowpass_FFT_filter(long entries, long draw_entries, double sampling_rate, 
  double lowpass_freq, std::vector<double> chnl_offsets, int filter_chnl){

  int chnl_number = pb_TH1s.size();
  int n_points = p_wave_template->GetXaxis()->GetNbins();
  int draw_count = 0;
  TCanvas *p_output_canvas = new TCanvas("waveform", "waveform");
  p_output_canvas->SetRightMargin(0.1);

  TH1D* p_waveform;
  std::vector<TH1D*> p_wave_in;
  for (int i = 0; i < chnl_number; ++i){
    p_waveform = (TH1D*)p_wave_template->Clone();
    p_wave_in.push_back(p_waveform); 
    //push_back must be in a seperate for loop, otherwise segment fault
  }

  TH1D* p_waveform_back;
  std::vector<TH1D*> p_wave_back;
  for (int i = 0; i < chnl_number; ++i){
    p_waveform_back = (TH1D*)p_wave_template->Clone();
    p_wave_back.push_back(p_waveform_back); 
    //push_back must be in a seperate for loop, otherwise segment fault
  }  

  TH1D* p_waveform_offset;
  std::vector<TH1D*> p_wave_offset;
  for (int i = 0; i < chnl_number; ++i){ //set offset for each channel
    p_waveform_offset = (TH1D*)p_wave_template->Clone();
    for (int j = 1; j <=p_waveform_offset->GetXaxis()->GetNbins(); ++j){
      p_waveform_offset->SetBinContent(j, chnl_offsets.at(i));
    }
    p_wave_offset.push_back(p_waveform_offset);
  }

  for (int i = 0; i < chnl_number; ++i){ //link input/output branch to waveforms
    pb_TH1s.at(i)->SetAddress(&p_wave_in.at(i));
    pb_TH1s_back.push_back(wave_back_tree->Branch(pb_TH1s.at(i)->GetFullName()+"_back", 
      "TH1D",&p_wave_back.at(i)));
  }  

  if(entries==0||entries>pb_TH1s.at(0)->GetEntries())  
    entries=pb_TH1s.at(0)->GetEntries(); 
  long event_print = 100;

  for (long j = 0; j < entries; ++j){  //loop by events
    if (j % event_print == 0) {
      std::time_t current_time_date = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());
      auto gmt_time = gmtime(&current_time_date);
      auto timestamp = std::put_time(gmt_time, "%Y-%m-%d %H:%M:%S");
      std::cout << "Do FFT for event " << j <<" at "<<
        timestamp<<", VM="<<getValueVM()<<" MB, PM="<<getValuePM()<<" MB"<<std::endl;
      if (TMath::Floor(TMath::Log10(j)) > TMath::Floor(TMath::Log10(event_print))) event_print*=10;
    }
    wave_in_tree->GetEntry(j);  //load entry for all branches
    int fill = 0;

    for (int i = 0; i < chnl_number; ++i){
      int fill_flag = 0; 
      fill_flag=FFT_filtering(p_wave_in.at(i),p_wave_back.at(i),p_wave_offset.at(i),
        pb_TH1s.at(i)->GetFullName(),sampling_rate,lowpass_freq,j,draw_entries);

      if(i==filter_chnl){
        fill=fill_flag; 
      }
    }

    if(fill){
      wave_back_tree->Fill(); 
      if(draw_count<draw_entries){
        for (int i = 0; i < chnl_number; ++i){
          p_output_canvas->cd();
          p_wave_back.at(i)->Draw("HIST");
          p_output_canvas->SetLogy(0);
          p_output_canvas->SaveAs((outFolder+"/event_sel/event"+std::to_string(j)+"_"+
            pb_TH1s.at(i)->GetFullName()+"_fftback.png"));
        }
      }
      ++draw_count;
    }
  }//loop by events
}


bool WaveFFT::FFT_filtering(TH1D* p_waveform_in, TH1D* p_waveform_out, TH1D* p_waveform_offset,
  TString channel_name, double sampling_rate, double lowpass_freq, long entry, long draw_entries){

  TH1 *hm = nullptr;
  TVirtualFFT::SetTransform(nullptr);
  int n_points = p_waveform_in->GetXaxis()->GetNbins();  
  Double_t *re_full = new Double_t[n_points];
  Double_t *im_full = new Double_t[n_points];
  TVirtualFFT *fft = nullptr;
  TVirtualFFT *fft_back = nullptr;

  TCanvas *p_output_canvas = new TCanvas("waveform_fft", "waveform_fft");
  p_output_canvas->SetRightMargin(0.1);
  p_waveform_in->Scale(-1.0);
  p_waveform_in->Add(p_waveform_offset);

  hm = p_waveform_in->FFT(hm, "MAG");  //load FFT result to hm
  //NOTE: for "real" frequencies you have to divide the x-axes range with the range of your function
   //y-axes has to be rescaled by a factor of 1/SQRT(n) to be right: this is not done automatically!
  double freq_below_10M = 0;
  double freq_below_20M = 0;

  hm->SetTitle("Magnitude");
  hm->SetBins(n_points,0,sampling_rate);
  hm->Scale(1.0/TMath::Sqrt(n_points));
  hm->GetXaxis()->SetRangeUser(0,1e9);

  for (int freq_index = 2; freq_index<=(int)(20e6/sampling_rate*n_points); 
    ++freq_index){   //freq_index=0 underflow freq_index=1 DC part (ignored here)
    freq_below_20M += hm->GetBinContent(freq_index);
    if (freq_index<=(int)(10e6/sampling_rate*n_points)){
      freq_below_10M += hm->GetBinContent(freq_index);
    }
  }


  if(entry<draw_entries){
    p_output_canvas->cd();
    p_output_canvas->SetLogy();
    
    hm->Draw("HIST");
    TPad *newpad=new TPad("newpad","a transparent pad",0,0,1,1);
    newpad->SetFillStyle(4000);
    newpad->Draw();
    newpad->cd();
    TPaveLabel *title = new TPaveLabel(0.1,0.94,0.9,0.98,
      ("<10M="+std::to_string(freq_below_10M)+", <20M=" +std::to_string(freq_below_20M)).c_str());
    title->SetFillColor(16);
    title->SetTextFont(52);       
    title->Draw();      
    p_output_canvas->SaveAs((outFolder+"/event_fft/event"+std::to_string(entry)+
      "_"+channel_name+"_fftmag.png"));
    delete newpad;
    delete title;
  }

  //Use the following method to get the full output:
  fft = TVirtualFFT::GetCurrentTransform();
  fft->GetPointsComplex(re_full,im_full);
  for (int freq_index = (int)lowpass_freq/sampling_rate*n_points; 
    freq_index<n_points-(int)lowpass_freq/sampling_rate*n_points; ++freq_index)
  {
    re_full[freq_index] = 0;
    im_full[freq_index] = 0;
  }
  fft_back = TVirtualFFT::FFT(1, &n_points, "C2R M K");
  fft_back->SetPointsComplex(re_full,im_full);
  fft_back->Transform();

  TH1::TransformHisto(fft_back,p_waveform_out,"Re");
  p_waveform_out->Scale(1.0/n_points);

  // p_waveform_out->Add(p_waveform_offset);


  if(entry<draw_entries){
    p_output_canvas->cd();
    p_waveform_out->Draw("HIST");
    p_output_canvas->SetLogy(0);
    p_output_canvas->SaveAs((outFolder+"/event_fft/event"+std::to_string(entry)+"_"+
      channel_name+"_fftback.png"));
  }


  delete hm;
  delete fft;
  delete fft_back;
  delete re_full;
  delete im_full;
  delete p_output_canvas;
  return (freq_below_10M>0.2)&&(freq_below_20M>0.3);
}