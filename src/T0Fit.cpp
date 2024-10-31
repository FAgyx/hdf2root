#include "inc/T0Fit.h"

TString fitDataNames[NT0FITDATA] = {"NEvents",
                                    "T0",
                                    "T0Error",
                                    "T0Slope",
                                    "T0SlopeError",
                                    "Background",
                                    "BackgroundError",
                                    "T0Chi2",
                                    "TMax",
                                    "TMaxError",
                                    "TMaxSlope",
                                    "TMaxSlopeError",
                                    "TMaxChi2",
                                    "ADCPeak",
                                    "ADCPeakError",
                                    "ADCWidth",
                                    "ADCWidthError",
                                    "ADCSkew",
                                    "ADCSkewError",
                                    "ADCChi2"};

TString fitDataUnits[NT0FITDATA] = {"", 
                                    " [ns]",
                                    " [ns]",
                                    " [ns]",
                                    " [ns]",
                                    "",
                                    "",
                                    "",
                                    " [ns]",
                                    " [ns]",
                                    " [ns]",
                                    " [ns]",
                                    "",
                                    " [ns]",
                                    " [ns]",
                                    " [ns]",
                                    " [ns]",
                                    "",
                                    "",
                                    ""};

// Constructor.  Silly really, the T0fit class is really just a namespace for file and histogram names
T0Fit::T0Fit( const char *fname, const char *hname, const int notitle ) {
  sprintf(Filename,"%s",fname);
  sprintf(Hisname,"%s",hname);
  T0fitstyle(notitle);
  clearfitdata();
}

