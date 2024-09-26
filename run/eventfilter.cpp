#include "inc/eventfilter.h"

void rootdraw_help() {
  printf("\n Usage: rootdraw [--help, -h] <inFile.dat> \n");
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

  long process_etry=0;  //0 means all data


  for(int l=1;l<argc;l++){
    TString arg = argv[l];
   
    if(arg.Contains("--help") || arg.Contains("-h")) {
      rootdraw_help();
      return 0;
    } else if(arg.EndsWith(".dat")) {
      inFilename = arg;
      inFilename.ReplaceAll(".dat","");
      inFilename.ReplaceAll("raw/","output/");
      inFilename = inFilename + "/waveform_fft.root"
    }
  }
  
  cout << "Processing " << inFilename.Data() << " ... " << endl;
  
  if(inFilename=="\0")
    return 0;
  
  // if(outFilename=="\0") {
  //   outFilename = inFilename;
  //   outFilename.ReplaceAll("|","");
  //   outFilename.ReplaceAll(" ","_");
  //   outFilename.ReplaceAll("raw/","");
  
  //   if(outFilename.Contains(".root"))
  //     outFilename.ReplaceAll(".root","");    
  // }
  // outFilename = "output/"+outFilename;
  // mkdir(outFilename.Data(),S_IRWXU|S_IRGRP|S_IROTH);

  // outFilename = outFilename+"/waveform_fft.root";

  TFile *p_input_rootfile = TFile::Open(inFilename.Data());
  if(p_input_rootfile==NULL) return 0;

  p_input_rootfile->ls();  
  
  TTree* p_tree_waveform_fft;  
  p_input_rootfile->GetObject("waveTH1_tree", p_tree_waveform_fft);
  p_tree_waveform_fft->ls();
  // // tree_attr->Print();

  // std::vector<int> chnls;
  // chnls.push_back(2);
  // chnls.push_back(3);
  // WaveAttr waveattr = WaveAttr(tree_attr,chnls);
  

  // long draw_entry=0;
  // TTree* tree_root;
  // p_input_rootfile->GetObject("root", tree_root);
  // tree_root->ls();
  // tree_root->Print();

  // TFile *p_output_rootfile = new TFile(outFilename.Data(), "RECREATE");
  // if(p_output_rootfile==NULL) return 0;
  // p_output_rootfile->cd();

  // WaveData wavedata = WaveData(tree_root,&waveattr,chnls);
  // wavedata.GetRawData(process_etry,draw_entry);
  // wavedata.Lowpass_FFT(draw_entry,70);
  // p_output_rootfile->Write();

  

  return 0;

}

