#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
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
#include "FWCore/Utilities/interface/CRC16.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "CalibFormats/HcalObjects/interface/HcalDbService.h"
#include "CalibFormats/HcalObjects/interface/HcalDbRecord.h"
#include "CalibFormats/HcalObjects/interface/HcalCalibrations.h"
#include "CalibFormats/HcalObjects/interface/HcalCoderDb.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

class digi2rawTester : public edm::EDProducer {
public:
  explicit digi2rawTester(const edm::ParameterSet&);
  ~digi2rawTester();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  virtual void beginJob() ;
  virtual void produce(edm::Event&, const edm::EventSetup&);
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

  std::cout << "digi2rawTester::digi2rawTester" << std::endl;

  produces<FEDRawDataCollection>("");
  tok_QIE10DigiCollection_ = consumes<HcalDataFrameContainer<QIE10DataFrame> >(edm::InputTag("hcalDigis"));

}


digi2rawTester::~digi2rawTester(){ std::cout << "digi2rawTester::~digi2rawTester" << std::endl;}

void digi2rawTester::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){
  using namespace edm;

  //collection to be inserted into event
  std::auto_ptr<FEDRawDataCollection> fed_buffers(new FEDRawDataCollection());

  //
  //  Extracting All the Collections containing useful Info
  iEvent.getByToken(tok_QIE10DigiCollection_,qie10DigiCollection);
  const QIE10DigiCollection& qie10dc=*(qie10DigiCollection);

  int FEDIDmin = FEDNumbering::MINHCALuTCAFEDID;
  int FEDIDmax = FEDNumbering::MAXHCALuTCAFEDID;

  typedef vector<uint16_t> uhtrData;
  typedef pair<int,int>    UHTRLoc;
  typedef map<UHTRLoc,uhtrData> UHTRMap;
  typedef map<int,FEDRawData> FEDMap;
  UHTRMap uhtrMap;
  UHTRLoc uhtrLoc;
  FEDMap fedMap;

  for (unsigned int j=0; j < qie10dc.size(); j++){

    std::cout << "j: " << j << std::endl;

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
  
  // what about dual FED readout?
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
	std::cout << "raw from uhtr: " << std::hex << m->second[i] << std::endl;	
      }
    }

    int fedId = FEDIDmin + m->first.first;
    //initialize FEDRawData object
    fedMap[fedId]=FEDRawData(m->second.size()*2); 

    int fedWord;
    for( unsigned int iWord = 0 ; iWord < m->second.size() ; iWord++){

      //fed data is unsigned char (8-bits)
      //uhtr data is 16-bit ints
      fedWord = iWord*2 ;
      if( _verbosity>5 )
	std::cout << "fed word: " << std::dec << fedWord << std::endl;
      //save the first 8 bits of uhtr data into fedWord
      fedMap[fedId].data()[fedWord] = m->second[iWord]&0xFF;
      //offset fed word and save last 8 bits of uhtr data
      fedMap[fedId].data()[fedWord+1] = (m->second[iWord]>>8)&0xFF;
    }

  }// end loop over uhtr containers

  for( FEDMap::iterator fed = fedMap.begin() ; fed != fedMap.end() ; ++fed ){

    // need to fix for cases where there are two FEDs per crate...
    int fedId = fed->first;

    if( _verbosity>5 )
      std::cout << "FED ID: " << std::dec << fedId << std::endl;
 
    if( fedId < FEDIDmin || fedId > FEDIDmax ){
      std::cout << "FED ID out of range" << std::endl;
    }

    FEDRawData& fedRawData = fed_buffers->FEDData(fedId);
    fedRawData = fed->second;
    
    if( _verbosity>0 )
      std::cout << "building FED header" << std::endl;
    FEDHeader hcalFEDHeader(fedRawData.data());
    hcalFEDHeader.set(fedRawData.data(), 0, iEvent.id().event(), 0, fedId);
    if( _verbosity>0 )
      std::cout << "building FED trailer" << std::endl;
    FEDTrailer hcalFEDTrailer(fedRawData.data()+(fedRawData.size()-8));
    hcalFEDTrailer.set(fedRawData.data()+(fedRawData.size()-8), fedRawData.size()/8, evf::compute_crc(fedRawData.data(),fedRawData.size()), 0, 0);

  }// end loop over FEDs with data

  if( _verbosity > 0 ) 
    std::cout << "putting FEDRawDataCollection in event" << std::endl;

  iEvent.put(fed_buffers);
  
  std::cout << "check mate" << std::endl;

}


// ------------ method called once each job just before starting event loop  ------------
void digi2rawTester::beginJob(){}

// ------------ method called once each job just after ending the event loop  ------------
void digi2rawTester::endJob(){

  std::cout << "digi2rawTester::endJob" << std::endl;

}

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
