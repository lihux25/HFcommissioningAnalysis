#ifndef PACKER
#define PACKER

#include "DataFormats/HcalDetId/interface/HcalElectronicsId.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"
#include "DataFormats/FEDRawData/interface/FEDRawData.h"

#include "CondFormats/HcalObjects/interface/HcalElectronicsMap.h"

#include <iostream>
#include "stdio.h"
#include <vector>
#include <map>

using namespace std;

typedef vector<uint16_t> uhtrData;
typedef int AMCslot;
typedef int CrateId;
typedef pair<CrateId,AMCslot>    UHTRLoc;
typedef map<int,uhtrData> UHTRMap; // the int here is intended to be the raw electronics ID 
typedef vector<unsigned char> FEDData;
typedef map<int,FEDData> FEDMap;

//int FEDIDmin = FEDNumbering::MINHCALuTCAFEDID;
//int FEDIDmax = FEDNumbering::MAXHCALuTCAFEDID;

class HCalFED{

public:
  
  //FEDMap feds;
  vector<unsigned char> fedData;
  vector<uint64_t> AMCHeaders;
  vector<uhtrData> uhtrs;
  int fedId;
  uint64_t OrbitNum;
  uint64_t Crate;

  /*
  namespace CommonDataFormatHeader{
    static const int OFFSET_H = 0; 
    static const int MASK_H = 0x8; 
    static const int OFFSET_FOV = 3;
    static const int MASK_FOV = 0xF;
    static const int OFFSET_SOURCE_ID = 7; 
    static const int MASK_SOURCE_ID = 0xFFF; 
    static const int OFFSET_BX_ID = 19; 
    static const int MASK_BX_ID = 0xFFF;
    static const int OFFSET_LV1_ID = 31;
    static const int MASK_LV1_ID = 0xFFFFFF;
    static const int OFFSET_EVT_TY = 55;
    static const int MASK_EVT_TY = 0xF;
    static const int OFFSET_FIXED_MSB = 59; 
    static const int MASK_FIXED_MSB = 0xF;
    static const int FIXED_MSB = 0x5;
  }

  namespace AMC13Header{
    static const int OFFSET_FIXED_LSB = 0;
    static const int MASK_FIXED_LSB = 0xF;
    static const int FIXED_LSB = 0x0;
    static const int OFFSET_ORN = 3;
    static const int MASK_ORN = 0xFFFFFFFF;
    static const int OFFSET_RESERVED = 35;
    static const int MASK_RESERVED = 0xFFFF;
    static const int OFFSET_NAMC = 51;
    static const int MASK_NAMC = 0xF;
    static const int OFFSET_RES = 55;
    static const int MASK_RES = 0xF;
    static const int OFFSET_UFOV = 59;
    static const int MASK_UFOV = 0xF;
  }

  namespace AMCHeader{
    static const int OFFSET_CRATE_ID = 0;
    static const int MASK_CRATE_ID = 0xFF;
    static const int OFFSET_SLOT_ID = 7;
    static const int MASK_SLOT_ID = 0xF;
    static const int OFFSET_PRESAMPLES = 11;
    static const int MASK_PRESAMPLES = 0xF;
    static const int OFFSET_AMC_NO = 15;
    static const int MASK_AMC_NO = 0xF;
    static const int OFFSET_BLK_NO = 19;
    static const int MASK_BLK_NO = 0xFF;
    static const int OFFSET_FIXED = 27;
    static const int MASK_FIXED = 0xF;
    static const int OFFSET_AMCN_SIZE = 31;
    static const int MASK_AMCN_SIZE = 0xFFFFFF;
    static const int OFFSET_C = 55;
    static const int MASK_C = 0x1;
    static const int OFFSET_V = 56;
    static const int MASK_V = 0x1;
    static const int OFFSET_P = 57;
    static const int MASK_P = 0x1;
    static const int OFFSET_E = 58;
    static const int MASK_E = 0x1;
    static const int OFFSET_S = 59;
    static const int MASK_S = 0x1;
    static const int OFFSET_M = 60;
    static const int MASK_M = 0x1;
    static const int OFFSET_L = 61;
    static const int MASK_L = 0x1;    
  }
  */

