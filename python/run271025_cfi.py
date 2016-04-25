import FWCore.ParameterSet.Config as cms

maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring() 
source = cms.Source ("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles)
readFiles.extend( ['/store/data/Run2016A/MinimumBias/RAW/v1/000/271/025/00000/3205B584-D008-E611-A0C5-02163E0138AC.root',
                   '/store/data/Run2016A/MinimumBias/RAW/v1/000/271/025/00000/38805BAA-CE08-E611-A224-02163E013537.root',
                   '/store/data/Run2016A/MinimumBias/RAW/v1/000/271/025/00000/3E36E4B8-D008-E611-991D-02163E013926.root',
                   '/store/data/Run2016A/MinimumBias/RAW/v1/000/271/025/00000/6C1DF032-D108-E611-AE75-02163E0146D8.root',
                   '/store/data/Run2016A/MinimumBias/RAW/v1/000/271/025/00000/9C666EB6-CE08-E611-A5F3-02163E0134F4.root',
                   '/store/data/Run2016A/MinimumBias/RAW/v1/000/271/025/00000/BA9780E9-D008-E611-B857-02163E0146D8.root'
                   ]);
secFiles.extend( [
        ] )
