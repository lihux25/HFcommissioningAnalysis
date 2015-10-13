const int nPoints = 15;

double phase[nPoints] = {0,15,30,45,60,75,90,105,120,150,165,180,195,210,225};
int runNumber[nPoints] = {299,301,302,305,307,308,309,310,313,316,317,320,321,322,323};

void makePlot( int phaseIndex = 0 ){

  gROOT->ProcessLine(".L ~/tdrstyle.C");
  gROOT->ProcessLine("setTDRStyle()");

  TH2F* histo[5];
  TProfile* profile[5];
  TCanvas* can = new TCanvas("can","can",500,500);

  char temp[100];

  sprintf(temp,"HFanalysisTree_000%i.root",runNumber[phaseIndex]);
  
  TFile* file = new TFile(temp,"READ");
  
  histo[0] = (TH2F*) file->Get("Qpulse_44");
  sprintf(temp,"profile_44_run%i",runNumber[phaseIndex]);
  profile[0] = histo[0]->ProfileX(temp,1,-1,"s");
  profile[0]->SetMarkerStyle(24);
  profile[0]->SetMarkerColor(1);
  profile[0]->GetXaxis()->SetTitle("BX");
  profile[0]->GetYaxis()->SetTitle("average charge [fC]");
  
  histo[1] = (TH2F*) file->Get("Qpulse_45");
  sprintf(temp,"profile_45_run%i",runNumber[phaseIndex]);
  profile[1] = histo[1]->ProfileX(temp,1,-1,"s");
  profile[1]->SetMarkerStyle(25);
  profile[1]->SetMarkerColor(2);
  profile[1]->GetXaxis()->SetTitle("BX");
  profile[1]->GetYaxis()->SetTitle("average charge [fC]");
  
  histo[2] = (TH2F*) file->Get("Qpulse_46");
  sprintf(temp,"profile_46_run%i",runNumber[phaseIndex]);
  profile[2] = histo[2]->ProfileX(temp,1,-1,"s");
  profile[2]->SetMarkerStyle(26);
  profile[2]->SetMarkerColor(3);
  
  histo[3] = (TH2F*) file->Get("Qpulse_47");
  sprintf(temp,"profile_47_run%i",runNumber[phaseIndex]);
  profile[3] = histo[3]->ProfileX(temp,1,-1,"s");
  profile[3]->SetMarkerStyle(27);
  profile[3]->SetMarkerColor(4);
  
  histo[4] = (TH2F*) file->Get("Qpulse_48");
  sprintf(temp,"profile_48_run%i",runNumber[phaseIndex]);
  profile[4] = histo[4]->ProfileX(temp,1,-1,"s");
  profile[4]->SetMarkerStyle(28);
  profile[4]->SetMarkerColor(6);
  
  can->cd(1);
  profile[1]->Draw("");
  profile[2]->Draw("SAME");
  profile[0]->Draw("SAME");
  profile[3]->Draw("SAME");
  profile[4]->Draw("SAME");

  TLegend* leg = new TLegend(0.2,0.65,0.4,0.9);
  leg->SetBorderSize(0);
  leg->SetFillColor(0);
  leg->AddEntry(profile[0],"R_{ext}=30#Omega","p");
  leg->AddEntry(profile[4],"R_{ext}=19#Omega","p");
  leg->AddEntry(profile[3],"R_{ext}=16#Omega","p");
  leg->AddEntry(profile[1],"R_{ext}=12#Omega","p");
  leg->AddEntry(profile[2],"R_{ext}=0#Omega","p");
  leg->Draw();

  sprintf(temp,"phaseScan_forSinglePhase_phase%i.png",int(phase[phaseIndex]));
  can->SaveAs(temp);

  profile[1]->GetYaxis()->SetRangeUser(0,1000);
  
  sprintf(temp,"phaseScan_forSinglePhase_phase%i_zoom.png",int(phase[phaseIndex]));
  can->SaveAs(temp);

  profile[1]->GetYaxis()->SetRangeUser(0,200);
  
  sprintf(temp,"phaseScan_forSinglePhase_phase%i_zoom2.png",int(phase[phaseIndex]));
  can->SaveAs(temp);
  
}

void phaseScan_forSinglePhase(){

  for( int i = 0 ; i < nPoints ; i++ ){

    makePlot(i);

  }

}
