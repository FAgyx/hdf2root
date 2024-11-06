#include "inc/eventfilter.h"
#include "AtlasStyle/AtlasStyle.h"
#include "AtlasStyle/AtlasStyle.C"

void eventfilter_help() {
  printf("\n Usage: eventfilter [--help, -h] <inFile.root> [-e] <entries>\n");
  printf("\n  %15s  %s ","--help, -h","Shows this message.");
  printf("\n\n");
}

int main (int argc,char *argv[]) {
  SetAtlasStyle();
  if(argc<=1) {
    eventfilter_help();
    return 0;
  }

  TString inFilename = "\0";
  TString outFileFolder = "\0";
  TString outRootFilename = "\0";

  long process_entry=0;  //0 means all data
  long draw_entry=0;   //0 means no draw
  double vth_base = 0;


  for(int l=1;l<argc;l++){
    TString arg = argv[l];
   
    if(arg.Contains("--help") || arg.Contains("-h")) {
      eventfilter_help();
      return 0;
    } else if(arg.EndsWith(".root")) {
      inFilename = arg;
    } else if(arg.Contains("-e")) {
      process_entry = std::stol(argv[l+1]);
    } else if(arg.Contains("-b")) {
      vth_base = std::stod(argv[l+1]);
    } else if(arg.Contains("-d")) {
      draw_entry = std::stod(argv[l+1]);
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
  mkdir(outFileFolder+"/event_select",S_IRWXU|S_IRGRP|S_IROTH);

  inFilename = outFileFolder+"/waveform_fft.root";
  cout << "Processing " << inFilename.Data() << " ... " << endl;
  outRootFilename = outFileFolder+"/waveform_sel.root";

  outFileFolder = outFileFolder + "/event_select";

  TFile *p_input_rootfile = TFile::Open(inFilename.Data());

  p_input_rootfile->ls();  
  
  std::vector<int> chnls;
  chnls.push_back(2);
  chnls.push_back(3);

  

  // vth_base = -0.105;

  TFile *p_output_rootfile = new TFile(outRootFilename.Data(), "RECREATE");
  if(p_output_rootfile==NULL) return 0;
  p_output_rootfile->cd();  //TFile should be created before TTree

  WaveFilter wavefilter = WaveFilter(p_input_rootfile, chnls, outFileFolder);
  // tree_root->SetAutoFlush(1000);

  std::cout<<"Event filter started"<<std::endl;

  std::cout<<"Channel"<<chnls.at(0)<<" baseline rejection at "<<vth_base<<" V"<<std::endl;

  wavefilter.filter_by_amplitude(process_entry,draw_entry,0,0.04,vth_base);
  p_output_rootfile->Write();

  std::cout<<"Event filter finished successfully"<<std::endl;

  return 1;

}

