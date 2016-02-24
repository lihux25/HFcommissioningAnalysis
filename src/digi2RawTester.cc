#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "EventFilter/HcalRawToDigi/interface/HcalUHTRData.h"
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
#include <string>

using namespace std;

class digi2rawTester : public edm::EDAnalyzer {
public:
  explicit digi2rawTester(const edm::ParameterSet&);
  ~digi2rawTester();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  virtual void beginJob() ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  void getData(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;

  int _verbosity;

  virtual void beginRun(edm::Run const&, edm::EventSetup const&);
  virtual void endRun(edm::Run const&, edm::EventSetup const&);
  virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
  virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

  edm::EDGetTokenT<HcalDataFrameContainer<QIE10DataFrame> > tok_QIE10DigiCollection_;
  edm::Handle<QIE10DigiCollection> qie10DigiCollection;
  
};

digi2rawTester::digi2rawTester(const edm::ParameterSet& iConfig) :
  _verbosity(iConfig.getUntrackedParameter<int>("Verbosity"))
{

  tok_QIE10DigiCollection_ = consumes<HcalDataFrameContainer<QIE10DataFrame> >(edm::InputTag("hcalDigis"));

}


digi2rawTester::~digi2rawTester(){}

void digi2rawTester::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){
  using namespace edm;

  //
  //  Extracting All the Collections containing useful Info
  iEvent.getByToken(tok_QIE10DigiCollection_,qie10DigiCollection);
  const QIE10DigiCollection& qie10dc=*(qie10DigiCollection);

  typedef vector<uint16_t> uhtrData;
  typedef pair<int,int>    UHTRLoc;
  typedef map<UHTRLoc,uhtrData> UHTRMap;

  UHTRMap uhtrMap;
  UHTRLoc uhtrLoc;

  for (unsigned int j=0; j < qie10dc.size(); j++){

    QIE10DataFrame qie10df = static_cast<QIE10DataFrame>(qie10dc[j]);
  
    // Extract info on detector location
    DetId detid = qie10df.detid();
    HcalElectronicsId eid(detid.rawId());
    int uhtrSlot = eid.slot();
    int uhtrCrate = eid.crateId();
    uhtrLoc.first = uhtrCrate;
    uhtrLoc.second = uhtrSlot;
    if( uhtrMap.find(uhtrLoc) == uhtrMap.end() ){ 
      // uhtr not found
      // initialize vector of 16-bit words
      uhtrMap[uhtrLoc] = uhtrData(8);
      
      // build header -- some information will be updated at the end
      uhtrMap[uhtrLoc][4] = (4<<12)|(uhtrSlot<<8)|(uhtrCrate); // n-presampels hardcoded for testing purposes
      uhtrMap[uhtrLoc][5] = 1041;// hardcoded for testing purposes
      uhtrMap[uhtrLoc][6] = 0560;// hardcoded for testing purposes
      
      

    }

    for( int i = 0 ; i < qie10df.samples()&&_verbosity>0 ; i++){

      int adc = qie10df[i].adc();
      int tdc = qie10df[i].le_tdc();
      int trail = qie10df[i].te_tdc();
      int capid = qie10df[i].capid();
      int soi = qie10df[i].soi();

      if (_verbosity>0){
	std::cout 
	  << "Sample " << i 
	  << ": ADC=" << adc 
	  << " TDC=" << tdc 
	  << " Trail=" << trail 
	  << " Capid=" << capid
	  << " SOI=" << soi 
	  << std::endl;
      }
    }

    // loop over words in dataframe (2 per sample)
    for(edm::DataFrame::iterator dfi=qie10df.begin() ; dfi!=qie10df.end(); ++dfi){
      
      if (_verbosity>0){
	std::cout << "raw from digi: " << std::hex << dfi[1] << endl; 
      }
      // push data into uhtr data container
      uhtrMap[uhtrLoc].push_back(dfi[1]);

    }// end loop over dataframe words

  }//end loop over digis

  for( UHTRMap::iterator m = uhtrMap.begin() ; m != uhtrMap.end() ; ++m){
    
    // add trailer to uhtrData
    uint64_t uhtr_size = m->second.size()+4;
    m->second.push_back( uhtr_size & 0xFF );
    m->second.push_back( (uhtr_size>>16) & 0xF );
    m->second.push_back( 0 );
    m->second.push_back( 0 );
    
    // set size in header
    m->second[0] = uhtr_size & 0xFF ;
    m->second[1] = m->second[1] |= (uhtr_size>>16) & 0xF ; 

    if( _verbosity>0 ){
      for( unsigned int i = 0 ; i < m->second.size() ; i++ ){
	cout << "raw from uhtr: " << std::hex << m->second[i] << endl;
	
      }
    }

    //initialize HcalUHTRData object
    //HcalUHTRData uhtr(uhtrData[0],static_cast<int>(uhtr_size));

  }// end loop over uhtr containers
  
}


// ------------ method called once each job just before starting event loop  ------------
void digi2rawTester::beginJob(){}

// ------------ method called once each job just after ending the event loop  ------------
void digi2rawTester::endJob(){}

// ------------ method called when starting to processes a run  ------------
void digi2rawTester::beginRun(edm::Run const&, edm::EventSetup const&){}

// ------------ method called when ending the processing of a run  ------------
void digi2rawTester::endRun(edm::Run const&, edm::EventSetup const&){}

// ------------ method called when starting to processes a luminosity block  ------------
void digi2rawTester::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&){}

// ------------ method called when ending the processing of a luminosity block  ------------
void digi2rawTester::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&){}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void digi2rawTester::fillDescriptions(edm::ConfigurationDescriptions& descriptions){
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(digi2rawTester);