  HCalFED(int fedId_ , uint64_t OrbitNum_=999){
    fedId = fedId_;
    OrbitNum = OrbitNum_;
    Crate = fedId - FEDNumbering::MINHCALuTCAFEDID; 

    // common data format header
    fedData.push_back(0x08);
    fedData.push_back(0xa4);
    fedData.push_back(0xf3);
    fedData.push_back(0x41);
    fedData.push_back(0x01);
    fedData.push_back(0x00);
    fedData.push_back(0x00);
    fedData.push_back(0x51);
    
    cout << "HCalFED, fed size: " << fedData.size() << endl;

    AMC13Header();

  };

  void AMC13Header(){

    uint64_t header = 0 ;
    header |= (OrbitNum&0xFFFFFFFF)<<4 ;
    header |= uint64_t(0x1)<<60 ; // nFOV

    fedData.push_back((header>>0 )&0xFF);
    fedData.push_back((header>>8 )&0xFF);
    fedData.push_back((header>>16)&0xFF);
    fedData.push_back((header>>24)&0xFF);
    fedData.push_back((header>>32)&0xFF);
    fedData.push_back((header>>40)&0xFF);
    fedData.push_back((header>>48)&0xFF);
    fedData.push_back((header>>56)&0xFF);

    cout << "AMC13Header, fed size: " << fedData.size() << endl;

  }

  uint64_t AMCHeader( uint64_t crate , uint64_t slot , uint64_t AMCsize , uint64_t presamples = 10 , uint64_t blockNum = 0 ){

    uint64_t header = 0 ;
    header |= crate&0xFF ;
    header |= (slot&0xF)<<8 ;
    header |= (presamples&0xF)<<12 ; // boardId
    header |= (slot&0xF)<<16 ; // AMC no.
    header |= (blockNum&0xFF)<<20 ; // Block No.
    header |= (AMCsize&0xFFFFFF)<<32 ; // size 
    header |= uint64_t(0x1)<<56 ; // CRC is valide
    header |= uint64_t(0x1)<<57 ; // EvN, BcN match
    header |= uint64_t(0x1)<<58 ; // Present, header is only made if data is present
    header |= uint64_t(0x1)<<59 ; // Enabled, header is only made if AMC is enabled
    header |= uint64_t(0x0)<<60 ; // Segmented, always zero for unsegmented data
    header |= uint64_t(0x0)<<61 ; // More data
    header |= uint64_t(0x0)<<62 ; // Indicates length error

    return header;
  }

  void addUHTR( uhtrData uhtr , int crate , int slot ){
    // push uhtr data into FED container
    uhtrs.push_back(uhtr);
    // create the corresponding AMC header
    AMCHeaders.push_back( AMCHeader( crate , slot , uhtr.size()/4 ) );
  };
  
