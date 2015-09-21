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

int main( int argc, const char* argv[] ){

  gROOT->ProcessLine(".L ~/tdrstyle.C");
  gROOT->ProcessLine("setTDRStyle()");
  
  TFile* inputFile = new TFile( "HFanalysisTree_000204.root" , "READ" );

  const int numChans = 48 ;

  TProfile* pulseProfile[numChans];
  TString pulseNameTemp = "Qpulse_";
  TH2F* pulse2D[numChans];

  TCanvas* can = new TCanvas("can","can",500,500);  

  for( int i = 1 ; i <= numChans ; i++ ){

    TString pulseName = "Qpulse_" + TString::Itoa(i,10) ;

    pulse2D[i] = (TH2F*) inputFile->Get( pulseName );
    if( pulse2D[i] == NULL ) continue;
    
    pulseName = "QpulseProfile_" + TString::Itoa(i,10);
    pulseProfile[i] = (TProfile*) pulse2D[i]->ProfileX( pulseName , 1 , -1 , "s" );

    pulseProfile[i]->GetYaxis()->SetRangeUser(1,350000);
    pulseProfile[i]->GetYaxis()->SetTitle("Charge [fC]");    
    pulseProfile[i]->GetXaxis()->SetTitle("BX");
    pulseProfile[i]->SetLineColor( i%4+1 );
    pulseProfile[i]->SetMarkerColor( i%4+1 );
    pulseProfile[i]->SetMarkerStyle( 8 );

  }

  can->SetLogy();

  for( int i = 1 ; i <= numChans ; i++ ){

    if( i % 4 == 1 )
      pulseProfile[i]->Draw();
    else 
      pulseProfile[i]->Draw("SAME");

    if( i % 4 == 0 ){
      TString fileName = "pulseShape_" + TString::Itoa(i/4,10) + ".png" ;
      can->SaveAs(fileName);
    }

  }

  return 0;

}
