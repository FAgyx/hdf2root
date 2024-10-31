#include "inc/WaveEdgeTime.h"


WaveEdgeTime::WaveEdgeTime(TFile* p_input_rootfile, std::vector<int> chnls, 
  TString outFileFolder){

  wave_tree_in==nullptr;
  wave_tree_in=getTreeFromRoot(p_input_rootfile);
  if(wave_tree_in==nullptr){
    std::cout<<"No TTree found."<<std::endl;
    exit(0);
  }
  pb_TH1s_in=getAllBranchFromTree(wave_tree_in);
  std::cout<<pb_TH1s_in.size()<<" branch(es) loaded."<<std::endl;
  if(pb_TH1s_in.size()==0){
    std::cout<<"No TBranch found."<<std::endl;
    exit(0);
  }
  for (auto chnl : chnls){
    _chnls.push_back(chnl);
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
  TH2D* p_waveform_2d;
  double edge_time;
  std::vector<double> edge_times;
  double integral;
  std::vector<double> integrals;
  
  int total_bins;
  total_bins = p_wave_template->GetXaxis()->GetNbins();

  TCanvas *p_output_canvas = new TCanvas("waveform", "waveform");
  p_output_canvas->SetRightMargin(0.1);

  // TVectorD fitData = TVectorD(NT0FITDATA);

  

  for (int i = 0; i < chnl_number; ++i){
    p_waveform = (TH1D*)p_wave_template->Clone();
    p_wave.push_back(p_waveform); 
    edge_times.push_back(edge_time);
    integrals.push_back(integral);
  }

  for (int i = 0; i < chnl_number; ++i){
    p_waveform = (TH1D*)p_wave_template->Clone();
    p_waveform->Reset(); //resets the bin contents and errors of a histogram
    firstEdgeTime.push_back(p_waveform); 
  }

  for (int i = 0; i < chnl_number; ++i){
    Int_t nbins = p_wave_template->GetNbinsX()/10;
    Double_t xmin = p_wave_template->GetXaxis()->GetXmin();
    Double_t xmax = p_wave_template->GetXaxis()->GetXmax();
    p_waveform_2d = new TH2D("hist2D", "TH2D;Drift Time (ns); Integration", 
      nbins, xmin, xmax, 100, -100, 1000);
    // p_waveform_2d->GetYaxis()->SetCanExtend(true);
    firstEdgeTime_integration.push_back(p_waveform_2d); 
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
  
  Int_t bin_start = p_wave.at(0)->FindBin(-100.0);  
  Int_t bin_end = p_wave.at(0)->FindBin(500.0);    

  long event_print = 100;
  for (long j = 0; j < entries; ++j){  //loop by events
    if (j % event_print == 0) {
      printMemoryUsage(j,event_print);
      if (TMath::Floor(TMath::Log10(j)) > TMath::Floor(TMath::Log10(event_print))) event_print*=10;
    }

    wave_tree_in->GetEntry(j); 
    for (int i = 0; i < chnl_number; ++i){ 
      edge_times.at(i) = find_first_edge_time(p_wave.at(i),vths.at(i));

      // Calculate the sum of the histogram contents in the specified range
      integrals.at(i) = p_wave.at(i)->Integral(bin_start, bin_end);

      firstEdgeTime.at(i)->Fill(edge_times.at(i));
      firstEdgeTime_integration.at(i)->Fill(edge_times.at(i),integrals.at(i));
      
    } //for i < chnl_number
    wave_tree_out->Fill();
  } //for j < entries

  T0Fit *t0fit = new T0Fit("TestData","Test Data");

  for (int i = 0; i < chnl_number; ++i){ 
    p_output_canvas->cd();
    // firstEdgeTime.at(i)->GetXaxis()->SetRangeUser(-200,600);
    firstEdgeTime.at(i)->Draw();
    p_output_canvas->SaveAs((outFolder+"/channel"+
      std::to_string(_chnls.at(i))+"_drifttime.png"));

    p_output_canvas->cd();
    firstEdgeTime.at(i)->Draw();
    t0fit->TdcFit(firstEdgeTime.at(i),0,1);
    p_output_canvas->SaveAs((outFolder+"/channel"+
      std::to_string(_chnls.at(i))+"_drifttime_fit.png"));
    fitted_t0s.push_back(t0fit->FitData[1]);

    
    p_output_canvas->cd();
    // firstEdgeTime_integration.at(i)->GetXaxis()->SetRangeUser(-200,600);
    firstEdgeTime_integration.at(i)->Draw();
    p_output_canvas->SaveAs((outFolder+"/channel"+
      std::to_string(_chnls.at(i))+"_intvsdrift.png"));

    // gStyle->SetOptStat(1111);
    // for (int binX = 1; binX <= firstEdgeTime_integration.at(i)->GetNbinsX(); ++binX) {
    //   TH1D* p_tmp = new TH1D("histTmp", "TH1D;Integration", 
    //     firstEdgeTime_integration.at(i)->GetNbinsY(), 
    //     firstEdgeTime_integration.at(i)->GetYaxis()->GetXmin(),
    //     firstEdgeTime_integration.at(i)->GetYaxis()->GetXmax());
    //   for (int binY = 1; binY <= firstEdgeTime_integration.at(i)->GetNbinsY(); ++binY) {
    //     Double_t count = firstEdgeTime_integration.at(i)->GetBinContent(binX, binY);  // Get count from original
    //     p_tmp->AddBinContent(binY, count);
    //   }
    //   p_output_canvas->cd();
    //   p_tmp->ResetStats();
    //   p_tmp->Draw();
    //   TPaveStats *stats = (TPaveStats*)p_tmp->GetListOfFunctions()->FindObject("stats");
    //   if (stats) {
    //     // Set the position and size of the stats box (NDC coordinates between 0 and 1)
    //     stats->SetX1NDC(0.7);  // X1 (left border)
    //     stats->SetX2NDC(0.9);  // X2 (right border)
    //     stats->SetY1NDC(0.7);  // Y1 (bottom border)
    //     stats->SetY2NDC(0.9);  // Y2 (top border)
    //     // Update the canvas to reflect the new size
    //     p_output_canvas->Modified();
    //     p_output_canvas->Update();
    // }

    //   p_output_canvas->SaveAs((outFolder+"/result/channel"+
    //     std::to_string(_chnls.at(i))+"_"+
    //     std::to_string(firstEdgeTime_integration.at(i)->GetXaxis()->GetBinCenter(binX))+
    //     ".png"));
    //   delete p_tmp;


    // }
    
  } //for i < chnl_number
  



}

double WaveEdgeTime::find_first_edge_time(TH1D* p_wave_in, double vth){
  int total_bins = p_wave_in->GetXaxis()->GetNbins();
  double edge_time=-999;
  for(int k = 5; k <total_bins;++k){
    if((p_wave_in->GetBinContent(k-4)<vth)
      &&(p_wave_in->GetBinContent(k-3)>vth)
      &&(p_wave_in->GetBinContent(k-2)>vth)
      &&(p_wave_in->GetBinContent(k-1)>vth))
    {
      edge_time = p_wave_in->GetXaxis()->GetBinCenter(k-3);
      break;
    }     
  } //for k <total_bins
  return edge_time;
}
