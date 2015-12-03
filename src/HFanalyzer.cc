// -*- C++ -*-
//
// Package:    HFanalyzer
// Class:      HFanalyzer
// 
/**\class HFanalyzer HFanalyzer.cc UserCode/HFanalyzer/src/HFanalyzer.cc

   Description: [one line class summary]

   Implementation:
   [Notes on implementation]
*/
//

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "EventFilter/HcalRawToDigi/interface/HcalHTRData.h"
#include "EventFilter/HcalRawToDigi/interface/HcalDCCHeader.h"
#include "EventFilter/HcalRawToDigi/interface/HcalUnpacker.h"
#include "DataFormats/HcalDetId/interface/HcalOtherDetId.h"
#include "DataFormats/HcalDigi/interface/HcalQIESample.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"
#include "DataFormats/HcalDetId/interface/HcalCalibDetId.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/FEDRawData/interface/FEDHeader.h"
#include "DataFormats/FEDRawData/interface/FEDTrailer.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"
#include "DataFormats/FEDRawData/interface/FEDRawData.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "CalibFormats/HcalObjects/interface/HcalDbService.h"
#include "CalibFormats/HcalObjects/interface/HcalDbRecord.h"
#include "CalibFormats/HcalObjects/interface/HcalCalibrations.h"
#include "CalibFormats/HcalObjects/interface/HcalCoderDb.h"

#include "HFcommissioning/Analysis/interface/ADC_Conversion.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TProfile.h"
#include "TFile.h"
#include "TSystem.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#define NUMCHS 120 
#define NUMTS 50
#define NUMCHSTS NUMCHS*NUMTS

#define NUMADCS 256

// NEEDS UPDATING
double adc2fC_QIE10[NUMADCS]={
  // - - - - - - - range 0 - - - - - - - -
  //subrange0 
  1.58, 4.73, 7.88, 11.0, 14.2, 17.3, 20.5, 23.6, 
  26.8, 29.9, 33.1, 36.2, 39.4, 42.5, 45.7, 48.8,
  //subrange1
  53.6, 60.1, 66.6, 73.0, 79.5, 86.0, 92.5, 98.9,
  105, 112, 118, 125, 131, 138, 144, 151,
  //subrange2
  157, 164, 170, 177, 186, 199, 212, 225,
  238, 251, 264, 277, 289, 302, 315, 328,
  //subrange3
  341, 354, 367, 380, 393, 406, 418, 431,
  444, 464, 490, 516, 542, 568, 594, 620,

  // - - - - - - - range 1 - - - - - - - -
  //subrange0
  569, 594, 619, 645, 670, 695, 720, 745,
  771, 796, 821, 846, 871, 897, 922, 947,
  //subrange1
  960, 1010, 1060, 1120, 1170, 1220, 1270, 1320,
  1370, 1430, 1480, 1530, 1580, 1630, 1690, 1740,
  //subrange2
  1790, 1840, 1890, 1940,  2020, 2120, 2230, 2330,
  2430, 2540, 2640, 2740, 2850, 2950, 3050, 3150,
  //subrange3
  3260, 3360, 3460, 3570, 3670, 3770, 3880, 3980,
  4080, 4240, 4450, 4650, 4860, 5070, 5280, 5490,
  
  // - - - - - - - range 2 - - - - - - - - 
  //subrange0
  5080, 5280, 5480, 5680, 5880, 6080, 6280, 6480,
  6680, 6890, 7090, 7290, 7490, 7690, 7890, 8090,
  //subrange1
  8400, 8810, 9220, 9630, 10000, 10400, 10900, 11300,
  11700, 12100, 12500, 12900, 13300, 13700, 14100, 14500,
  //subrange2
  15000, 15400, 15800, 16200, 16800, 17600, 18400, 19300,
  20100, 20900, 21700, 22500, 23400, 24200, 25000, 25800,
  //subrange3
  26600, 27500, 28300, 29100, 29900, 30700, 31600, 32400,
  33200, 34400, 36100, 37700, 39400, 41000, 42700, 44300,

  // - - - - - - - range 3 - - - - - - - - -
  //subrange0
  41100, 42700, 44300, 45900, 47600, 49200, 50800, 52500,
  54100, 55700, 57400, 59000, 60600, 62200, 63900, 65500,
  //subrange1
  68000, 71300, 74700, 78000, 81400, 84700, 88000, 91400,
  94700, 98100, 101000, 105000, 108000, 111000, 115000, 118000,
  //subrange2
  121000, 125000, 128000, 131000, 137000, 145000, 152000, 160000,
  168000, 176000, 183000, 191000, 199000, 206000, 214000, 222000,
  //subrange3
  230000, 237000, 245000, 253000, 261000, 268000, 276000, 284000,
  291000, 302000, 316000, 329000, 343000, 356000, 370000, 384000

};

