#include "inc/rootdraw.h"

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

  std::vector<int> chnls;
  chnls.push_back(2);
  chnls.push_back(3);
  WaveAttr waveattr = WaveAttr(tree_attr,chnls);
  


  TTree* tree_root;
  p_input_rootfile->GetObject("root", tree_root);
  tree_root->ls();
  tree_root->Print();

  TFile *p_output_rootfile = new TFile("waveform.root", "RECREATE");
  p_output_rootfile->cd();

  WaveData wavedata = WaveData(tree_root,&waveattr,chnls);
  wavedata.GetRawData(100);
  wavedata.Lowpass_FFT();
  p_output_rootfile->Write();




  // p_output_canvas->cd();
  // p_output_canvas->SetLogy(1);
  // //Compute the transform and look at the magnitude of the output
  

  return 0;

}

