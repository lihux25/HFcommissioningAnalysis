import FWCore.ParameterSet.Config as cms

process = cms.Process("digi2rawTesting")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

#process.load("HFcommissioning.Analysis.run271025_cfi")
readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring()
process.source = cms.Source ("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles)
readFiles.extend( [
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/006C252F-5E26-E611-AFB0-02163E012604.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/02C7CBEF-5D26-E611-AB42-02163E011DC2.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/04527432-5E26-E611-B8E9-02163E011E61.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/04D1DDEF-5D26-E611-8C39-02163E011B54.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/06640709-5E26-E611-8A03-02163E014473.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/089A4534-5D26-E611-AB0E-02163E0146A2.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/0A065709-5E26-E611-B9CB-02163E013557.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/0A23DB08-5E26-E611-ADA8-02163E0143A2.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/0A411C55-5D26-E611-8033-02163E0143C8.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/0E4CB32B-5E26-E611-9DD0-02163E012A6E.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/10A5FFED-5D26-E611-8FFC-02163E014724.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/16D2A2EF-5D26-E611-B04E-02163E011DC7.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/183F1A09-5E26-E611-8D10-02163E01423B.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/18FEE528-5E26-E611-B739-02163E014454.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/1A114A0A-5E26-E611-88C8-02163E011CE2.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/20498B2B-5E26-E611-B02A-02163E014101.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/20F9B43D-5E26-E611-92CE-02163E012801.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/22FFF017-5E26-E611-9209-02163E011C60.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/244B11F0-5D26-E611-A3F5-02163E011F9D.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/24B7F856-5D26-E611-AA59-02163E014127.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/28906530-5D26-E611-9B62-02163E011B75.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/2C274735-5E26-E611-A1E6-02163E011FBB.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/2C6C4D08-5E26-E611-B089-02163E0139CA.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/2CA9A771-5D26-E611-BCBF-02163E014716.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/30124C0A-5E26-E611-BFED-02163E013589.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/329D9DE5-5D26-E611-A79D-02163E013427.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/32AAB4EE-5D26-E611-A0C9-02163E01459E.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/3435AB34-5D26-E611-A860-02163E0146A7.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/36AD9A3D-5E26-E611-936D-02163E0141B4.root',
    '/store/data/Run2016B/JetHT/RAW/v2/000/274/199/00000/36FABC0A-5E26-E611-9153-02163E0133BB.root',
                   ]);
secFiles.extend( [
        ] )

process.options = cms.untracked.PSet(
        wantSummary = cms.untracked.bool(False)
        )

process.hcalDigis = cms.EDProducer("HcalRawToDigi",
                                   FilterDataQuality = cms.bool(False),
                                   InputLabel = cms.InputTag('rawDataCollector'),
                                   HcalFirstFED = cms.untracked.int32(1132),
                                   ComplainEmptyData = cms.untracked.bool(False),
                                   FEDs = cms.untracked.vint32(1132),
                                   firstSample = cms.int32(0),
                                   lastSample = cms.int32(10),
                                   silent = cms.untracked.bool(False)
                                   )

process.load("EventFilter.HcalRawToDigi.HcalRawToDigi_cfi")

process.digi2raw = cms.EDProducer("digi2rawTester",
                                  Verbosity = cms.untracked.int32(6)
                                  )

process.hcalDigi2rawDigi = cms.EDProducer("HcalRawToDigi",
                                          FilterDataQuality = cms.bool(False),
                                          InputLabel = cms.InputTag('digi2raw'),
#                                          HcalFirstFED = cms.untracked.int32(1153),
                                          ComplainEmptyData = cms.untracked.bool(False),
#                                          FEDs = cms.untracked.vint32(1153),
                                          firstSample = cms.int32(0),
                                          lastSample = cms.int32(10),
                                          silent = cms.untracked.bool(False)
                                   )

process.hcalAnalyzer = cms.EDAnalyzer('HFanalyzer',
                                       OutFileName = cms.untracked.string('HFanalysisTree_raw2digi.root'),
                                       Verbosity = cms.untracked.int32(0),
                                       digiCollection = cms.untracked.string('hcalDigis')
)

process.hcalAnalyzer2 = cms.EDAnalyzer('HFanalyzer',
                                       OutFileName = cms.untracked.string('HFanalysisTree_digi2rawTest.root'),
                                       Verbosity = cms.untracked.int32(3),
                                       digiCollection = cms.untracked.string('hcalDigi2rawDigi')
)

process.load('Configuration.Geometry.GeometryIdeal_cff')

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.autoCond import autoCond
from CondCore.DBCommon.CondDBSetup_cfi import *

process.GlobalTag.globaltag = autoCond['startup'] 

#   EMAP Needed for H2 DATA
process.es_ascii = cms.ESSource('HcalTextCalibrations',
        input = cms.VPSet(
               cms.PSet(
                object = cms.string('ElectronicsMap'),
                file = cms.FileInPath('HFcommissioning/Analysis/test/UXCngHFmap.txt')
               )
        )
)

process.es_prefer = cms.ESPrefer('HcalTextCalibrations', 'es_ascii')

#   EMAP Needed for H2 DATA
#process.es_ascii = cms.ESSource('HcalTextCalibrations',
#        input = cms.VPSet(
#               cms.PSet(
#                object = cms.string('ElectronicsMap'),
#                file = cms.FileInPath('HFcommissioning/Analysis/test/EMAP-QUADRANT.txt')
#               )
#        )
#)
#process.es_prefer = cms.ESPrefer('HcalTextCalibrations', 'es_ascii')

#
#   For Debugging: Create a Pool Output Module
#
process.output = cms.OutputModule(
        'PoolOutputModule',
        fileName = cms.untracked.string('digi2Raw.root')
)

process.p = cms.Path(process.hcalDigis
                     *process.digi2raw
                     *process.hcalDigi2rawDigi
                     *process.hcalAnalyzer
                     *process.hcalAnalyzer2
                     )
process.outpath = cms.EndPath(process.output)
