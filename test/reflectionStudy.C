
double reflection(TFile* file , int chan=45 , int BXofInterest = 4){

  char temp[100];
  sprintf(temp,"Qpulse_%i",chan);

  TH2F* histo = (TH2F*) file->Get(temp);
  TProfile* histoPro = histo->ProfileX("histoPro",1,-1,"s");

  int max = 0;

  for( int i = 1 ; i <= 10 ; i++ ){
    if( histoPro->GetBinContent(i) > histoPro->GetBinContent(max) ) max = i;
  }

  double ped = 0.0;
  double pedCount = 0.;
  for( int i = 1 ; i < max ; i++ ){
    ped += histoPro->GetBinContent(i);
    pedCount += 1.;
  }
  ped = ped/pedCount;
    
  return (histoPro->GetBinContent(max+BXofInterest)-ped)/(histoPro->GetBinContent(max)-ped);

}

void reflectionStudy(){

  gROOT->ProcessLine(".L ~/tdrstyle.C");
  gROOT->ProcessLine("setTDRStyle()");
  
  const int nPoints = 15;

  double phase[nPoints] = {0.,15.,30.,45.,60.,75.,90.,105.,120.,150.,165.,180.,195.,210.,225.};
  int runNumber[nPoints] = {299,301,302,305,307,308,309,310,313,316,317,320,321,322,323};
  double ref44[nPoints];
  double ref45[nPoints];
  double ref46[nPoints];
  double ref47[nPoints];
  double ref48[nPoints];

  double ref44p[nPoints];
  double ref45p[nPoints];
  double ref46p[nPoints];
  double ref47p[nPoints];
  double ref48p[nPoints];

  char temp[100];

  for( int i = 0 ; i < nPoints ; i++ ){
    
    sprintf(temp,"HFanalysisTree_000%i.root",runNumber[i]);
    TFile* file = new TFile(temp,"READ");
    ref44[i] = reflection(file,44);
    ref45[i] = reflection(file,45);
    ref46[i] = reflection(file,46);
    ref47[i] = reflection(file,47);
    ref48[i] = reflection(file,48);
    ref44p[i] = reflection(file,44,5);
    ref45p[i] = reflection(file,45,5);
    ref46p[i] = reflection(file,46,5);
    ref47p[i] = reflection(file,47,5);
    ref48p[i] = reflection(file,48,5);

  }

  TGraph* gr44 = new TGraph(nPoints,phase,ref44);
  gr44->SetMarkerStyle(20);
  gr44->SetMarkerColor(1);
  gr44->GetXaxis()->SetNdivisions(505);
  gr44->GetXaxis()->SetTitle("Clock phase (a.u.)");
  gr44->GetYaxis()->SetTitle("#alpha_{j}");
  gr44->GetYaxis()->SetRangeUser(-.3,.3);

  TGraph* gr45 = new TGraph(nPoints,phase,ref45);
  gr45->SetMarkerStyle(20);
  gr45->SetMarkerColor(2);
  TGraph* gr46 = new TGraph(nPoints,phase,ref46);
  gr46->SetMarkerStyle(20);
  gr46->SetMarkerColor(3);
  TGraph* gr47 = new TGraph(nPoints,phase,ref47);
  gr47->SetMarkerStyle(20);
  gr47->SetMarkerColor(4);
  TGraph* gr48 = new TGraph(nPoints,phase,ref48);
  gr48->SetMarkerStyle(20);
  gr48->SetMarkerColor(6);

  TGraph* gr44p = new TGraph(nPoints,phase,ref44p);
  gr44p->SetMarkerStyle(24);
  gr44p->SetMarkerColor(1);
  TGraph* gr45p = new TGraph(nPoints,phase,ref45p);
  gr45p->SetMarkerStyle(24);
  gr45p->SetMarkerColor(2);
  TGraph* gr46p = new TGraph(nPoints,phase,ref46p);
  gr46p->SetMarkerStyle(24);
  gr46p->SetMarkerColor(3);
  TGraph* gr47p = new TGraph(nPoints,phase,ref47p);
  gr47p->SetMarkerStyle(24);
  gr47p->SetMarkerColor(4);
  TGraph* gr48p = new TGraph(nPoints,phase,ref48p);
  gr48p->SetMarkerStyle(24);
  gr48p->SetMarkerColor(6);

  gr44->Draw("Ap");
  gr45->Draw("p,SAME");
  gr46->Draw("p,SAME");
  gr47->Draw("p,SAME");
  gr48->Draw("p,SAME");
  gr44p->Draw("p,SAME");
  gr45p->Draw("p,SAME");
  gr46p->Draw("p,SAME");
  gr47p->Draw("p,SAME");
  gr48p->Draw("p,SAME");

  TLegend* leg = new TLegend(0.2,.7,.5,.9);
  leg->SetFillColor(0);
  leg->SetBorderSize(0);

  leg->AddEntry(gr44,"R_{ext}=30#Omega","p");
  leg->AddEntry(gr48,"R_{ext}=19#Omega","p");
  leg->AddEntry(gr47,"R_{ext}=16#Omega","p");
  leg->AddEntry(gr45,"R_{ext}=12#Omega","p");
  leg->AddEntry(gr46,"R_{ext}=0#Omega","p");

  leg->Draw();

  TLegend* leg2 = new TLegend(0.5,.8,.7,.9);
  leg2->SetFillColor(0);
  leg2->SetBorderSize(0);

  leg2->AddEntry(gr44,"TS4/SOI","p");
  leg2->AddEntry(gr44p,"TS5/SOI","p");

  leg2->Draw();
  
}