struct TQIE10Info
{
  int numChs;
  int numTS;
  int iphi[NUMCHS];
  int ieta[NUMCHS];
  int depth[NUMCHS];
  double pulse[NUMCHS][NUMTS];
  double ped[NUMCHS];
  double pulse_adc[NUMCHS][NUMTS];
  double ped_adc[NUMCHS];
  bool link_error[NUMCHS];
  bool soi[NUMCHS][NUMTS];
};


//
// class declaration
//

class HFanalyzer : public edm::EDAnalyzer {
public:
  explicit HFanalyzer(const edm::ParameterSet&);
  ~HFanalyzer();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  virtual void beginJob() ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  void getData(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;

  TFile *_file;
  TTree *_treeQIE10;

  vector<TH1F*> ADCspectrum;		// 1D hist: ADC values
  vector<TH1F*> Qspectrum;      	// 1D hist: charge in fC
  vector<TH1F*> TDCspectrum;		// 1D hist: TDC leading edge
  vector<TH1F*> TDCtrailing;		// 1D hist: TDC trailing edge   
  vector<TH2F*> Pulse;			// 2D hist: charge (non converted) vs. time sample (BX)
  vector<TH2F*> Qpulse;			// 2D hist: charge vs. time sample (BX)
  vector<TH2F*> SOIplusBX;      	// 2D hist: charge of Sample of interest(SOI) vs. time sample (BX)
  vector<TH1F*> PulseEnergy1D;          // 1D hist: charge of SOI + charge of next BX 
  vector<TH2F*> TDCvsBX;                // 2D hist: TDC leading edge vs. BX
  vector<TH2F*> TDCtrailVsBX;           // 2D hist: TDC trailing edge vs. BX
  vector<TH2F*> PedVsCapID;   		// 2D hist: Pedestal vs. CapID
  vector<TH2F*> CapIDvsBX;
  //  vector<TProfile*> QProfile;           // Profile of "Qpulse" histograms


  int numChannels;
  string _outFileName;
  int _verbosity;

  TQIE10Info _qie10Info;

  virtual void beginRun(edm::Run const&, edm::EventSetup const&);
  virtual void endRun(edm::Run const&, edm::EventSetup const&);
  virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
  virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

  edm::EDGetTokenT<HcalDataFrameContainer<QIE10DataFrame> > tok_QIE10DigiCollection_;
  edm::Handle<QIE10DigiCollection> qie10DigiCollection;
  
  // ----------member data ---------------------------
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
HFanalyzer::HFanalyzer(const edm::ParameterSet& iConfig) :
  _outFileName(iConfig.getUntrackedParameter<string>("OutFileName")),
  _verbosity(iConfig.getUntrackedParameter<int>("Verbosity"))
{


  tok_QIE10DigiCollection_ = consumes<HcalDataFrameContainer<QIE10DataFrame> >(edm::InputTag("hcalDigis"));

  //now do what ever initialization is needed

  _file = new TFile(_outFileName.c_str(), "recreate");
  _file->mkdir("QIE10Data");

  _file->cd("QIE10Data");
  _treeQIE10 = new TTree("Events", "Events");
  _treeQIE10->Branch("numChs", &_qie10Info.numChs, "numChs/I");
  _treeQIE10->Branch("numTS", &_qie10Info.numTS, "numTS/I");
  _treeQIE10->Branch("iphi", _qie10Info.iphi, "iphi[numChs]/I");
  _treeQIE10->Branch("ieta", _qie10Info.ieta, "ieta[numChs]/I");
  _treeQIE10->Branch("depth", _qie10Info.depth, "depth[numChs]/I");
  _treeQIE10->Branch("pulse", _qie10Info.pulse, "pulse[numChs][50]/D");
  _treeQIE10->Branch("ped", _qie10Info.ped, "ped[numChs]/D");
  _treeQIE10->Branch("pulse_adc", _qie10Info.pulse_adc, "pulse_adc[numChs][50]/D");
  _treeQIE10->Branch("ped_adc", _qie10Info.ped_adc, "ped_adc[numChs]/D");
  _treeQIE10->Branch("link_error", _qie10Info.link_error, "link_error[numChs]/O");
  _treeQIE10->Branch("soi", _qie10Info.soi, "soi[numChs][50]/O");

  // for histo stuff
  numChannels=0;

}


HFanalyzer::~HFanalyzer()
{
  
  _file->cd();

  for( unsigned int j = 0 ; j < ADCspectrum.size() ; j++ ){
    ADCspectrum[j]->Write();
  }// end loop over ADCspectrum
  for( unsigned int j = 0 ; j < Qspectrum.size() ; j++ ){    
    Qspectrum[j]->Write();
  }// end loop over Qspectrum
  for( unsigned int j = 0 ; j < TDCspectrum.size() ; j++ ){    
    TDCspectrum[j]->Write();
  }// end loop over TDCspectrum
  for ( unsigned int j = 0 ; j < TDCtrailing.size() ; j++ ){
    TDCtrailing[j]->Write();
  }//end loop over TDCtrailing
  for( unsigned int j = 0 ; j < Pulse.size() ; j++ ){    
    Pulse[j]->Write();
  }// end loop over Pulse
  for( unsigned int j = 0 ; j < Qpulse.size() ; j++ ){    
    Qpulse[j]->Write();
  }// end loop over Qpulse
  for( unsigned int j = 0 ; j < SOIplusBX.size() ; j++ ){
    SOIplusBX[j]->Write();
  }// end loop over SOIplusBX  
  for( unsigned int j = 0 ; j < PulseEnergy1D.size() ; j++ ){
  PulseEnergy1D[j]->Write();
  }// end loop over PulseEnergy 
  for( unsigned int j = 0 ; j < TDCtrailVsBX.size() ; j++ ){
    TDCtrailVsBX[j]->Write();
  }// end loop over TDCtrailvsBX 
  for( unsigned int j = 0 ; j < TDCvsBX.size() ; j++ ){    
    TDCvsBX[j]->Write();
  }// end loop over TDCvsBX
  for( unsigned int j = 0 ; j < PedVsCapID.size() ; j++ ){    
    PedVsCapID[j]->Write();
  }// end loop over PedVsCapID
  for( unsigned int j = 0 ; j < CapIDvsBX.size() ; j++ ){
    CapIDvsBX[j]->Write();
  }
  //  for( unsigned int j = 0 ; j < QProfile.size() ; j++ ){
  //    QProfile[j]->Write();
  //  }// end loop over QProfile 
  
  _file->Write();
  _file->Close();

}
	
void HFanalyzer::getData(const edm::Event &iEvent, 
			 const edm::EventSetup &iSetup)
{
  using namespace edm;


  //
  //  Extracting All the Collections containing useful Info
  iEvent.getByToken(tok_QIE10DigiCollection_,qie10DigiCollection);
  const QIE10DigiCollection& qie10dc=*(qie10DigiCollection);
  //  -----------------------------------------------------

  if (_verbosity>0)
    {
      cout << "### Before Loop: " << endl;
      cout << "### QIE10 Digis=" << qie10DigiCollection->size() << endl;

    }
        
  // --------------------------
  // --   QIE10 Information  --
  // --------------------------
    
  char histoName[100];
  //  char QProfileName[100];

  if (_verbosity>0) std::cout << "Trying to access the qie collection" << std::endl;
    
  for (int j=0; j < qie10dc.size(); j++){

    if( ADCspectrum.size() <= (unsigned int)j ){
	sprintf(histoName,"ADCspectrum_%i",numChannels);
	numChannels++;
    	ADCspectrum.push_back(new TH1F(histoName,histoName,256,-0.5,255.5));      
   	
	sprintf(histoName,"Qspectrum_%i",numChannels);
      	Qspectrum.push_back(new TH1F(histoName,histoName,100000,0.,350000.));      

      	sprintf(histoName,"TDCspectrum_%i",numChannels);
      	TDCspectrum.push_back(new TH1F(histoName,histoName,64,-0.5,63.5));      

	sprintf(histoName,"TDCtrailing_%i",numChannels);
     	TDCtrailing.push_back(new TH1F(histoName,histoName,64,-0.5,100));

  	sprintf(histoName,"Qpulse_%i",numChannels);
	Qpulse.push_back(new TH2F(histoName,histoName,10,-0.5,9.5,100000,0.,350000.));      

      	sprintf(histoName,"Pulse_%i",numChannels);
      	Pulse.push_back(new TH2F(histoName,histoName,10,-0.5,9.5,256,-0.5,255.5));      

	sprintf(histoName,"SOIplusBX_%i",numChannels);
        SOIplusBX.push_back(new TH2F(histoName,histoName,10,-0.5,9.5,30,0.,100.));

        sprintf(histoName,"TDCtrailVsBX_%i",numChannels);
        TDCtrailVsBX.push_back(new TH2F(histoName,histoName,10,-0.5,9.5,64,-0.5,100));

        sprintf(histoName,"TDCvsBX_%i",numChannels);
      	TDCvsBX.push_back(new TH2F(histoName,histoName,10,-0.5,9.5,64,-0.5,63.5));      

      	sprintf(histoName,"PedVsCapID_%i",numChannels);
      	PedVsCapID.push_back(new TH2F(histoName,histoName,40,-0.5,3.5,30,0.0,90.0));      

	sprintf(histoName,"PulseEnergy1D_%i",numChannels);
        PulseEnergy1D.push_back(new TH1F(histoName,histoName,30,0.,100.));

	sprintf(histoName,"CapIDvsBX_%i",numChannels);
        CapIDvsBX.push_back(new TH2F(histoName,histoName,10,-0.5,9.5,40,-0.5,3.5));

	//        QProfile.push_back(new TProfile(QProfileName,QProfileName,10,-0.5,9.5,0,100));

    }

    if (_verbosity>0){
      std::cout << "Printing raw dataframe" << std::endl;
      std::cout << qie10dc[j] << std::endl;
            
      std::cout << "Printing content of samples() method" << std::endl;
      std::cout << qie10dc[j].samples() << std::endl;
    }
        
    // Extract info on detector location
    DetId detid = qie10dc[j].detid();
    HcalDetId hcaldetid = HcalDetId(detid);
    int ieta = hcaldetid.ieta();
    int iphi = hcaldetid.iphi();
    int depth = hcaldetid.depth();
        
    if (_verbosity>0){
      std::cout << " detid: " << detid.rawId() << std::endl;
      std::cout << " ieta: " << ieta << "\n"
		<< " iphi: " << iphi << "\n"
		<< " depth: " << depth << std::endl;
    }
        
    // loop over the samples in the digi
    int nTS = qie10dc[j].samples();

    float ped_adc = 0;
    float ped_fc = 0;

    for(int i=0; i<nTS; ++i)
      {

	// j - QIE channel
	// i - time sample (TS)
	int adc = qie10dc[j][i].adc();
	int tdc = qie10dc[j][i].le_tdc();
	int trail = qie10dc[j][i].te_tdc();
	int capid = qie10dc[j][i].capid();
	int soi = qie10dc[j][i].soi();

	// store pulse information
	float charge = adc2fC_QIE10[ adc ];

	//-----------------------------------------------
	//-- Compute the charge of SOI and the next BX --
	//----------------------------------------------
        if (_qie10Info.soi[j][i] != 0){
		float PulseEnergy = qie10dc[j][i].adc() + qie10dc[j][i+1].adc();
		SOIplusBX[j]->Fill( i , PulseEnergy);
                PulseEnergy1D[j]->Fill(PulseEnergy);
		}
	//Fill the histograms
	ADCspectrum[j]->Fill( adc );
	Qspectrum[j]->Fill( charge );
	TDCspectrum[j]->Fill( tdc );
        TDCtrailing[j]->Fill( trail );
	Qpulse[j]->Fill( i , charge );
	Pulse[j]->Fill( i , adc );
        TDCtrailVsBX[j]->Fill( i , trail );
	TDCvsBX[j]->Fill( i , tdc );
	PedVsCapID[j]->Fill( capid , charge );
	CapIDvsBX[j]->Fill ( i , capid );
	//-------------------------------------------------
	//--Create a profile for each "Qpulse" histogram --
	//-------------------------------------------------
	//	sprintf(QProfileName,"QProfile_%i)",j+1);
	//        QProfile[j] = (TProfile*) Qpulse[j]->ProfileX( QProfileName, 1 , -1 , "s" );
	//        QProfile[j]->GetYaxis()->SetTitle("Charge [fC]");
	//        QProfile[j]->GetXaxis()->SetTitle("BX");
	//        QProfile[j]->SetLineColor( i%4+1 );
	//        QProfile[j]->SetMarkerColor( i%4+1 );
	//        QProfile[j]->SetMarkerStyle( 8 );
	//--------------------------------------------------

	_qie10Info.pulse[j][i] = charge;
	_qie10Info.pulse_adc[j][i] = adc;
	_qie10Info.soi[j][i] = soi;

	if (_verbosity>0)
	  std::cout << "Sample " << i << ": ADC=" << adc << " Charge=" << charge << "fC" << " TDC=" << tdc << " Capid=" << capid
		    << " SOI=" << soi << std::endl;

	//-------------------------------------------
	//-- Compute ped from first 3 time samples --
	//-------------------------------------------
	if (i<3){
	  ped_adc += adc;
	  ped_fc += charge;
	}
            
      }
    ped_adc = ped_adc/3.;
    ped_fc = ped_fc/3.; 

    if (_verbosity>0)
      std::cout << "The pedestal for this channel is " << ped_adc << "ADC counts and " << ped_fc << " fC" << std::endl;
  
    // -------------------------------------
    // --    Set the Branched arrays      --
    // -------------------------------------
    _qie10Info.iphi[j] = iphi;
    _qie10Info.ieta[j] = ieta;
    _qie10Info.depth[j] = depth;
    _qie10Info.ped[j] = ped_fc;
    _qie10Info.ped_adc[j] = ped_adc;
    _qie10Info.link_error[j] = qie10dc[j].linkError();
  }

  _qie10Info.numChs = qie10dc.size();
  _qie10Info.numTS = qie10dc.samples();

  _treeQIE10->Fill();


  return;
}


//
// member functions
//

// ------------ method called for each event  ------------
void
HFanalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  getData(iEvent, iSetup);

#ifdef THIS_IS_AN_EVENT_EXAMPLE
  Handle<ExampleData> pIn;
  iEvent.getByLabel("example",pIn);
#endif

#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
  ESHandle<SetupData> pSetup;
  iSetup.get<SetupRecord>().get(pSetup);
#endif
}


// ------------ method called once each job just before starting event loop  ------------
void 
HFanalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
HFanalyzer::endJob() 
{
  //      _file->Write();
  //      _file->Close();
}

// ------------ method called when starting to processes a run  ------------
void 
HFanalyzer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
HFanalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{
  //      _file->Write();
  //      _file->Close();
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
HFanalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
HFanalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
HFanalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(HFanalyzer);
