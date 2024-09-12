#include "inc/rootdraw.hh"

void rootdraw_help() {
  printf("\n Usage: rootdraw [--help, -h] <inFile.root> <outFile.png(=inFile.root)>\n");
  printf("\n  %15s  %s ","--help, -h","Shows this message.");
  printf("\n\n");
}

int main (int argc,char *argv[]) {
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

  return 0;

}

