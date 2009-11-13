import FWCore.ParameterSet.Config as cms

HLTMuonDimuonL3Filter = cms.EDFilter( "HLTMuonDimuonL3Filter",
                    BeamSpotTag = cms.InputTag( "hltOfflineBeamSpot" ),
                    CandTag = cms.InputTag( "hltL3MuonCandidates" ),
                    PreviousCandTag = cms.InputTag( '' ),
                    FastAccept = cms.bool( False ),
                    MaxEta = cms.double( 2.5 ),
                    MinNhits = cms.int32( 0 ),
                    MaxDr = cms.double( 2.0 ),
                    MaxDz = cms.double( 9999.0 ),
                    ChargeOpt = cms.int32( 0 ),
                    MinPtPair = cms.double( 0.0 ),
                    MinPtMax = cms.double( 3.0 ),
                    MinPtMin = cms.double( 3.0 ),
                    MinInvMass = cms.double( 2.8 ),
                    MaxInvMass = cms.double( 3.4 ),
                    MinAcop = cms.double( -1.0 ),
                    MaxAcop = cms.double( 3.15 ),
                    MinPtBalance = cms.double( -1.0 ),
                    MaxPtBalance = cms.double( 999999.0 ),
                    NSigmaPt = cms.double( 0.0 ),
                    SaveTag = cms.untracked.bool( False )
                )
