import FWCore.ParameterSet.Config as cms

process = cms.Process("ECALDQM")

import RecoLocalCalo.EcalRecProducers.ecalFixedAlphaBetaFitUncalibRecHit_cfi
process.ecalUncalibHit = RecoLocalCalo.EcalRecProducers.ecalFixedAlphaBetaFitUncalibRecHit_cfi.ecalFixedAlphaBetaFitUncalibRecHit.clone()

#import RecoLocalCalo.EcalRecProducers.ecalGlobalUncalibRecHit_cfi
#process.ecalUncalibHit = RecoLocalCalo.EcalRecProducers.ecalGlobalUncalibRecHit_cfi.ecalGlobalUncalibRecHit.clone()

process.load("RecoLocalCalo.EcalRecProducers.ecalRecHit_cfi")

process.load("RecoLocalCalo.EcalRecAlgos.EcalSeverityLevelESProducer_cfi")

process.load("Geometry.CaloEventSetup.CaloGeometry_cfi")

process.load("Geometry.CaloEventSetup.CaloTopology_cfi")

process.load("Geometry.CaloEventSetup.EcalTrigTowerConstituents_cfi")

process.load("Geometry.CMSCommonData.cmsIdealGeometryXML_cfi")

process.load("Geometry.EcalMapping.EcalMapping_cfi")

process.load("Geometry.EcalMapping.EcalMappingRecord_cfi")

process.load("RecoEcal.EgammaClusterProducers.ecalClusteringSequence_cff")

process.load("CalibCalorimetry.EcalLaserCorrection.ecalLaserCorrectionService_cfi")

process.load("DQM.EcalEndcapMonitorModule.EcalEndcapMonitorModule_cfi")

process.load("DQM.EcalEndcapMonitorTasks.EcalEndcapMonitorTasks_cfi")

process.load("DQM.EcalEndcapMonitorTasks.mergeRuns_cff")

process.load("DQM.EcalEndcapMonitorClient.EcalEndcapMonitorClient_cfi")

process.load("DQMServices.Core.DQM_cfg")

process.load("FWCore.Modules.preScaler_cfi")

process.dqmInfoEE = cms.EDAnalyzer("DQMEventInfo",
    subSystemFolder = cms.untracked.string('EcalEndcap')
)

process.dqmQTestEE = cms.EDAnalyzer("QualityTester",
    reportThreshold = cms.untracked.string('red'),
    prescaleFactor = cms.untracked.int32(1),
    qtList = cms.untracked.FileInPath('DQM/EcalEndcapMonitorModule/test/data/EcalEndcapQualityTests.xml'),
    getQualityTestsFromFile = cms.untracked.bool(True)
)

process.dqmSaver = cms.EDAnalyzer("DQMFileSaver",
    dirName = cms.untracked.string('.'),
    convention = cms.untracked.string('Online')
)

process.maxEvents = cms.untracked.PSet(
#    input = cms.untracked.int32(150)
#    input = cms.untracked.int32(300)
)

process.source = cms.Source("PoolSource",
    noEventSort = cms.untracked.bool(True),
    duplicateCheckMode = cms.untracked.string('noDuplicateCheck'),
#---
    setRunNumber = cms.untracked.uint32(10),
    fileNames = cms.untracked.vstring('/store/user/dellaric/data/relval_zee_310.root', 
        '/store/user/dellaric/data/relval_zee_310.root', 
        '/store/user/dellaric/data/relval_zee_310.root', 
        '/store/user/dellaric/data/relval_zee_310.root', 
        '/store/user/dellaric/data/relval_zee_310.root', 
        '/store/user/dellaric/data/relval_zee_310.root', 
        '/store/user/dellaric/data/relval_zee_310.root', 
        '/store/user/dellaric/data/relval_zee_310.root', 
        '/store/user/dellaric/data/relval_zee_310.root', 
        '/store/user/dellaric/data/relval_zee_310.root', 
        '/store/user/dellaric/data/relval_zee_310.root', 
        '/store/user/dellaric/data/relval_zee_310.root')
#---
)

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = "GR_R_44_V1::All"

process.MessageLogger = cms.Service("MessageLogger",
    cout = cms.untracked.PSet(
        threshold = cms.untracked.string('WARNING'),
        noLineBreaks = cms.untracked.bool(True),
        noTimeStamps = cms.untracked.bool(True),
        default = cms.untracked.PSet(
            limit = cms.untracked.int32(0)
        ),
        EcalEndcapMonitorModule = cms.untracked.PSet(
            limit = cms.untracked.int32(0)
        )
    ),
    categories = cms.untracked.vstring('EcalEndcapMonitorModule'),
    destinations = cms.untracked.vstring('cout')
)

process.preScaler.prescaleFactor = 1

process.ecalDataSequence = cms.Sequence(process.preScaler*process.ecalUncalibHit*process.ecalRecHit*process.hybridClusteringSequence*process.multi5x5BasicClustersCleaned*process.multi5x5SuperClustersCleaned)

process.ecalEndcapMonitorSequence = cms.Sequence(process.ecalEndcapMonitorModule*process.dqmInfoEE*process.ecalEndcapMonitorClient*process.dqmQTestEE)

process.p = cms.Path(process.ecalDataSequence*process.ecalEndcapMonitorSequence*process.dqmSaver)
process.q = cms.EndPath(process.ecalEndcapDefaultTasksSequence*process.ecalEndcapClusterTask)

process.ecalUncalibHit.MinAmplBarrel = 12.
process.ecalUncalibHit.MinAmplEndcap = 16.
process.ecalUncalibHit.EBdigiCollection = 'simEcalDigis:ebDigis'
process.ecalUncalibHit.EEdigiCollection = 'simEcalDigis:eeDigis'

process.ecalRecHit.EBuncalibRecHitCollection = 'ecalUncalibHit:EcalUncalibRecHitsEB'
process.ecalRecHit.EEuncalibRecHitCollection = 'ecalUncalibHit:EcalUncalibRecHitsEE'

process.ecalEndcapMonitorModule.mergeRuns = True
process.ecalEndcapMonitorModule.EEDigiCollection = 'simEcalDigis:eeDigis'
process.ecalEndcapMonitorModule.runType = 13 # PHYSICS_GLOBAL
process.ecalEndcapMonitorModule.EcalTrigPrimDigiCollection = 'ecalTriggerPrimitiveDigis'

process.ecalEndcapOccupancyTask.EEDigiCollection = 'simEcalDigis:eeDigis'
process.ecalEndcapOccupancyTask.EcalTrigPrimDigiCollection = 'ecalTriggerPrimitiveDigis'

process.ecalEndcapPedestalOnlineTask.EEDigiCollection = 'simEcalDigis:eeDigis'

process.ecalEndcapTriggerTowerTask.EEDigiCollection = 'simEcalDigis:eeDigis'

process.ecalEndcapMonitorClient.mergeRuns = True
process.ecalEndcapMonitorClient.location = 'H4'
process.ecalEndcapMonitorClient.enabledClients = ['Integrity', 'Occupancy', 'PedestalOnline', 'Timing', 'Cluster', 'Summary']

