//  Alberto Martinez de la Ossa (FLA @ DESY)
//  July 27, 2011
//  Revised: Mar 26, 2014

#ifndef EVENTFILTER
#define EVENTFILTER



#include <iostream>
#include <string>
#include <vector>
#include <sys/stat.h>

#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TCanvas.h>

#include "TH1D.h"
#include "TVirtualFFT.h"
#include "TF1.h"
#include "TMath.h"

#include "inc/WaveFilter.h"



using namespace std;



void eventfilter_help();
int main (int argc,char *argv[]);

#endif

