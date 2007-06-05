#ifndef HLTrigger_Muon_Test_HLTMuonRateAnalyzerWithWeight_H
#define HLTrigger_Muon_Test_HLTMuonRateAnalyzerWithWeight_H

/** \class HLTMuonRateAnalyzerWithWeight
 *  Get L1/HLT efficiency/rate plots
 *
 *  \author J. Alcaraz
 */

// Base Class Headers
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/InputTag.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include <vector>

class TFile;
class TH1F;

class HLTMuonRateAnalyzerWithWeight: public edm::EDAnalyzer {
public:
  /// Constructor
  HLTMuonRateAnalyzerWithWeight(const edm::ParameterSet& pset);

  /// Destructor
  virtual ~HLTMuonRateAnalyzerWithWeight();

  // Operations

  void analyze(const edm::Event & event, const edm::EventSetup& eventSetup);

  virtual void beginJob(const edm::EventSetup& eventSetup) ;
  virtual void endJob() ;
  virtual bool isbc(HepMC::GenEvent evt) ;
  virtual double parentWeight(HepMC::GenEvent evt) ;

private:
  // Input from cfg file
  edm::InputTag theGenLabel;
  edm::InputTag theL1CollectionLabel;
  std::vector<edm::InputTag> theHLTCollectionLabels;
  double theL1ReferenceThreshold;
  std::vector<double> theNSigmas;
  unsigned int theNumberOfObjects;
  double theLuminosity;
  double thePtMin;
  double thePtMax;
  double theIntegratedLumi;
  int type;
  unsigned int theNbins;
  std::string theRootFileName;

  // The output Root file
  TFile* theFile;

  // Histograms
  TH1F* hBCL1eff;
  TH1F* hNumEvents;
  TH1F* hBCL1rate;
  std::vector <TH1F*> hBCHLTeff;
  std::vector <TH1F*> hBCHLTrate;
  TH1F* hLightL1eff;
  TH1F* hLightL1rate;
  std::vector <TH1F*> hLightHLTeff;
  std::vector <TH1F*> hLightHLTrate;

  // Counter of events (weighted in general)
  double theNumberOfLightEvents;
  double theNumberOfBCEvents;
  
};
#endif