  // does not include HEADER and TRAILER
  FEDRawData* formatFEDdata(){

    //vector<unsigned char> output;
    if( uhtrs.size() != AMCHeaders.size() ){
      return NULL ;
    }

    // set the number of AMCs in the AMC13 header
    fedData[14] |= (uhtrs.size()&0xF)<<4;

    // fill fedData with AMC headers
    for( unsigned int iAMC = 0 ; iAMC < AMCHeaders.size() ; ++iAMC ){
      // adjust the AMCsize bits to match uhtr header
      //AMCHeaders[iAMC] |= uint64_t(uhtrs[iAMC][1]&0xF)<<51 ;
      //AMCHeaders[iAMC] |= uint64_t(uhtrs[iAMC][0]&0xFFFF)<<47 ;

      fedData.push_back((AMCHeaders[iAMC]>>0 )&0xFF); // split 64-bit words into 8-bit 
      fedData.push_back((AMCHeaders[iAMC]>>8 )&0xFF); // split 64-bit words into 8-bit 
      fedData.push_back((AMCHeaders[iAMC]>>16)&0xFF); // split 64-bit words into 8-bit 
      fedData.push_back((AMCHeaders[iAMC]>>24)&0xFF); // split 64-bit words into 8-bit 
      fedData.push_back((AMCHeaders[iAMC]>>32)&0xFF); // split 64-bit words into 8-bit 
      fedData.push_back((AMCHeaders[iAMC]>>40)&0xFF); // split 64-bit words into 8-bit 
      fedData.push_back((AMCHeaders[iAMC]>>48)&0xFF); // split 64-bit words into 8-bit 
      fedData.push_back((AMCHeaders[iAMC]>>56)&0xFF); // split 64-bit words into 8-bit 
    }

    // fill fedData with AMC data 
    for( unsigned int iAMC = 0 ; iAMC<uhtrs.size() ; ++iAMC ){

      unsigned int nWords16 = uhtrs[iAMC].size(); 
      for( unsigned int amcWord = 0 ; amcWord<nWords16 ; ++amcWord ){
	fedData.push_back((uhtrs[iAMC][amcWord]>>0 )&0xFF);  // split 16-bit words into 8-bit
	fedData.push_back((uhtrs[iAMC][amcWord]>>8 )&0xFF);
      }// end loop over uhtr words
    }// end loop over uhtrs

    // fedData should be a integer number of 64 bit words
    while( fedData.size()%8 != 0 )
      fedData.push_back(0);

    // convert to the correct format
    FEDRawData *rawData = new FEDRawData(fedData.size());
    unsigned char* words = reinterpret_cast<unsigned char*>(rawData->data());

    for( unsigned int i = 0 ; i < fedData.size() ; ++i ){
      //std::cout << "Full FED data: " << int(fedData[i]) << std::endl;
      *words = fedData[i];
      words++;
    }

    for( unsigned int i = 0 ; i < rawData->size() ; i+=2){
      printf("Full FED data: %02X%02X \n",int(*(rawData->data()+i+1)),int(*(rawData->data()+i)));
    }

    return rawData; //output;

  };

};

class uHTRpacker{

public: 

  UHTRMap uhtrs;

  // FIRST WORD
  static const int OFFSET_DATA_LENGTH = 0;
  static const int MASK_DATA_LENGTH = 0xFFFFF;
  static const int OFFSET_BCN = 19;
  static const int MASK_BCN = 0xFFF;
  static const int OFFSET_EVN = 31;
  static const int MASK_EVN = 0xFFFFFF;
  static const int OFFSET_FILED_BY_AMC13 = 55;
  static const int MASK_FILED_BY_AMC13 = 0xFF;
  // SECOND WORD
  static const int OFFSET_CRATE_ID = 0;
  static const int MASK_CRATE_ID = 0xFFFFFF;
  static const int OFFSET_SLOT_ID = 7;
  static const int MASK_SLOT_ID = 0xF;
  static const int OFFSET_PRESAMPLES = 11;
  static const int MASK_PRESAMPLES = 0xF;
  static const int OFFSET_ORN = 15;
  static const int MASK_ORN = 0xFFFF;
  static const int OFFSET_FW_FLAVOR = 31;
  static const int MASK_FW_FLAVOR = 0xFF;
  static const int OFFSET_EVENT_TYPE = 39;
  static const int MASK_EVENT_TYPE = 0xF;
  static const int OFFSET_PAYLOAD_FORMAT = 43;
  static const int MASK_PAYLOAD_FORMAT = 0xF;
  static const int OFFSET_FW_VERSION = 47;
  static const int MASK_FW_VERSION = 0xFFFF;

  uHTRpacker(){};

