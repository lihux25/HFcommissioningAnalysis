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

  TProfile* tdcProfile[numChans];
  TH2F* tdc2D[numChans];

  TCanvas* can = new TCanvas("can","can",500,500);  

  for( int i = 1 ; i <= numChans ; i++ ){

    TString tdcName = "TDCvsBX_" + TString::Itoa(i,10) ;

    tdc2D[i] = (TH2F*) inputFile->Get( tdcName );
    if( tdc2D[i] == NULL ) continue;
    
    tdcName = "TDCprofile_" + TString::Itoa(i,10);
    tdcProfile[i] = (TProfile*) tdc2D[i]->ProfileX( tdcName , 1 , -1 , "s" );

    tdcProfile[i]->GetYaxis()->SetRangeUser(0,64);
    tdcProfile[i]->GetYaxis()->SetTitle("TDC");    
    tdcProfile[i]->GetXaxis()->SetTitle("BX");
    tdcProfile[i]->SetLineColor( i%4+1 );
    tdcProfile[i]->SetMarkerColor( i%4+1 );
    tdcProfile[i]->SetMarkerStyle( 8 );

  }

  for( int i = 1 ; i <= numChans ; i++ ){

    if( i % 4 == 1 )
      tdcProfile[i]->Draw();
    else 
      tdcProfile[i]->Draw("SAME");

    if( i % 4 == 0 ){
      TString fileName = "TDCprofile_" + TString::Itoa(i/4,10) + ".png" ;
      can->SaveAs(fileName);
    }

  }

  return 0;

}
