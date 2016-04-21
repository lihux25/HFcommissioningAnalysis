import FWCore.ParameterSet.Config as cms

maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring() 
source = cms.Source ("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles)
readFiles.extend( ['/store/data/Commissioning2016/MinimumBias/RAW/v1/000/270/394/00000/2239CB18-CD06-E611-87BE-02163E013462.root',
                   '/store/data/Commissioning2016/MinimumBias/RAW/v1/000/270/394/00000/289FE04D-BC06-E611-97ED-02163E01279D.root',
                   '/store/data/Commissioning2016/MinimumBias/RAW/v1/000/270/394/00000/6AC95BCF-B706-E611-9C31-02163E0142E5.root',
                   '/store/data/Commissioning2016/MinimumBias/RAW/v1/000/270/394/00000/72611D74-C006-E611-BA14-02163E01381F.root',
                   '/store/data/Commissioning2016/MinimumBias/RAW/v1/000/270/394/00000/92E331A7-B306-E611-A141-02163E011D60.root'] );
secFiles.extend( [
               ] )
