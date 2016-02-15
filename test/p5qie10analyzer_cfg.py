import FWCore.ParameterSet.Config as cms

process = cms.Process("H2TestBeam")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(5000) )

#
#   Command Line Input(Copied from DQM for now)
#
import sys
if len(sys.argv)!= 3:
    print "### ERROR: No Run File has been provided"
    print "### Use: cmsRun h2testbeamanalyzer_cfg.py <run number>"
    sys.exit(1)

#
#   Change the filename to process
#
runNumber = sys.argv[2]

#process.source = cms.Source("HcalTBSource",
#    fileNames = cms.untracked.vstring(
        #'file:/afs/cern.ch/work/e/ealvesco/public/B904_Integration_'+runNumber+'.root'
        #'file:/afs/cern.ch/user/w/whitbeck/workPublic/B904_Integration_'+runNumber+'.root'
        #'file:/afs/cern.ch/user/y/yanchu/work/public/cms904/B904_Integration_'+runNumber+'.root'
        #'file:/afs/cern.ch/user/w/whitbeck/workPublic/USC_'+runNumber+'.root',
#        'root://cmsxrootd.fnal.gov///store/hidata/HIRun2015/MinimumBias/RAW/v1/000/263/244/00000/BC8B4F3A-0C9A-E511-AFBA-02163E0142D2.root'
#        )
#)

process.load("HFcommissioning.Analysis.HIdata_cfi")

process.options = cms.untracked.PSet(
        wantSummary = cms.untracked.bool(False)
        )

process.tbunpack = cms.EDProducer("HcalTBObjectUnpacker",
        IncludeUnmatchedHits = cms.untracked.bool(False),
        ConfigurationFile=cms.untracked.string('HFcommissioning/Analysis/test/configQADCTDC.txt'),
        HcalSlowDataFED = cms.untracked.int32(3),
        HcalTriggerFED = cms.untracked.int32(1),
        HcalTDCFED = cms.untracked.int32(8),
        HcalQADCFED = cms.untracked.int32(8),
        fedRawDataCollectionTag = cms.InputTag('source')
)

process.hcalDigis = cms.EDProducer("HcalRawToDigi",
                                   #       UnpackHF = cms.untracked.bool(True),
                                   ### Falg to enable unpacking of TTP channels(default = false)
                                   ### UnpackTTP = cms.untracked.bool(True),
                                   FilterDataQuality = cms.bool(False),
                                   InputLabel = cms.InputTag('source'),
                                   HcalFirstFED = cms.untracked.int32(1132),
                                   ComplainEmptyData = cms.untracked.bool(False),
                                   #       UnpackCalib = cms.untracked.bool(True),
                                   FEDs = cms.untracked.vint32(1132),
                                   firstSample = cms.int32(0),
                                   lastSample = cms.int32(14)
                                   )


process.hcalAnalyzer = cms.EDAnalyzer('HFanalyzer',
        OutFileName = cms.untracked.string('HFanalysisTree_'+runNumber+'.root'),
        Verbosity = cms.untracked.int32(0)
)

#
#   For Debugging: Create a Pool Output Module
#
process.output = cms.OutputModule(
        'PoolOutputModule',
        fileName = cms.untracked.string('HFanalysis_'+runNumber+'.root')
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
                file = cms.FileInPath('HFcommissioning/Analysis/test/EMAP-P5.txt')
               )
        )
)

process.es_prefer = cms.ESPrefer('HcalTextCalibrations', 'es_ascii')

process.dump = cms.EDAnalyzer("HcalDigiDump")

process.p = cms.Path(process.hcalDigis
                     *process.dump
                     *process.hcalAnalyzer
                     )
process.outpath = cms.EndPath(process.output)

