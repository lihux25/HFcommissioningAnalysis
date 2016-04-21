import FWCore.ParameterSet.Config as cms

process = cms.Process("H2TestBeam")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(5000) )

process.load("HFcommissioning.Analysis.firstBeams_cfi")

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
                                   lastSample = cms.int32(10)
                                   )


process.hcalAnalyzer = cms.EDAnalyzer('HFanalyzer',
                                      OutFileName = cms.untracked.string('HFanalysisTree_validation.root'),
                                      Verbosity = cms.untracked.int32(0),
                                      digiCollection = cms.untracked.string('hcalDigis')
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

process.dump = cms.EDAnalyzer("HcalDigiDump")

process.p = cms.Path(process.hcalDigis
                     #*process.dump
                     *process.hcalAnalyzer
                     )