/************************************************************
*  Fit Time spectrum for T0 and Tmax and optionally create PNG
*  h       = Drift Time spectrum histogram to fit (pointer)
*  pardata = array of fit data used to set initial values of parameters (optional - if pointer to array is NULL, defaults will be determined from histogram)
*            If you fit a chamber-level time spectra first, can use parameters from chamber fit for fits of smaller units, e.g. individual tube histograms.
*  plot    = Make PNG of fit histogram.  Default name: <h->GetName()>.png.  Default gPad is used 
************************************************************/
int T0Fit::TdcFit(TH1D *h, double pardata[NT0FITDATA], const int plot ) {
  double t0,slope,ampl,xmin,xmax,maxval=130.;
  double tmax,tmslope,tmback=0.,A=3.,B=-0.0015;
 
  clearfitdata();

  // Protect for null histograms and low statistics time spectra
  if( h == 0 || h->GetEntries()<1000. ) return -1;
  printf("Fitting %s\n",h->GetName());

  //gStyle->SetOptStat("e"); 

  // Reset histogram title if desired
  if( Hisname[0] != '\0' ) {
    h->SetTitle(Form("%s %s",Hisname,h->GetTitle()));
  }

  FitData[0] = CheckNaN(h->GetEntries());
  
//  default text box locations  
  TPaveText *t0PT = new TPaveText(0.55,0.78,0.95,0.95,"NDC");
  TPaveText *tmPT = new TPaveText(0.55,0.63,0.95,0.78,"NDC");
  TPaveText *tdPT = new TPaveText(0.55,0.55,0.95,0.63,"NDC");

//  Fit T0.  Used pardata for initial values if available, else estimate from histogram data
  maxval = h->GetMaximum();
  if( pardata ) {
    t0    = pardata[1];
    slope = pardata[3];
  } else {
    t0     = h->GetBinCenter(h->FindFirstBinAbove(0.45*maxval));
    slope  = 2.5;
  }
  //  ampl  = h->GetEntries()/4000.;
  ampl  = maxval/1.1;
  xmin  = t0 - 300.;
  //if( xmin < -200. ) xmin = -200.;
  xmax = t0 + 35;

  printf("Estimated parameters t0=%.1f slope=%.1f back=%.1f ampl=%.1f xmin=%.1f xmax=%.1f maxval=%.1f\n",t0,slope,0.,ampl,xmin,xmax,maxval);

    // Define TF1 function for T0 fit
  TF1 *ft0 = new TF1("mt_t0_fermi",mt_t0_fermi,xmin,xmax,4);
  ft0->SetParameters(t0,slope,0.,ampl);
  //changed
  ft0->SetLineColor(kRed);
  ft0->SetParLimits(2,0.,ampl/1.0);  //Do not allow negative noise
  h->Fit("mt_t0_fermi","R");

  printf("Noise=%.1f\n",ft0->GetParameter(2));
  if( ft0->GetParameter(2) < 0. ) {
    printf("Noise<0; Refit %s\n",h->GetName());
    ft0->FixParameter(2,0.);
    h->Fit("mt_t0_fermi","R");
  }

// Code to refit pathalogical time spectra.
// refit with a smaller range if problems: 
//  slope too small refit 
//  fitted T0 very different from estimated T0
//  if( ft0->GetParameter(1) < 1. || TMath::Abs(ft0->GetParameter(0)-t0) > 5. ) {
//    printf("Refit %s\n",h->GetName());
//    refit++;
//    ft0 = new TF1("mt_t0_fermi",mt_t0_fermi,xmin,xmax-35.,4);
//    ft0->SetParameters(t0,2.,0.,ampl);
//    h->Fit("mt_t0_fermi","r");
//  }
// if still slope fix slope=2
//  if( ft0->GetParameter(1) < 1. ) {
//    printf("Refit AGAIN %s\n",h->GetName());
//    refit++;
//    ft0 = new TF1("mt_t0_fermi",mt_t0_fermi,xmin,xmax-20.,4);
//    ft0->SetParameters(t0,2.,0.,ampl);
//    ft0->SetParLimits(2,2.,2.);     //force slope=2
//    h->Fit("mt_t0_fermi","r");
//  }

  t0PT->AddText(Form("T0 %.2lf #pm %.2lf ns",ft0->GetParameter(0),ft0->GetParError(0)));
  t0PT->AddText(Form("Slope %.2lf #pm %.2lf /ns",ft0->GetParameter(1),ft0->GetParError(1)));
  t0PT->SetTextColor(2);
  t0PT->Draw();
  FitData[1] = CheckNaN(ft0->GetParameter(0));
  FitData[2] = CheckNaN(ft0->GetParError(0));
  FitData[3] = CheckNaN(ft0->GetParameter(1));
  FitData[4] = CheckNaN(ft0->GetParError(1));
  FitData[5] = CheckNaN(ft0->GetParameter(2));
  FitData[6] = CheckNaN(ft0->GetParError(2));
  double dof = ft0->GetNDF();
  if( dof ) FitData[7] = CheckNaN(ft0->GetChisquare()/dof);

  // Only do Tmax fit if there are more than 2000 entries
  if( h->GetEntries() >2000. ) {
  // Do Tmax fit, first estimate input parameters
    t0 = FitData[1];
    if( pardata ) {
      tmax    = pardata[8]+t0;
      tmslope = pardata[10];
      tmback  = FitData[5];    //use background from T0 fit
    } else {
      tmax    = h->GetBinCenter(h->FindLastBinAbove(0.1*maxval));
      tmslope = 8.;
      tmback  = FitData[5];    //use background from T0 fit
    }

    tmax = t0+400.;

    xmin = tmax - 80.;
    xmax = tmax + 100.;
    if( xmax > 900. &&(!IsMDT)) xmax=900.;
    if( xmax > 1200. &&(IsMDT)) xmax=1200.;
    printf("Estimated Tmax parameters tmax=%.1f slope=%.1f back=%.1f A=%.1f B=%.5f xmin=%.1f xmax=%.1f\n",
	   tmax,tmslope,tmback,A,B,xmin,xmax);
    // Define TF1 function for Tmax fit
    TF1 *ftm = new TF1("mt_tmax_fermi",mt_tmax_fermi,xmin,xmax,6);
    ftm->SetParameters(tmax,tmslope,tmback,A,B,t0);
    ftm->SetParLimits(6,t0,t0);     //Use t0 from ft0 fit
    if(IsMDT) ftm->SetParLimits(3,0.,1400.);  //Tmax limits
    else ftm->SetParLimits(3,0.,1000.);  //Tmax limits
    ftm->SetLineColor(8);
    h->Fit("mt_tmax_fermi","r+");   //the "+" is to add the function to the list
    
    ftm->SetLineColor(8);
    tmPT->AddText(Form("Tmax %.1lf #pm %.1lf ns",  ftm->GetParameter(0),ftm->GetParError(0)));
    tmPT->AddText(Form("Slope %.1lf #pm %.1lf /ns",ftm->GetParameter(1),ftm->GetParError(1)));
    tmPT->SetTextColor(8);
    tmPT->Draw();

    // Compute max drift time of time spectra: Tmax-T0
    double dtmax  = ftm->GetParameter(0) - ft0->GetParameter(0);
    double dtmaxe = TMath::Sqrt( ftm->GetParError(0)*ftm->GetParError(0)+
				  ft0->GetParError(0)*ft0->GetParError(0) );
    tdPT->AddText(Form("DTmax %.1lf #pm %.1lf ns",dtmax,dtmaxe));
    tdPT->SetTextColor(1);
    tdPT->Draw();
    FitData[8]  = CheckNaN(dtmax);
    FitData[9]  = CheckNaN(dtmaxe);
    FitData[10] = CheckNaN(ftm->GetParameter(1));
    FitData[11] = CheckNaN(ftm->GetParError(1));
    dof = ftm->GetNDF();
    if( dof ) FitData[12] = CheckNaN(ftm->GetChisquare()/dof);
  }

  t0PT->Draw();  //should not be needed, done above, but does not appear if you don't
  return 0;
}  //end T0Fit::TdcFit



// Convert NaNs and Infs to some default value 
double T0Fit::CheckNaN( const double x, const double defaultx ) {
  if( std::isnan(x) || std::isinf(x) ) return defaultx;
  return x;
}

//  Set ROOT style parameters for plots - set notitle=1 to remove histogram titles from plots
void T0Fit::T0fitstyle( const int notitle ) {
  gStyle->SetTitleH(0.12);
  gStyle->SetTitleX(0.55);
  gStyle->SetTitleY(1.0);
  gStyle->SetStatW(0.48);
  gStyle->SetStatX(0.98);
  gStyle->SetStatY(0.90);
  gStyle->SetStatH(0.35);
  if( notitle ) {
    gStyle->SetTitleX(999.);
    gStyle->SetTitleY(999.);
  }
}

