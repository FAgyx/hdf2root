#include "inc/doFFT.h"
#include "AtlasStyle/AtlasStyle.h"
#include "AtlasStyle/AtlasStyle.C"

void doFFT_help() {
  printf("\n Usage: doFFT [--help, -h] <inFile.root> [-e] <entries> [-d] <draw entries>\n");
  printf("\n  %15s  %s ","--help, -h","Shows this message.");
  printf("\n\n");
}

int main (int argc,char *argv[]) {
  SetAtlasStyle();
  if(argc<=1) {
    doFFT_help();
    return 0;
  }

  TString inFilename = "\0";
  TString outFileFolder = "\0";
  TString outRootFilename = "\0";

  long process_entry=0;  //0 means all data
  long draw_entry=0;     //0 means no draw


  for(int l=1;l<argc;l++){
    TString arg = argv[l];
   
    if(arg.Contains("--help") || arg.Contains("-h")) {
      doFFT_help();
      return 0;
    } else if(arg.EndsWith(".root")) {
      inFilename = arg;
    } else if(arg.Contains("-e")) {
      process_entry = std::stol(argv[l+1]);
    } else if(arg.Contains("-d")) {
      draw_entry = std::stol(argv[l+1]);
    }
  }
  
  if(inFilename=="\0")
    return 0;
  
  if(outFileFolder=="\0") {
    outFileFolder = inFilename;
    outFileFolder.ReplaceAll("|","");
    outFileFolder.ReplaceAll(" ","_");
    outFileFolder.ReplaceAll("raw/","");
  
    if(outFileFolder.Contains(".root"))
      outFileFolder.ReplaceAll(".root","");    
  }
  outFileFolder = "output/"+outFileFolder;
  mkdir(outFileFolder.Data(),S_IRWXU|S_IRGRP|S_IROTH);
  mkdir(outFileFolder+"/event_fft",S_IRWXU|S_IRGRP|S_IROTH);
  mkdir(outFileFolder+"/event_sel",S_IRWXU|S_IRGRP|S_IROTH);

  inFilename = outFileFolder+"/waveform.root";
  cout << "Processing " << inFilename.Data() << " ... " << endl;
  outRootFilename = outFileFolder+"/waveform_fft.root";


  TFile *p_input_rootfile = TFile::Open(inFilename.Data());

  p_input_rootfile->ls();  
  
  std::vector<int> chnls;
  chnls.push_back(2);
  chnls.push_back(3);

  double chnl2_baseline = -0.121;  //Volt
  // double chnl2_baseline = 0.0;  //Volt
  double chnl3_baseline = 0.0;   //volt
  std::vector<double> chnl_offsets;
  chnl_offsets.push_back(chnl2_baseline);
  chnl_offsets.push_back(chnl3_baseline);

  double freq_cut = 70e6; //in Hz
  double sampling_rate = 2.5e9; //in Hz
  

  

  TFile *p_output_rootfile = new TFile(outRootFilename.Data(), "RECREATE");
  if(p_output_rootfile==NULL) return 0;
  p_output_rootfile->cd();  //TFile should be created before TTree

  WaveFFT wavefft = WaveFFT(p_input_rootfile, chnls, outFileFolder);
  // tree_root->SetAutoFlush(1000);

  std::cout<<"FFT started"<<std::endl;

  wavefft.Lowpass_FFT_filter(process_entry,draw_entry,sampling_rate,freq_cut,chnl_offsets,0);
  p_output_rootfile->Write();

  std::cout<<"doFFT finished successfully"<<std::endl;

  return 1;

}

