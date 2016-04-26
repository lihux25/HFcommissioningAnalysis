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

#include "CondFormats/HcalObjects/interface/HcalElectronicsMap.h"

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

#include "PackerHelp.cc"

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
  std::string electronicsMapLabel_;

  virtual void beginRun(edm::Run const&, edm::EventSetup const&);
  virtual void endRun(edm::Run const&, edm::EventSetup const&);
  virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
  virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

  edm::EDGetTokenT<HcalDataFrameContainer<QIE10DataFrame> > tok_QIE10DigiCollection_;
  edm::Handle<QIE10DigiCollection> qie10DigiCollection;
  edm::EDGetTokenT<HcalDataFrameContainer<QIE11DataFrame> > tok_QIE11DigiCollection_;
  edm::Handle<QIE11DigiCollection> qie11DigiCollection;

  /*
  edm::EDGetTokenT<HBHEDigiCollection> tok_HBHEDigiCollection_;
  edm::Handle<HBHEDigiCollection> hbheDigiCollection;
  edm::EDGetTokenT<HFDigiCollection> tok_HFDigiCollection_;
  edm::Handle<HFDigiCollection> hfDigiCollection;
  edm::EDGetTokenT<HODigiCollection> tok_HODigiCollection_;
  edm::Handle<HODigiCollection> hoDigiCollection;
  */
};

digi2rawTester::digi2rawTester(const edm::ParameterSet& iConfig) :
  _verbosity(iConfig.getUntrackedParameter<int>("Verbosity")),
  electronicsMapLabel_(iConfig.getUntrackedParameter<std::string>("ElectronicsMap",""))
{
  produces<FEDRawDataCollection>("");
  tok_QIE10DigiCollection_ = consumes<HcalDataFrameContainer<QIE10DataFrame> >(edm::InputTag("hcalDigis"));
  tok_QIE11DigiCollection_ = consumes<HcalDataFrameContainer<QIE11DataFrame> >(edm::InputTag("hcalDigis"));
  /*
  tok_HBHEDigiCollection_ = consumes<HBHEDigiCollection >(edm::InputTag("hcalDigis"));
  tok_HFDigiCollection_ = consumes<HFDigiCollection >(edm::InputTag("hcalDigis"));
  tok_HODigiCollection_ = consumes<HODigiCollection >(edm::InputTag("hcalDigis"));
  */
}

digi2rawTester::~digi2rawTester(){}

void digi2rawTester::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){
  using namespace edm;

  edm::ESHandle<HcalDbService> pSetup;
  iSetup.get<HcalDbRecord>().get( pSetup );
  edm::ESHandle<HcalElectronicsMap> item;
  iSetup.get<HcalElectronicsMapRcd>().get(electronicsMapLabel_,item);
  const HcalElectronicsMap* readoutMap = item.product();

  //collection to be inserted into event
  std::auto_ptr<FEDRawDataCollection> fed_buffers(new FEDRawDataCollection());
  
  //
  //  Extracting All the Collections containing useful Info
  iEvent.getByToken(tok_QIE10DigiCollection_,qie10DigiCollection);
  const QIE10DigiCollection& qie10dc=*(qie10DigiCollection);
  iEvent.getByToken(tok_QIE11DigiCollection_,qie11DigiCollection);
  const QIE11DigiCollection& qie11dc=*(qie11DigiCollection);
  /*
  iEvent.getByToken(tok_HBHEDigiCollection_,hbheDigiCollection);
  const HBHEDigiCollection& hbhedc=*(hbheDigiCollection);
  iEvent.getByToken(tok_HFDigiCollection_,hfDigiCollection);
  const HFDigiCollection& hfdc=*(hfDigiCollection);
  iEvent.getByToken(tok_HODigiCollection_,hoDigiCollection);
  const QIE10DigiCollection& hodc=*(hoDigiCollection);
  */
  /* QUESTION: what about dual FED readout? */
  /* QUESTION: what do I do if the number of 16-bit words
               are not divisible by 4? -- these need to
	       fit into the 64-bit words of the FEDRawDataFormat */

  // first argument is the fedid (minFEDID+crateId)
  map<int,HCalFED*> fedMap;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // QIE10 precision data
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  uHTRhelper<QIE10DataFrame> uhtrs_qie10;
  // loop over each digi and allocate memory for each
  for (unsigned int j=0; j < qie10dc.size(); j++){
    QIE10DataFrame qie10df = static_cast<QIE10DataFrame>(qie10dc[j]);
    if( ! uhtrs_qie10.exist(qie10df , *readoutMap ) ){
      uhtrs_qie10.newUHTR( qie10df , *readoutMap );
    }
    uhtrs_qie10.addChannel(qie10df,*readoutMap,_verbosity);
  }
  // loop over each uHTR and format data
  for( UHTRMap::iterator uhtr = uhtrs_qie10.uhtrs.begin() ; uhtr != uhtrs_qie10.uhtrs.end() ; ++uhtr){

    uhtrs_qie10.finalizeHeadTail(&(uhtr->second),_verbosity);
    int fedId = FEDNumbering::MINHCALuTCAFEDID + uhtrs_qie10.getCrate(uhtr->first); // get crateId
    if( fedMap.find(fedId) == fedMap.end() ){
      /* QUESTION: where should the orbit number come from? */
      fedMap[fedId] = new HCalFED(fedId);
    }
    fedMap[fedId]->addUHTR(uhtr->second,uhtrs_qie10.getCrate(uhtr->first),uhtrs_qie10.getSlot(uhtr->first));
  }// end loop over uhtr containers

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // QIE11 precision data
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  uHTRhelper<QIE11DataFrame> uhtrs_qie11;
  // loop over each digi and allocate memory for each
  for (unsigned int j=0; j < qie11dc.size(); j++){
    QIE11DataFrame qie11df = static_cast<QIE11DataFrame>(qie11dc[j]);
    if( ! uhtrs_qie11.exist(qie11df , *readoutMap ) ){
      uhtrs_qie11.newUHTR( qie11df , *readoutMap );
    }
    uhtrs_qie11.addChannel(qie11df,*readoutMap,_verbosity);
  }
  // loop over each uHTR and format data
  for( UHTRMap::iterator uhtr = uhtrs_qie11.uhtrs.begin() ; uhtr != uhtrs_qie11.uhtrs.end() ; ++uhtr){

    uhtrs_qie11.finalizeHeadTail(&(uhtr->second),_verbosity);
    int fedId = FEDNumbering::MINHCALuTCAFEDID + uhtrs_qie11.getCrate(uhtr->first); // get crateId
    if( fedMap.find(fedId) == fedMap.end() ){
      /* QUESTION: where should the orbit number come from? */
      fedMap[fedId] = new HCalFED(fedId);
    }
    fedMap[fedId]->addUHTR(uhtr->second,uhtrs_qie11.getCrate(uhtr->first),uhtrs_qie11.getSlot(uhtr->first));
  }// end loop over uhtr containers

  
  /* ------------------------------------------------------
     ------------------------------------------------------
           putting together the FEDRawDataCollection
     ------------------------------------------------------
     ------------------------------------------------------ */
  for( map<int,HCalFED*>::iterator fed = fedMap.begin() ; fed != fedMap.end() ; ++fed ){

    int fedId = fed->first;

    FEDRawData* rawData =  fed->second->formatFEDdata(); // fed->second.fedData;
    FEDRawData& fedRawData = fed_buffers->FEDData(fedId);
    fedRawData = *rawData;
    delete rawData;

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
