import FWCore.ParameterSet.Config as cms

process = cms.Process("digi2rawTesting")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("HcalTBSource",
    fileNames = cms.untracked.vstring(
        'file:./B904_Integration_002002.root'
    )
)

process.options = cms.untracked.PSet(
        wantSummary = cms.untracked.bool(False)
        )

process.hcalDigis = cms.EDProducer("HcalRawToDigi",
                                   #       UnpackHF = cms.untracked.bool(True),
                                   ### Falg to enable unpacking of TTP channels(default = false)
                                   ### UnpackTTP = cms.untracked.bool(True),
                                   FilterDataQuality = cms.bool(False),
                                   InputLabel = cms.InputTag('source'),
                                   HcalFirstFED = cms.untracked.int32(932),
                                   ComplainEmptyData = cms.untracked.bool(False),
                                   #       UnpackCalib = cms.untracked.bool(True),
                                   FEDs = cms.untracked.vint32(932),
                                   firstSample = cms.int32(0),
                                   lastSample = cms.int32(9),
                                   silent = cms.untracked.bool(False)
                                   )

process.digi2raw = cms.EDProducer("digi2rawTester",
                                  Verbosity = cms.untracked.int32(0)
                                  )

process.hcalDigi2rawDigi = cms.EDProducer("HcalRawToDigi",
                                          FilterDataQuality = cms.bool(False),
                                          InputLabel = cms.InputTag('digi2raw'),
                                          HcalFirstFED = cms.untracked.int32(1153),
                                          ComplainEmptyData = cms.untracked.bool(False),
                                          #       UnpackCalib = cms.untracked.bool(True),
                                          FEDs = cms.untracked.vint32(1153),
                                          firstSample = cms.int32(0),
                                          lastSample = cms.int32(9),
                                          silent = cms.untracked.bool(False)
                                   )

process.hcalAnalyzer = cms.EDAnalyzer('HFanalyzer',
                                       OutFileName = cms.untracked.string('HFanalysisTree_raw2digi.root'),
                                       Verbosity = cms.untracked.int32(0),
                                       digiCollection = cms.untracked.string('hcalDigis')
)

process.hcalAnalyzer2 = cms.EDAnalyzer('HFanalyzer',
                                       OutFileName = cms.untracked.string('HFanalysisTree_digi2rawTest.root'),
                                       Verbosity = cms.untracked.int32(0),
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
                file = cms.FileInPath('HFcommissioning/Analysis/test/EMAP-QUADRANT.txt')
               )
        )
)
process.es_prefer = cms.ESPrefer('HcalTextCalibrations', 'es_ascii')

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
                     #*process.hcalAnalyzer
                     *process.hcalAnalyzer2
                     )
process.outpath = cms.EndPath(process.output)
