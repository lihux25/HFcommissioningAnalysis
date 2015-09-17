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
// Original Authors:  Jay Dittmann, Nadja Strobbe, Joe Pastika
// Based on work by:  Viktor Khristenko,510 1-004,+41227672815,
//         Created:   Tue Sep 16 15:47:09 CEST 2014
// $Id$
//
//


// system include files
#include <memory>

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

#include "TBDataFormats/HcalTBObjects/interface/HcalTBTriggerData.h"
#include "TBDataFormats/HcalTBObjects/interface/HcalTBBeamCounters.h"
#include "TBDataFormats/HcalTBObjects/interface/HcalTBEventPosition.h"
#include "TBDataFormats/HcalTBObjects/interface/HcalTBParticleId.h"
#include "TBDataFormats/HcalTBObjects/interface/HcalTBTiming.h"

#include "RecoTBCalo/HcalTBObjectUnpacker/interface/HcalTBTriggerDataUnpacker.h"
#include "RecoTBCalo/HcalTBObjectUnpacker/interface/HcalTBSlowDataUnpacker.h"

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

#define NUMADCS 128

// NEEDS UPDATING
double adc2fC_QIE10[NUMADCS]={
    -0.5,0.5,1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5,9.5, 10.5,11.5,12.5,
    13.5,15.,17.,19.,21.,23.,25.,27.,29.5,32.5,35.5,38.5,42.,46.,50.,54.5,59.5,
    64.5,59.5,64.5,69.5,74.5,79.5,84.5,89.5,94.5,99.5,104.5,109.5,114.5,119.5,
    124.5,129.5,137.,147.,157.,167.,177.,187.,197.,209.5,224.5,239.5,254.5,272.,
    292.,312.,334.5,359.5,384.5,359.5,384.5,409.5,434.5,459.5,484.5,509.5,534.5,
    559.5,584.5,609.5,634.5,659.5,684.5,709.5,747.,797.,847.,897.,947.,997.,
    1047.,1109.5,1184.5,1259.5,1334.5,1422.,1522.,1622.,1734.5,1859.5,1984.5,
    1859.5,1984.5,2109.5,2234.5,2359.5,2484.5,2609.5,2734.5,2859.5,2984.5,
    3109.5,3234.5,3359.5,3484.5,3609.5,3797.,4047.,4297.,4547.,4797.,5047.,
    5297.,5609.5,5984.5,6359.5,6734.5,7172.,7672.,8172.,8734.5,9359.5,9984.5};

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

        string _outFileName;
        int _verbosity;

        TQIE10Info _qie10Info;


      virtual void beginRun(edm::Run const&, edm::EventSetup const&);
      virtual void endRun(edm::Run const&, edm::EventSetup const&);
      virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
      virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

      edm::EDGetTokenT<HcalDataFrameContainer<QIE10DataFrame> > tok_QIE10DigiCollection_;

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

}


HFanalyzer::~HFanalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
    _file->Write();
    _file->Close();
}

void HFanalyzer::getData(const edm::Event &iEvent, 
                const edm::EventSetup &iSetup)
{
    using namespace edm;

    //
    //  Extracting All the Collections containing useful Info
    //
    edm::Handle<QIE10DigiCollection> qie10DigiCollection;

    iEvent.getByToken(tok_QIE10DigiCollection_,qie10DigiCollection);
    
    if (_verbosity>0)
    {
      cout << "### Before Loop: " << endl;
      cout << "### QIE10 Digis=" << qie10DigiCollection->size() << endl;

    }
        

    // --------------------------
    // --   QIE10 Information  --
    // --------------------------
    
    
    if (_verbosity>0) std::cout << "Trying to access the qie collection" << std::endl;
    
    const QIE10DigiCollection& qie10dc=*(qie10DigiCollection);

    for (int j=0; j < qie10dc.size(); j++){
        
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
            std::cout << "Where am I?\n detid: " << detid.rawId() << std::endl;
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
            int adc = qie10dc[j][i].adc();
            int tdc = qie10dc[j][i].le_tdc();
            int capid = qie10dc[j][i].capid();
            int soi = qie10dc[j][i].soi();
            
            // store pulse information
            Converter Convertadc2fC_QIE10;
            float charge = Convertadc2fC_QIE10.linearize(adc);
            _qie10Info.pulse[j][i] = charge;
            _qie10Info.pulse_adc[j][i] = adc;
            _qie10Info.soi[j][i] = soi;

            if (_verbosity>0)
                std::cout << "Sample " << i << ": ADC=" << adc << " Charge=" << charge << "fC" << " TDC=" << tdc << " Capid=" << capid
                          << " SOI=" << soi << std::endl;

            // compute ped from first 3 time samples
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
