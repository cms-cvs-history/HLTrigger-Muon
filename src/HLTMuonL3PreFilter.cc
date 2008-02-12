/** \class HLTMuonL3PreFilter
 *
 * See header file for documentation
 *
 *  \author J. Alcaraz
 *
 */

#include "HLTrigger/Muon/interface/HLTMuonL3PreFilter.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/RefToBase.h"

#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"
#include "DataFormats/HLTReco/interface/TriggerRefsCollections.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidateFwd.h"
#include "DataFormats/MuonReco/interface/MuonTrackLinks.h"

//
// constructors and destructor
//
using namespace std;
using namespace edm;
using namespace reco;
using namespace trigger;

HLTMuonL3PreFilter::HLTMuonL3PreFilter(const ParameterSet& iConfig) :
   candTag_   (iConfig.getParameter<InputTag > ("CandTag")),
   linksTag_   (iConfig.getParameter<InputTag > ("LinksTag")),
   previousCandTag_   (iConfig.getParameter<InputTag > ("PreviousCandTag")),
   min_N_     (iConfig.getParameter<int> ("MinN")),
   max_Eta_   (iConfig.getParameter<double> ("MaxEta")),
   min_Nhits_ (iConfig.getParameter<int> ("MinNhits")),
   max_Dr_    (iConfig.getParameter<double> ("MaxDr")),
   max_Dz_    (iConfig.getParameter<double> ("MaxDz")),
   min_Pt_    (iConfig.getParameter<double> ("MinPt")),
   nsigma_Pt_  (iConfig.getParameter<double> ("NSigmaPt")) 
{

   LogDebug("HLTMuonL3PreFilter")
      << " CandTag/MinN/MaxEta/MinNhits/MaxDr/MaxDz/MinPt/NSigmaPt : " 
      << candTag_.encode()
      << " " << min_N_ 
      << " " << max_Eta_
      << " " << min_Nhits_
      << " " << max_Dr_
      << " " << max_Dz_
      << " " << min_Pt_
      << " " << nsigma_Pt_;

   //register your products
   produces<TriggerFilterObjectWithRefs>();
}

HLTMuonL3PreFilter::~HLTMuonL3PreFilter()
{
}

//
// member functions
//

// ------------ method called to produce the data  ------------
bool
HLTMuonL3PreFilter::filter(Event& iEvent, const EventSetup& iSetup)
{

   // All HLT filters must create and fill an HLT filter object,
   // recording any reconstructed physics objects satisfying (or not)
   // this HLT filter, and place it in the Event.

   // The filter object
   auto_ptr<TriggerFilterObjectWithRefs>
     filterproduct (new TriggerFilterObjectWithRefs(path(),module()));
   // Ref to Candidate object to be recorded in filter object
   RecoChargedCandidateRef ref;

   // get hold of trks
   Handle<RecoChargedCandidateCollection> mucands;
   iEvent.getByLabel (candTag_,mucands);
   Handle<TriggerFilterObjectWithRefs> previousLevelCands;
   iEvent.getByLabel (previousCandTag_,previousLevelCands);

   Handle<MuonTrackLinksCollection> mulinks; 
   vector<RecoChargedCandidateRef> vl2cands;

   // needed to compare to L2
   iEvent.getByLabel (linksTag_,mulinks);
   previousLevelCands->getObjects(TriggerMuon,vl2cands);

   // look at all mucands,  check cuts and add to filter object
   int n = 0;
   RecoChargedCandidateCollection::const_iterator cand;
   for (cand=mucands->begin(); cand!=mucands->end(); cand++) {
      TrackRef tk = cand->get<TrackRef>();

      LogDebug("HLTMuonL3PreFilter") << " Muon in loop, q*pt= " << tk->charge()*tk->pt() << ", eta= " << tk->eta() << ", hits= " << tk->numberOfValidHits() << ", d0= " << tk->d0() << ", dz= " << tk->dz();

      // find the L2 Track corresponding to the L3 Track
      if(!triggeredByLevel2(tk,mulinks,vl2cands)) continue;
      // eta cut


      if (fabs(tk->eta())>max_Eta_) continue;

      // cut on number of hits
      if (tk->numberOfValidHits()<min_Nhits_) continue;

      //dr cut
      if (fabs(tk->d0())>max_Dr_) continue;

      //dz cut
      if (fabs(tk->dz())>max_Dz_) continue;

      // Pt threshold cut
      double pt = tk->pt();
      double err0 = tk->error(0);
      double abspar0 = fabs(tk->parameter(0));
      double ptLx = pt;
      // convert 50% efficiency threshold to 90% efficiency threshold
      if (abspar0>0) ptLx += nsigma_Pt_*err0/abspar0*pt;
      LogTrace("HLTMuonL3PreFilter") << " ...Muon in loop, pt= "
            << pt << ", ptLx= " << ptLx;
      if (ptLx<min_Pt_) continue;

      n++;
      ref= RecoChargedCandidateRef(Ref<RecoChargedCandidateCollection>
                     (mucands,distance(mucands->begin(),cand)));
      filterproduct->addObject(TriggerMuon,ref);
   }

   vector<RecoChargedCandidateRef> vref;
   filterproduct->getObjects(TriggerMuon,vref);
   for (unsigned int i=0; i<vref.size(); i++ ) {
     RecoChargedCandidateRef candref =  RecoChargedCandidateRef(vref[i]);
     TrackRef tk = candref->get<TrackRef>();
     LogDebug("HLTMuonL3PreFilter")
       << " Track passing filter: pt= " << tk->pt() << ", eta: " 
       << tk->eta();
   }
   
   // filter decision
   const bool accept (n >= min_N_);
   
   // put filter object into the Event
   iEvent.put(filterproduct);
   
   LogDebug("HLTMuonL3PreFilter") << " >>>>> Result of HLTMuonL3PreFilter is " << accept << ", number of muons passing thresholds= " << n; 

   return accept;
}
bool
HLTMuonL3PreFilter::triggeredByLevel2(TrackRef& tk,Handle<MuonTrackLinksCollection> &mulinks,vector<RecoChargedCandidateRef>& vcands)
{
  bool ok=false;
  TrackRef staTrack;
  MuonTrackLinksCollection::const_iterator l3muon;
  for ( l3muon=mulinks->begin(); l3muon != mulinks->end();++l3muon){
    if ( l3muon->globalTrack() == tk ) {
      staTrack= l3muon->standAloneTrack();
      LogDebug("HLTMuonL3PreFilter") << "Found StaTrack corresponding to L3";
      break;
    }
  }  
  for (unsigned int i=0; i<vcands.size(); i++) {
    RecoChargedCandidateRef candref =  RecoChargedCandidateRef(vcands[i]);
    TrackRef tk = candref->get<TrackRef>();
    if ( tk == staTrack ) {
      ok=true;
      LogDebug("HLTMuonL3PreFilter") << "The L2 track triggered";
      break;
    }
  }
  return ok;
}
