#include "TH2F.h"
#include "TFile.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TString.h"
#include "TROOT.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cstdio>

/* --------------------------
simple script that grabs a number of 
histograms produce by HFanalysis
and makes pulse shape profiles 
--------------------------- */ 

using namespace std;

void plotPulseProfile(TString runNumber){
  gStyle->SetOptStat(000000);
  //gROOT->ProcessLine(".L ~/tdrstyle.C");
  //gROOT->ProcessLine("setTDRStyle()");
/*  TString inputFileName = argv[1];
  double rangeBoundary = 30000;

  if( argc < 2 ){
    cout << "Please provide the input file as a command line argument" << endl; 
    return 1;
  }else if( argc < 3 ){
    rangeBoundary = atof(argv[2]);
  }
    
*/ 
  
TString inputName = "HFanalysisTree_000"+runNumber+".root";
TString outputNameRoot = "/afs/cern.ch/user/y/yanchu/work/public/cms904/plusprofile/CapIDvsBXProfile_"+runNumber+".root";
TString outputNamePngfolder = "/afs/cern.ch/user/y/yanchu/www/904LED/plotCapIDvsBXProfile_"+runNumber;
//TString outputNameRootTest = "/afs/cern.ch/work/e/ealvesco/CMSSW_7_5_0_pre5/src/HFcommissioning/Analysis/test/plotCapIDvsBXProfile_"+runNumber+".root";
//TString outputFolder = "/afs/cern.ch/work/e/ealvesco/CMSSW_7_5_0_pre5/src/HFcommissioning/Analysis/test/plotCapIDvsBXProfile_"+runNumber;
// system("mkdir -p "+outputNamePngfolder);
 system("mkdir -p "+outputFolder);

TFile* inputFile = new TFile( inputName , "READ" );
TFile* outputFile = new TFile( outputNameRootTest , "RECREATE");
cout << "LOADED" << endl;
const int numChans = 96 ;

TProfile* pulseProfile[numChans];
TString pulseNameTemp = "CapIDvsBX_";
TH2F* pulse2D[numChans];

TCanvas* can = new TCanvas("can","can",1366,768);  
//can->Divide(6,4);

for( int i = 1 ; i <= numChans ; i++ ){
	TString pulseName = "CapIDvsBX_" + TString::Itoa(i,10) ;

	pulse2D[i] = (TH2F*) inputFile->Get( pulseName );
   		
	if( pulse2D[i] == NULL ) continue;
    
   	pulseName = "CapIDvsBXProfile_" + TString::Itoa(i,10);
    	pulseProfile[i] = (TProfile*) pulse2D[i]->ProfileX( pulseName , 1 , -1 , "s" );

    	pulseProfile[i]->GetYaxis()->SetTitle("CapID");    
    	pulseProfile[i]->GetXaxis()->SetTitle("BX");
    	pulseProfile[i]->SetLineColor( i%4+1 );
    	pulseProfile[i]->SetMarkerColor(i%4+1 );
    	pulseProfile[i]->SetMarkerStyle( 7 );

	}

    
for( int i = 1 ; i <= numChans ; i++ ){

  //    	can->cd(i);
	pulseProfile[i]->SetMinimum(0);
	pulseProfile[i]->SetMaximum(pulseProfile[i]->GetMaximum()*1.2);
    	pulseProfile[i]->Draw();
   	TString fileName = outputNamePngfolder+"/CapIDvsBX_" + TString::Itoa(i,10) + ".png" ;
        //TString fileNametest = outputFolder+"/CapIDvsBX_" + TString::Itoa(i,10) + ".png" ;
 
  	can->SaveAs(fileName);
 }
// can->SaveAs(outputNameRoot);
 outputFile->Write();
 outputFile->Close();
}

