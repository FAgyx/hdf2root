//  Alberto Martinez de la Ossa (FLA @ DESY)
//  July 27, 2011
//  Revised: Mar 26, 2014

#ifndef DOFFT
#define DOFFT



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



#include "inc/WaveAttr.h"
#include "inc/WaveData.h"
#include "inc/WaveFFT.h"


using namespace std;


void doFFT_help();
int main (int argc,char *argv[]);


#endif