  /*
  //QUESTION: need some error handling, I guess, for the DetId stuff????
  int getCrate( digiType qiedf , const HcalElectronicsMap& emap ){
    DetId detid = qiedf.detid();
    //printf("DetId: %08X",detid.rawId());
    HcalElectronicsId eid(emap.lookup(detid));
    //printf("HCalElectronicsId: %08X",eid.rawId());

    return eid.crateId() ;

  };

  int getSlot( digiType qiedf , const HcalElectronicsMap& emap ){
    DetId detid = qiedf.detid();
    //printf("DetId: %08X",detid.rawId());
    HcalElectronicsId eid(emap.lookup(detid));
    //printf("HCalElectronicsId: %08X",eid.rawId());

    return eid.slot() ;
  };

  int getCrate( int index ){ return index&0xFF; };
  int getSlot( int index ){ return (index&0xF00)>>8; };

  int getLocation( digiType qiedf , const HcalElectronicsMap& emap ){
    // Extract info on detector location
    DetId detid = qiedf.detid();
    //printf("DetId: %08X \n",detid.rawId());
    HcalElectronicsId eid(emap.lookup(detid));
    //printf("HCalElectronicsId: %08X \n",eid.rawId());
        
    uint16_t index = eid.crateId()&0xFF;
    index |= (eid.slot()&0xF)<<8;

    return index;

  };
  */

  bool exist( int uhtrIndex ){

    //printf("uhtr index: %03X \n",index);
    //for( UHTRMap::iterator uhtr = uhtrs.begin() ; uhtr != uhtrs.end() ; ++uhtr){  
    //  printf("map key: %03X \n",uhtr->first);
    //}
    
    return uhtrs.count(uhtrIndex) != 0  ; 

  };

  uhtrData* newUHTR( int uhtrIndex , int orn = 0 , int bcn = 0 , int evt = 0 ){
    
    // initialize vector of 16-bit words
    uhtrs[uhtrIndex] = uhtrData(8);
    // build header -- some information will be updated at the end    
    
    uint64_t presamples    = 10;     // hardcoded for testing purposes
    uint64_t uhtrCrate     = uhtrIndex&0xFF;
    uint64_t uhtrSlot      = uhtrIndex&0xF00; 
    uint64_t fwFlavor      = 0x41;    // hardcoded for testing purposes
    uint64_t eventType     = 0x0;     // hardcoded for testing purposes
    uint64_t payloadFormat = 0x1;   // hardcoded for testing purposes
    uint64_t fwVersion     = 0x1560;  // hardcoded for testing purposes

    uint64_t uhtrHeader1 = 0;
    uhtrHeader1 |= (uint64_t(0x0)&MASK_DATA_LENGTH)<<OFFSET_DATA_LENGTH;
    uhtrHeader1 |= (bcn&MASK_BCN)<<OFFSET_BCN;
    uhtrHeader1 |= (evt&MASK_EVN)<<OFFSET_EVN;
    uhtrHeader1 |= (uint64_t(0x0)&MASK_FILED_BY_AMC13)<<OFFSET_FILED_BY_AMC13;
    
    uint64_t uhtrHeader2 = 0;
    uhtrHeader2 |= (uhtrCrate&MASK_CRATE_ID)<<OFFSET_CRATE_ID;
    uhtrHeader2 |= (uhtrSlot&MASK_SLOT_ID)<<OFFSET_SLOT_ID;
    uhtrHeader2 |= (presamples&MASK_PRESAMPLES)<<OFFSET_PRESAMPLES;
    uhtrHeader2 |= (orn&MASK_ORN)<<OFFSET_ORN;
    uhtrHeader2 |= (fwFlavor&MASK_FW_FLAVOR)<<OFFSET_FW_FLAVOR;
    uhtrHeader2 |= (eventType&MASK_EVENT_TYPE)<<OFFSET_EVENT_TYPE;
    uhtrHeader2 |= (payloadFormat&MASK_PAYLOAD_FORMAT)<<OFFSET_PAYLOAD_FORMAT;
    uhtrHeader2 |= (fwVersion&MASK_FW_VERSION)<<OFFSET_FW_VERSION;

    // push header into vector of 16-bit words
    uhtrs[uhtrIndex][0] = (uhtrHeader1>>0)&0xFFFF ;
    uhtrs[uhtrIndex][1] = (uhtrHeader1>>16)&0xFFFF ;
    uhtrs[uhtrIndex][2] = (uhtrHeader1>>32)&0xFFFF ;
    uhtrs[uhtrIndex][3] = (uhtrHeader1>>48)&0xFFFF ;
    uhtrs[uhtrIndex][4] = (uhtrHeader2>>0)&0xFFFF ;
    uhtrs[uhtrIndex][5] = (uhtrHeader2>>16)&0xFFFF ;
    uhtrs[uhtrIndex][6] = (uhtrHeader2>>32)&0xFFFF ;
    uhtrs[uhtrIndex][7] = (uhtrHeader2>>48)&0xFFFF ;

    return &(uhtrs[uhtrIndex]);
  };

