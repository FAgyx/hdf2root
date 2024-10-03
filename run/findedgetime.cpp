#include "inc/findedgetime.h"
#include "AtlasStyle/AtlasStyle.h"
#include "AtlasStyle/AtlasStyle.C"

void findedgetime_help() {
  printf("\n Usage: eventfilter [--help, -h] <inFile.root>\n");
  printf("\n  %15s  %s ","--help, -h","Shows this message.");
  printf("\n\n");
}

int main (int argc,char *argv[]) {
  SetAtlasStyle();
  if(argc<=1) {
    findedgetime_help();
    return 0;
  }

  TString inFilename = "\0";
  TString outFileFolder = "\0";
  TString outRootFilename = "\0";

  long process_entry=0;  //0 means all data
  double vth_upper_base = 0;


  for(int l=1;l<argc;l++){
    TString arg = argv[l];
   
    if(arg.Contains("--help") || arg.Contains("-h")) {
      findedgetime_help();
      return 0;
    } else if(arg.EndsWith(".root")) {
      inFilename = arg;
    } else if(arg.Contains("-e")) {
      process_entry = std::stol(argv[l+1]);
    } else if(arg.Contains("-b")) {
      vth_upper_base = std::stod(argv[l+1]);
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


  inFilename = outFileFolder+"/waveform_sel.root";
  cout << "Processing " << inFilename.Data() << " ... " << endl;
  outRootFilename = outFileFolder+"/waveform_result.root";

  TFile *p_input_rootfile = TFile::Open(inFilename.Data());

  p_input_rootfile->ls();  
  
  std::vector<int> chnls;
  chnls.push_back(2);
  // chnls.push_back(3);

  std::vector<double> vths;
  vths.push_back(0.04);




  TFile *p_output_rootfile = new TFile(outRootFilename.Data(), "RECREATE");
  if(p_output_rootfile==NULL) return 0;
  p_output_rootfile->cd();  //TFile should be created before TTree

  WaveEdgeTime waveedgetime = WaveEdgeTime(p_input_rootfile, chnls, outFileFolder);
  // tree_root->SetAutoFlush(1000);

  std::cout<<"Event find first edge started"<<std::endl;

  waveedgetime.find_result(process_entry, vths);
  p_output_rootfile->Write();

  std::cout<<"Find first edge completed successfully"<<std::endl;

  return 1;

}

