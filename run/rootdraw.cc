#include "inc/rootdraw.hh"

void rootdraw_help() {
  printf("\n Usage: rootdraw [--help, -h] <inFile.root> <outFile.png(=inFile.root)>\n");
  printf("\n  %15s  %s ","--help, -h","Shows this message.");
  printf("\n\n");
}

int main (int argc,char *argv[]) {
  SetAtlasStyle();
  if(argc<=1) {
    rootdraw_help();
    return 0;
  }

  TString inFilename = "\0";
  TString outFilename = "\0";


  for(int l=1;l<argc;l++){
    TString arg = argv[l];
   
    if(arg.Contains("--help") || arg.Contains("-h")) {
      rootdraw_help();
      return 0;
    } else if(arg.EndsWith(".root")) {
      inFilename = arg;
    } else if((arg.EndsWith(".png")) || (arg.EndsWith(".pdf"))){
      outFilename = arg;
    } 
  }
  
  cout << "Drawing " << inFilename.Data() << " ... " << endl;
  
  if(inFilename=="\0")
    return 0;
  
  if(outFilename=="\0") {
    outFilename = inFilename;
    outFilename.ReplaceAll("|","");
    outFilename.ReplaceAll(" ","_");
  
    if(outFilename.Contains(".root"))
      outFilename.ReplaceAll(".root",".png");    
  }

  TFile *p_input_rootfile = TFile::Open(inFilename.Data());
  p_input_rootfile->ls();
  
  
  TTree* tree_attr;
  p_input_rootfile->GetObject("root_att", tree_attr);
  // tree_attr->ls();
  // tree_attr->Print();

  TBranch *channel2_horiz_offset = tree_attr->GetBranch("channel2_horiz_offset");
  double horiz_offset;
  channel2_horiz_offset->SetAddress(&horiz_offset);
  channel2_horiz_offset->GetEntry(0);
  std::cout<<"horiz_offset = "<<horiz_offset<<std::endl;
  TBranch *channel2_horiz_interval = tree_attr->GetBranch("channel2_horiz_interval");
  double horiz_interval;
  channel2_horiz_interval->SetAddress(&horiz_interval);
  channel2_horiz_interval->GetEntry(0);
  std::cout<<"horiz_interval = "<<horiz_interval<<std::endl;
  TBranch *channel2_nom_subarray_count = tree_attr->GetBranch("channel2_nom_subarray_count");
  int nom_subarray_count;
  channel2_nom_subarray_count->SetAddress(&nom_subarray_count);
  channel2_nom_subarray_count->GetEntry(0);
  std::cout<<"nom_subarray_count = "<<nom_subarray_count<<std::endl;
  TBranch *channel2_wave_array_count = tree_attr->GetBranch("channel2_wave_array_count");
  int wave_array_count;
  channel2_wave_array_count->SetAddress(&wave_array_count);
  channel2_wave_array_count->GetEntry(0);
  std::cout<<"wave_array_count = "<<wave_array_count<<std::endl;
  TBranch *channel2_vertical_gain = tree_attr->GetBranch("channel2_vertical_gain");
  double vertical_gain;
  channel2_vertical_gain->SetAddress(&vertical_gain);
  channel2_vertical_gain->GetEntry(0);
  std::cout<<"vertical_gain = "<<vertical_gain<<std::endl;
  TBranch *channel2_vertical_offset = tree_attr->GetBranch("channel2_vertical_offset");
  double vertical_offset;
  channel2_vertical_offset->SetAddress(&vertical_offset);
  channel2_vertical_offset->GetEntry(0);
  std::cout<<"vertical_offset = "<<vertical_offset<<std::endl;


  TTree* tree_root;
  p_input_rootfile->GetObject("root", tree_root);
  tree_root->ls();
  tree_root->Print();
  int n_points;
  n_points = wave_array_count/nom_subarray_count;
  TBranch *channel2 = tree_root->GetBranch("channel2");
  char channel2_array[n_points];
  channel2->SetAddress(&channel2_array);
  channel2->GetEntry(0);
  TH1D *p_waveform = new TH1D("waveform", "waveform", 
    n_points,horiz_offset*1e9,(horiz_offset+horiz_interval*n_points)*1e9);
  double channel2_double[nom_subarray_count][n_points];
  for (int i = 0; i < n_points; ++i){
    p_waveform->SetBinContent(i+1,channel2_array[i]*vertical_gain-vertical_offset);
  }
  p_waveform->GetXaxis()->SetTitle("Time (ns)");
  p_waveform->GetYaxis()->SetTitle("Voltage (V)");
  TCanvas *p_output_canvas = new TCanvas("waveform", "waveform");
  p_output_canvas->cd();
  p_waveform->Draw();
  p_output_canvas->SaveAs("test.png");


  //Compute the transform and look at the magnitude of the output
  TH1 *hm =nullptr;
  TVirtualFFT::SetTransform(nullptr);
  hm = p_waveform->FFT(hm, "MAG");
  hm->SetTitle("Magnitude of the 1st transform");
  hm->Draw();
  p_output_canvas->SaveAs("fft.png");

  return 0;

}

