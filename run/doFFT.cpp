#include "inc/doFFT.h"
#include "AtlasStyle/AtlasStyle.h"
#include "AtlasStyle/AtlasStyle.C"

void doFFT_help() {
  printf("\n Usage: rootdraw [--help, -h] <inFile.root> [-e] <entries>\n");
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


  for(int l=1;l<argc;l++){
    TString arg = argv[l];
   
    if(arg.Contains("--help") || arg.Contains("-h")) {
      doFFT_help();
      return 0;
    } else if(arg.EndsWith(".root")) {
      inFilename = arg;
    } else if(arg.Contains("-e")) {
      process_entry = std::stol(argv[l+1]);
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

  inFilename = outFileFolder+"/waveform.root";
  cout << "Processing " << inFilename.Data() << " ... " << endl;
  outRootFilename = outFileFolder+"/waveform_fft.root";

  TFile *p_input_rootfile = TFile::Open(inFilename.Data());

  p_input_rootfile->ls();  
  
  std::vector<int> chnls;
  chnls.push_back(2);
  chnls.push_back(3);
  long draw_entry=0;

  

  TFile *p_output_rootfile = new TFile(outRootFilename.Data(), "RECREATE");
  if(p_output_rootfile==NULL) return 0;
  p_output_rootfile->cd();  //TFile should be created before TTree

  WaveFFT wavefft = WaveFFT(p_input_rootfile, chnls, outFileFolder);
  // tree_root->SetAutoFlush(1000);

  std::cout<<"FFT started"<<std::endl;

  wavefft.Lowpass_FFT(process_entry,draw_entry,70);
  p_output_rootfile->Write();

  std::cout<<"doFFT finished successfully"<<std::endl;

  return 1;

}

