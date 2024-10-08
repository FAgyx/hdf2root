#include "inc/rootdraw.h"
#include "AtlasStyle/AtlasStyle.h"
#include "AtlasStyle/AtlasStyle.C"

void rootdraw_help() {
  printf("\n Usage: rootdraw [--help, -h] <inFile.root> [-e] <entries>\n");
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
  TString outFileFolder = "\0";
  TString outRootFilename = "\0";

  long process_etry=0;  //0 means all data


  for(int l=1;l<argc;l++){
    TString arg = argv[l];
   
    if(arg.Contains("--help") || arg.Contains("-h")) {
      rootdraw_help();
      return 0;
    } else if(arg.EndsWith(".root")) {
      inFilename = arg;
    } else if(arg.Contains("-e")) {
      process_etry = std::stol(argv[l+1]);
    }
  }
  
  cout << "Drawing " << inFilename.Data() << " ... " << endl;
  
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

  outRootFilename = outFileFolder+"/waveform.root";



  TFile *p_input_rootfile = TFile::Open(inFilename.Data());

  p_input_rootfile->ls();  
  
  TTree* tree_attr;  
  p_input_rootfile->GetObject("root_att", tree_attr);
  // tree_attr->ls();
  // tree_attr->Print();

  std::vector<int> chnls;
  chnls.push_back(2);
  chnls.push_back(3);
  WaveAttr waveattr = WaveAttr(tree_attr,chnls);
  

  long draw_entry=0;
  TTree* tree_root;
  p_input_rootfile->GetObject("root", tree_root);
  tree_root->ls();
  tree_root->Print();
  // tree_root->SetAutoFlush(1000);

  TFile *p_output_rootfile = new TFile(outRootFilename.Data(), "RECREATE");
  if(p_output_rootfile==NULL) return 0;
  p_output_rootfile->cd();

  WaveData wavedata = WaveData(tree_root,&waveattr,chnls,outFileFolder);
  wavedata.GetRawData(process_etry,draw_entry);
  // wavedata.Lowpass_FFT(draw_entry,70);
  p_output_rootfile->Write();

  std::cout<<"Run finished successfully"<<std::endl;




  // p_output_canvas->cd();
  // p_output_canvas->SetLogy(1);
  // //Compute the transform and look at the magnitude of the output
  

  return 0;

}

