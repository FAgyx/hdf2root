#include "inc/doT0Fit.h"
#include "AtlasStyle/AtlasStyle.h"
#include "AtlasStyle/AtlasStyle.C"
#include "inc/T0Fit.h"

void doT0Fit_help() {
  printf("\n Usage: doT0Fit_help [--help, -h] <inFile.root>\n");
  printf("\n  %15s  %s ","--help, -h","Shows this message.");
  printf("\n\n");
}

int main (int argc,char *argv[]) {
  SetAtlasStyle();
  if(argc<=1) {
    doT0Fit_help();
    return 0;
  }

  TString inFilename = "\0";
  TString outFileFolder = "\0";
  TString outRootFilename = "\0";

  long process_entry=0;  //0 means all data
  double vth = 0;


  for(int l=1;l<argc;l++){
    TString arg = argv[l];
   
    if(arg.Contains("--help") || arg.Contains("-h")) {
      doT0Fit_help();
      return 0;
    } else if(arg.EndsWith(".root")) {
      inFilename = arg;
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



  inFilename = outFileFolder+"/waveform_result.root";
  cout << "Processing " << inFilename.Data() << " ... " << endl;


  TFile *p_input_rootfile = TFile::Open(inFilename.Data());

  p_input_rootfile->ls();  
  
  std::vector<int> chnls;
  chnls.push_back(2);
  // chnls.push_back(3);

  TCanvas *p_output_canvas = new TCanvas("waveform", "waveform");
  p_output_canvas->SetRightMargin(0.1);


  T0Fit *t0fit = new T0Fit("TestData","Test Data");
  TH1D *firstEdgeTime;
  p_input_rootfile->GetObject("waveform", firstEdgeTime);
  // firstEdgeTime->Draw();
  t0fit->TdcFit(firstEdgeTime,0,1);
  double t0 = t0fit->FitData[1];

  
  std::cout<<"Fitted T0 is: "<<t0<<std::endl;

  TH2D *h2;
  p_input_rootfile->GetObject("hist2D", h2);
  h2->Draw("COLZ");
  p_output_canvas->SaveAs((outFileFolder+"/2d.png"));

  // Get original binning parameters
  int nbinsX = h2->GetNbinsX();
  double xMin = h2->GetXaxis()->GetXmin();
  double xMax = h2->GetXaxis()->GetXmax();
  int nbinsY = h2->GetNbinsY();
  double yMin = h2->GetYaxis()->GetXmin();
  double yMax = h2->GetYaxis()->GetXmax();

  double xMin_shifted = xMin - t0;
  double xMax_shifted = xMax - t0;


  // Create a new TH2D with shifted x-axis
  TH2D* hShifted2D = new TH2D("hShifted2D", "Shifted Histogram",
                            nbinsX, xMin_shifted, xMax_shifted,
                            nbinsY, yMin, yMax);

  // Copy bin content from the original histogram to the shifted one
  for (int i = 1; i <= nbinsX; i++) {
    for (int j = 1; j <= nbinsY; j++) {
      double content = h2->GetBinContent(i, j);
      double error = h2->GetBinError(i, j);
      hShifted2D->SetBinContent(i, j, content);
      hShifted2D->SetBinError(i, j, error);  // Preserve bin errors if needed
    }
  }

  hShifted2D->Draw("COLZ");
  hShifted2D->SetXTitle("Drift time (ns)");
  hShifted2D->SetYTitle("waveform integral (arb. unit)");
  hShifted2D->GetXaxis()->SetRangeUser(-100,600);
  p_output_canvas->SaveAs((outFileFolder+"/2d_shifted.png"));




  TH1D* h1 = new TH1D("h1", "Projection of Y Mean onto X", nbinsX, xMin_shifted, xMax_shifted);

  // Loop over the x-axis bins
  for (int i = 1; i <= nbinsX; i++) {
    // Get the projection of the y-axis for the current x-bin
    TH1D* hProjY = hShifted2D->ProjectionY("t="+TString::Format("%.2f", 
      hShifted2D->GetXaxis()->GetBinCenter(i))+" ns", i, i);
    gStyle->SetOptStat(1111);
    gStyle->SetStatFontSize(0.05);  // 5% of the pad size
    gStyle->SetStatW(0.4);
    gStyle->SetStatX(0.90);
    gStyle->SetStatY(0.90);
    gStyle->SetStatH(0.3);
    hProjY->GetYaxis()->SetTitle("Entries");
    hProjY->Draw("HIST");
    // TPaveStats *stats = (TPaveStats*)hProjY->GetListOfFunctions()->FindObject("stats");
    // if (stats) {
    //     // Set the position and size of the stats box (NDC coordinates between 0 and 1)
    //     stats->SetX1NDC(0.7);  // X1 (left border)
    //     stats->SetX2NDC(0.9);  // X2 (right border)
    //     stats->SetY1NDC(0.7);  // Y1 (bottom border)
    //     stats->SetY2NDC(0.9);  // Y2 (top border)
    //     // Update the canvas to reflect the new size
    //     p_output_canvas->Modified();
    //     p_output_canvas->Update();
    // }

    p_output_canvas->SaveAs((outFileFolder+"/result/"+
      hShifted2D->GetXaxis()->GetBinCenter(i)+".png"));


    // Calculate the mean of the y-axis for this projection
    double meanY = hProjY->GetMean();

    // Set the mean in the corresponding bin of the new TH1D
    h1->SetBinContent(i, meanY);

    // Clean up the temporary projection
    delete hProjY;
  }


  h1->GetXaxis()->SetRangeUser(0,500);
  h1->Draw("HIST");
  h1->SetXTitle("Drift time (ns)");
  h1->SetYTitle("Waveform integral (arb. unit)");
  p_output_canvas->SaveAs((outFileFolder+"/dE_dt.png"));



  std::cout<<"doT0fit completed successfully"<<std::endl;

  return 1;

}