  void finalizeHeadTail(uhtrData* uhtr , bool verbosity){

    uint64_t uhtr_size = uhtr->size()-8;

    // adjust the size bits
    uhtr->at(0) = uhtr_size&0xFFFF ;
    uhtr->at(1) |= (uhtr_size>>16)&0xF ; 

    // add trailer
    uhtr->push_back( uhtr_size&0xFFFF );
    uhtr->push_back( (uhtr_size>>16)&0xF );
    // this is ignoring the event number... I am not sure what this should be

    // adding some blank stuff for the CRC bits
    uhtr->push_back( 0 );
    uhtr->push_back( 0 );

    if( verbosity>0 ){
      for( unsigned int i = 0 ; i < uhtr->size() ; i++ ){
	printf("raw from uhtr: %04X \n",uhtr->at(i));
      }
    }
    
  };

  void addChannel( int uhtrIndex , edm::SortedCollection<HFDataFrame>::const_iterator& qiedf , int verbosity = 0 ){
    // loop over words in dataframe
    for( int iTS = 0 ; iTS < qiedf->size() ; iTS++ ){
      if(verbosity>0) printf("raw from HF digi: %04X \n",qiedf->sample(iTS).raw());
      // push data into uhtr data container
      uhtrs[uhtrIndex].push_back(qiedf->sample(iTS).raw());
    }// end loop over dataframe words
  };

  void addChannel( int uhtrIndex , edm::SortedCollection<HBHEDataFrame>::const_iterator qiedf , int verbosity = 0 ){
    // loop over words in dataframe
    for( int iTS = 0 ; iTS < qiedf->size() ; iTS++ ){
      if(verbosity>0) printf("raw from HF digi: %04X \n",qiedf->sample(iTS).raw());
      // push data into uhtr data container
      uhtrs[uhtrIndex].push_back(qiedf->sample(iTS).raw());
    }// end loop over dataframe words
  };

  void addChannel( int uhtrIndex , QIE11DataFrame qiedf , int verbosity = 0 ){ 
    // loop over words in dataframe 
    for(edm::DataFrame::iterator dfi=qiedf.begin() ; dfi!=qiedf.end(); ++dfi){      
      if(verbosity>0) printf("raw from digi: %04X \n",dfi[0]);
      // push data into uhtr data container
      uhtrs[uhtrIndex].push_back(dfi[0]);
    }// end loop over dataframe words
  };

  void addChannel( int uhtrIndex , QIE10DataFrame qiedf , int verbosity = 0 ){ 
    // loop over words in dataframe 
    for(edm::DataFrame::iterator dfi=qiedf.begin() ; dfi!=qiedf.end(); ++dfi){      
      if(verbosity>0) printf("raw from digi: %04X \n",dfi[0]);
      // push data into uhtr data container
      uhtrs[uhtrIndex].push_back(dfi[0]);
    }// end loop over dataframe words
  };

};

#endif
