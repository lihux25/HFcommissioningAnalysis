#ifndef PACKERHELP
#define PACKERHELP

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
    AMCHeaders.push_back( AMCHeader( crate , slot , uhtr.size() ) );
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
      fedData.push_back((AMCHeaders[iAMC]>>0 )&0xFF); // split 64-bit words into 8-bit 
      fedData.push_back((AMCHeaders[iAMC]>>8 )&0xFF); // split 64-bit words into 8-bit 
      fedData.push_back((AMCHeaders[iAMC]>>16)&0xFF); // split 64-bit words into 8-bit 
      fedData.push_back((AMCHeaders[iAMC]>>24)&0xFF); // split 64-bit words into 8-bit 
      fedData.push_back((AMCHeaders[iAMC]>>32)&0xFF); // split 64-bit words into 8-bit 
      fedData.push_back((AMCHeaders[iAMC]>>40)&0xFF); // split 64-bit words into 8-bit 
      fedData.push_back((AMCHeaders[iAMC]>>48)&0xFF); // split 64-bit words into 8-bit 
      fedData.push_back((AMCHeaders[iAMC]>>56)&0xFF); // split 64-bit words into 8-bit 

      cout << "formatFEDData, fedData size: " << fedData.size() << endl;
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

template <class digiType> class uHTRhelper{

public: 

  UHTRMap uhtrs;

  uHTRhelper(){};

  /* QUESTION: need some error handling, I guess, 
     for the DetId stuff???? */
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
        
    /*  Defining a custom index that will encode only
	the information about the crate and slot of a 
	given channel:

	crate: bits 0-7
	slot:  bits 8-12
    */
    uint16_t index = eid.crateId()&0xFF;
    index |= (eid.slot()&0xF)<<8;

    return index;

  };

  bool exist( digiType qiedf , const HcalElectronicsMap& emap ){

    int index = getLocation( qiedf , emap );

    //printf("uhtr index: %03X \n",index);
    //for( UHTRMap::iterator uhtr = uhtrs.begin() ; uhtr != uhtrs.end() ; ++uhtr){  
    //  printf("map key: %03X \n",uhtr->first);
    //}
    
    return uhtrs.count(index) != 0  ; 

  };

  uhtrData* newUHTR( digiType qiedf , const HcalElectronicsMap& emap ){
    
    int uhtrLoc = getLocation( qiedf , emap );
    // initialize vector of 16-bit words
    uhtrs[uhtrLoc] = uhtrData(8);
    // build header -- some information will be updated at the end    
    
    int presamples = 10;
    int uhtrCrate = getCrate(qiedf,emap);
    int uhtrSlot  = getSlot(qiedf,emap); 
    uhtrs[uhtrLoc][0] = 0 ;
    uhtrs[uhtrLoc][1] = 0 ;
    uhtrs[uhtrLoc][2] = 0 ;
    uhtrs[uhtrLoc][3] = 0 ;
    uhtrs[uhtrLoc][4] = ((presamples&0xF)<<12)|((uhtrSlot&0xF)<<8)|(uhtrCrate); // n-presamples hardcoded for testing purposes
    uhtrs[uhtrLoc][5] = 0 ;
    uhtrs[uhtrLoc][6] = 0x1041;// hardcoded for testing purposes
    uhtrs[uhtrLoc][7] = 0x1560;// hardcoded for testing purposes         
    // will this ever be null?

    cout << "SANITY CHECK: " << uhtrs[uhtrLoc].size() << endl;

    return &(uhtrs[uhtrLoc]);
  };

  void addChannel( digiType qiedf , const HcalElectronicsMap& emap , int verbosity ){ 

    int uhtrLoc = getLocation( qiedf , emap );
    
    /*
    // fill info about channel (first 16-bits)
    uint16_t prefix = 1<<15 ; // constant
    prefix |= 2<<12  ; // flavor 
    prefix |= 1<< 11 ; // Link Error
    prefix |= 0<< 9 ; // Reserved
    prefix |= 0<< 8  ; // Mark & Pass
    prefix |= (8*fiber+fiberChannel)&0xFF; // channel id

    printf("PREFIX: %04X \n",prefix);
    uhtrs[uhtrLoc].push_back( prefix );
    */
    
    // loop over words in dataframe 
    for(edm::DataFrame::iterator dfi=qiedf.begin() ; dfi!=qiedf.end(); ++dfi){      
      if(verbosity>0) printf("raw from digi: %04X \n",dfi[0]);
      // push data into uhtr data container
      uhtrs[uhtrLoc].push_back(dfi[0]);
    }// end loop over dataframe words

  };

  void finalizeHeadTail(uhtrData* uhtr , bool verbosity){
    // add trailer to uhtrData
    uint64_t uhtr_size = uhtr->size()+4;
    uhtr->push_back( uhtr_size&0xFFFF );
    uhtr->push_back( (uhtr_size>>16)&0xF );
    // this is ignoring the event number... I am not sure what this should be

    // adding some blank stuff for the CRC bits
    uhtr->push_back( 0 );
    uhtr->push_back( 0 );

    // set size in header
    uhtr->at(0) = uhtr_size&0xFFFF ;
    uhtr->at(1) |= (uhtr_size>>16)&0xF ; 

    if( verbosity>0 ){
      for( unsigned int i = 0 ; i < uhtr->size() ; i++ ){
	printf("raw from uhtr: %04X \n",uhtr->at(i));
      }
    }
    
  };

};

#endif
