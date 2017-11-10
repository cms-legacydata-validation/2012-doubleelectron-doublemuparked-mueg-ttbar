// -*- C++ -*-
//
// Package:    Analyzer
// Class:      Analyzer
// 
/**\class Analyzer Analyzer.cc ttbar/Analyzer/src/Analyzer.cc

 Description: ttbar ntuple production

 Implementation:
     for Open Data 2011
*/


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

//------ EXTRA HEADER FILES--------------------//
#include "math.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Common/interface/Ref.h"

// for tracking information
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"

// for vertex information 
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

// for muons
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/MuonReco/interface/MuonIsolation.h"

// for electrons
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"

// for jets
#include <DataFormats/JetReco/interface/PFJet.h>
#include <DataFormats/BTauReco/interface/JetTag.h>
#include "JetMETCorrections/Objects/interface/JetCorrector.h"

// for MET
#include "DataFormats/METReco/interface/PFMET.h"

//for beamspot information
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

// triggers
#include "DataFormats/Common/interface/TriggerResults.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

// for MC generator level
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

// ROOT
#include <TLorentzVector.h>
#include <TFile.h>
#include <TTree.h>

#include <TTree.h>
#include <TDirectory.h>

//
// class declaration
//
class Analyzer : public edm::EDAnalyzer {
   public:
      explicit Analyzer(const edm::ParameterSet&);
      ~Analyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      virtual void beginRun(edm::Run const&, edm::EventSetup const&);
      virtual void endRun(edm::Run const&, edm::EventSetup const&);
      virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
      virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
      
      // user routines (detailed description given with the method implementations)
      int SelectEvent(const edm::Event& iEvent);
      int SelectMu(const edm::Handle<reco::MuonCollection>& muons, const reco::VertexCollection::const_iterator& pv);
      int SelectEl(const edm::Handle<reco::GsfElectronCollection>& electrons, const reco::VertexCollection::const_iterator& pv);
      int SelectJet(const edm::Handle<reco::PFJetCollection>& jets, const reco::JetTagCollection& bTags, const edm::Event& iEvent, const edm::EventSetup& iSetup);
      int SelectMET(const edm::Handle<edm::View<reco::PFMET> >& pfmets);
      reco::PFJetCollection::const_iterator MatchJet(const edm::Handle<reco::PFJetCollection>& jets, const edm::RefToBase<reco::Jet>& jettag, double& diff, double& nextdiff);
      void FindTriggerBits(const HLTConfigProvider& trigConf);
      void SelectTriggerBits(const edm::Handle<edm::TriggerResults>& HLTR);
      void PrintTriggerBits();
      int SelectPrimaryVertex(const edm::Handle<reco::VertexCollection>& primVertex);
      const reco::Candidate* GetFinalState(const reco::Candidate* particle, const int id);
      void FillFourMomentum(const reco::Candidate* particle, float* p);
      void SelectMCGen(const edm::Handle<reco::GenParticleCollection>& genParticles);
      void InitBranchVars();

      // input tags
      edm::InputTag _inputTagMuons;
      edm::InputTag _inputTagElectrons;
      edm::InputTag _inputTagBtags;
      edm::InputTag _inputTagJets;
      edm::InputTag _inputTagMet;
      edm::InputTag _inputTagTriggerResults;
      edm::InputTag _inputTagPrimaryVertex;
      edm::InputTag _inputTagMCgen;

      // jet correction label
      std::string mJetCorr;

      // general flags and variables
      int _flagMC;
      int _flagRECO;
      int _flagGEN;
      int _nevents;
      int _neventsSelected;
      int _signLeptonP;
      int _signLeptonM;
      
      // storage
      TFile* _file;
      TTree* _tree;
      
      // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      // >>>>>>>>>>>>>>>> event variables >>>>>>>>>>>>>>>>>>>>>>>
      // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      // (their description given when tree branches are created)
      // event
      int _evRunNumber;
      int _evEventNumber;
      // muons
      static const int _maxNmu = 10;
      int _Nmu;
      float _muPt[_maxNmu];
      float _muEta[_maxNmu];
      float _muPhi[_maxNmu];
      float _muIso03[_maxNmu];
      float _muIso04[_maxNmu];
      int _muHitsValid[_maxNmu];
      int _muHitsPixel[_maxNmu];
      float _muDistPV0[_maxNmu];
      float _muDistPVz[_maxNmu];
      float _muTrackChi2NDOF[_maxNmu];
      // electrons
      static const int _maxNel = 10;
      int _Nel;
      float _elPt[_maxNel];
      float _elEta[_maxNel];
      float _elPhi[_maxNel];
      float _elIso03[_maxNel];
      float _elIso04[_maxNel];
      int _elConvFlag[_maxNel];
      float _elConvDist[_maxNel];
      float _elConvDcot[_maxNel];
      float _elMissHits[_maxNel];
      float _elDistPV0[_maxNel];
      float _elDistPVz[_maxNel];
      // jets
      static const int _maxNjet = 25;
      int _Njet;
      float _jetPt[_maxNjet];
      float _jetEta[_maxNjet];
      float _jetPhi[_maxNjet];
      float _jetMass[_maxNjet];
      float _jetMuEn[_maxNjet];
      float _jetElEn[_maxNjet];
      float _jetBTagDiscr[_maxNjet];
      float _jetBTagMatchDiff1[_maxNjet];
      float _jetBTagMatchDiff2[_maxNjet];
      // MET
      float _metPx;
      float _metPy;
      // triggers
      int _triggers;
      std::vector<std::vector<int> > _vecTriggerBits;
      std::vector<std::string> _vecTriggerNames;
      // primary vertex
      int _Npv;
      int _pvNDOF;
      float _pvZ;
      float _pvRho;
      // MC generated info
      int _mcNTTbar;
      int _mcNTWb;
      int _mcNTtbarDilepton;
      int _mcNTtbarDileptonEE;
      int _mcNTtbarDileptonMuMu;
      int _mcNTtbarDileptonEMu;
      int _mcEventType;
      // generator level four vectors
      float _mcT[4];
      float _mcTbar[4];
      float _mcWp[4];
      float _mcWm[4];
      float _mcB[4];
      float _mcBbar[4];
      float _mcLp[4];
      float _mcNu[4];
      float _mcLm[4];
      float _mcNubar[4];
};

//
// constants (particle masses)
//
double _massMu = 0.105658;
double _massEl = 0.000511;

//
// constructor
//
Analyzer::Analyzer(const edm::ParameterSet& iConfig)
{
  // for proper log files writing (immediate output)
  setbuf(stdout, NULL);
  
  // input tags
  _inputTagMuons = edm::InputTag("muons");
  _inputTagElectrons = edm::InputTag("gsfElectrons");
  _inputTagBtags = edm::InputTag("combinedSecondaryVertexBJetTags");
  _inputTagJets = edm::InputTag("ak5PFJets");
  _inputTagMet = edm::InputTag("pfMet");
  _inputTagTriggerResults = edm::InputTag("TriggerResults", "", "HLT");
  _inputTagPrimaryVertex = edm::InputTag("offlinePrimaryVerticesWithBS");
  _inputTagMCgen = edm::InputTag("genParticles");
  
  // jet correction label
  mJetCorr = "ak5PFL1FastL2L3Residual";

  // read configuration parameters
  _flagMC = iConfig.getParameter<int>("mc"); // true for MC, false for data
  _flagRECO = iConfig.getParameter<int>("reco"); // if true, RECO level processed
  _flagGEN = iConfig.getParameter<int>("gen"); // if true, generator level processed (works only for MC)
  _nevents = 0; // number of processed events
  _neventsSelected = 0; // number of selected events
  std::string fileout = iConfig.getParameter<std::string>("outFile"); // output file name
  _file = new TFile(fileout.c_str(), "recreate"); // output file
  _tree = new TTree("tree", "ttbar"); // output tree

  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // >>>>>>> tree branches >>>>>>>>>>>>
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  //
  // event
  _tree->Branch("evRunNumber", &_evRunNumber, "evRunNumber/I"); // run number
  _tree->Branch("evEventNumber", &_evEventNumber, "evEventNumber/I"); // event number

  if(_flagRECO)
  {
    // muons
    _tree->Branch("Nmu", &_Nmu, "Nmu/I"); // number of muons 
    _tree->Branch("muPt", _muPt, "muPt[Nmu]/F"); // muon pT
    _tree->Branch("muEta", _muEta, "muEta[Nmu]/F"); // muon pseudorapidity
    _tree->Branch("muPhi", _muPhi, "muPhi[Nmu]/F"); // muon phi
    _tree->Branch("muIso03", _muIso03, "muIso03[Nmu]/F"); // muon isolation, delta_R=0.3
    _tree->Branch("muIso04", _muIso04, "muIso04[Nmu]/F"); // muon isolation, delta_R=0.4
    _tree->Branch("muHitsValid", _muHitsValid, "muHitsValid[Nmu]/I"); // muon valid hits number
    _tree->Branch("muHitsPixel", _muHitsPixel, "muHitsPixel[Nmu]/I"); // muon pixel hits number
    _tree->Branch("muDistPV0", _muDistPV0, "muDistPV0[Nmu]/F"); // muon distance to the primary vertex (projection on transverse plane)
    _tree->Branch("muDistPVz", _muDistPVz, "muDistPVz[Nmu]/F"); // muon distance to the primary vertex (z projection)
    _tree->Branch("muTrackChi2NDOF", _muTrackChi2NDOF, "muTrackChi2NDOF[Nmu]/F"); // muon track number of degrees of freedom
    // electrons
    _tree->Branch("Nel", &_Nel, "Nel/I"); // number of electrons
    _tree->Branch("elPt", _elPt, "elPt[Nel]/F"); // electron pT
    _tree->Branch("elEta", _elEta, "elEta[Nel]/F"); // electron pseudorapidity
    _tree->Branch("elPhi", _elPhi, "elPhi[Nel]/F"); // electron phi
    _tree->Branch("elIso03", _elIso03, "elIso03[Nel]/F"); // electron isolation, delta_R=0.3
    _tree->Branch("elIso04", _elIso04, "elIso04[Nel]/F"); // electron isolation, delta_R=0.4
    _tree->Branch("elConvFlag", _elConvFlag, "elConvFlag[Nel]/I"); // electron (not used) electron conversion flag
    _tree->Branch("elConvDist", _elConvDist, "elConvDist[Nel]/F"); // electron (not used) electron conversion distance
    _tree->Branch("elConvDcot", _elConvDcot, "elConvDcot[Nel]/F"); // electron (not used) electron conversion cotangent
    _tree->Branch("elMissHits", _elMissHits, "elMissHits[Nel]/F"); // electron missing hits number 
    // jets
    _tree->Branch("Njet", &_Njet, "Njet/I"); // number of jets
    _tree->Branch("jetPt", _jetPt, "jetPt[Njet]/F"); // jet pT
    _tree->Branch("jetEta", _jetEta, "jetEta[Njet]/F"); // jet pseudorapidity
    _tree->Branch("jetPhi", _jetPhi, "jetPhi[Njet]/F"); // jet phi
    _tree->Branch("jetMass", _jetMass, "jetMass[Njet]/F"); // jet mass
    _tree->Branch("jetMuEn", _jetMuEn, "jetMuEn[Njet]/F"); // jet muon energy
    _tree->Branch("jetElEn", _jetElEn, "jetElEn[Njet]/F"); // jet electron energy
    _tree->Branch("jetBTagDiscr", _jetBTagDiscr, "jetBTagDiscr[Njet]/F"); // jet b-tagging discriminant (Combined Secondary Vertex, CSV)
    _tree->Branch("jetBTagMatchDiff1", _jetBTagMatchDiff1, "jetBTagMatchDiff1[Njet]/F"); // (not used, for checks) jet b-tagging: eta-phi distance to the closest matched jet
    _tree->Branch("jetBTagMatchDiff2", _jetBTagMatchDiff2, "jetBTagMatchDiff2[Njet]/F"); // (not used, for checks) jet b-tagging: eta-phi distance to the second closest matched jet
    // MET
    _tree->Branch("metPx", &_metPx, "metPx/F"); // missing transverse energy x component
    _tree->Branch("metPy", &_metPy, "metPy/F"); // missing transverse energy y component
    // triggers
    _tree->Branch("Triggers", &_triggers, "Triggers/I"); // trigger bits (see trigger names below)
    // mumu triggers
    _vecTriggerNames.push_back("HLT_Mu17_Mu8_v");
    _vecTriggerNames.push_back("HLT_Mu17_TkMu8_v");
    //_vecTriggerNames.push_back("HLT_Mu17_Mu8_v17");
    //_vecTriggerNames.push_back("HLT_Mu17_TkMu8_v10");
    // ee triggers
    _vecTriggerNames.push_back("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL");
    _vecTriggerNames.push_back("Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v");
    // emu triggers
    _vecTriggerNames.push_back("HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v");
    _vecTriggerNames.push_back("HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v");
    // primary vertex
    _tree->Branch("Npv", &_Npv, "Npv/I"); // total number of primary vertices
    _tree->Branch("pvNDOF", &_pvNDOF, "pvNDOF/I"); // number of degrees of freedom of the primary vertex
    _tree->Branch("pvZ", &_pvZ, "pvZ/F"); // z component of the primary vertex
    _tree->Branch("pvRho", &_pvRho, "pvRho/F"); // rho of the primary vertex (projection on transverse plane)
  }

  // MC generated info
  if(_flagGEN)
  {
    _mcNTTbar = 0; // number of ttbar pairs
    _mcNTWb = 0; // number of t,tbar->Wb decays 
    _mcNTtbarDilepton = 0; // number of dielectron ttbar decays
    _mcNTtbarDileptonEE = 0; // number of dielectron ttbar decays
    _mcNTtbarDileptonMuMu = 0; // number of dielectron ttbar decays
    _mcNTtbarDileptonEMu = 0; // number of electron-muon ttbar decayse
    // MC generated info to store
    _tree->Branch("mcEventType", &_mcEventType, "mcEventType/I"); // MC generator level event type: 1 ttbar decay into ee, 2 ttbar decay into mumu, 3 ttbar decay into emu, 0 anything else
    _tree->Branch("mcT", _mcT, "mcT[4]/F"); // generator level top four vector
    _tree->Branch("mcTbar", _mcTbar, "mcTbar[4]/F"); // generator level antitop four vector
    _tree->Branch("mcWp", _mcWp, "mcWp[4]/F"); // generator level W+ four vector
    _tree->Branch("mcWm", _mcWm, "mcWm[4]/F"); // generator level W- four vector
    _tree->Branch("mcB", _mcB, "mcB[4]/F"); // generator level top b vector
    _tree->Branch("mcBbar", _mcBbar, "mcBbar[4]/F"); // generator level bbar four vector
    _tree->Branch("mcLp", _mcLp, "mcLp[4]/F"); // generator level top l+ vector
    _tree->Branch("mcNu", _mcNu, "mcNu[4]/F"); // generator level top neutrino vector
    _tree->Branch("mcLm", _mcLm, "mcLm[4]/F"); // generator level top l- vector
    _tree->Branch("mcNubar", _mcNubar, "mcNubar[4]/F"); // generator level top antineutrino vector
  }
}


// destructor
Analyzer::~Analyzer()
{
   // close files, deallocate resources etc.
  _file->cd();
  _tree->Write();
  _file->Close();

  if(_flagGEN)
  {
    // MC generator level info printout
    printf("%25s = %10d\n", "mcTTbar", _mcNTTbar);
    printf("%25s = %10d(%5.2f%%)\n", "_mcNTWb", _mcNTWb, 100. * _mcNTWb / _mcNTTbar / 2);
    printf("%25s = %10d(%5.2f%%)\n", "_mcNTtbarDilepton", _mcNTtbarDilepton, 100. * _mcNTtbarDilepton / _mcNTTbar);
    printf("%25s = %10d(%5.2f%%)\n", "_mcNTtbarDileptonEE", _mcNTtbarDileptonEE, 100. * _mcNTtbarDileptonEE / _mcNTTbar);
    printf("%25s = %10d(%5.2f%%)\n", "_mcNTtbarDileptonMuMu", _mcNTtbarDileptonMuMu, 100. * _mcNTtbarDileptonMuMu / _mcNTTbar);
    printf("%25s = %10d(%5.2f%%)\n", "_mcNTtbarDileptonEMu", _mcNTtbarDileptonEMu, 100. * _mcNTtbarDileptonEMu / _mcNTTbar);
  }

  // print total number of processed and selected events
  printf("Processed %d events, selected %d\n", _nevents, _neventsSelected);
}


//
// member functions
//

// initialise event variables with needed default (zero) values; called in the beginning of each event
void Analyzer::InitBranchVars()
{
  _evRunNumber = 0;
  _evEventNumber = 0;
  _Nmu = 0;
  _Nel = 0;
  _Njet = 0;
  _metPx = 0;
  _metPy = 0;
  _triggers = 0;
  _Npv= 0;
  _pvNDOF = 0;
  _pvZ = 0;
  _pvRho = 0;
  _mcEventType = 0;
}

// Store event info (fill corresponding tree variables)
int Analyzer::SelectEvent(const edm::Event& iEvent)
{
  _evRunNumber = iEvent.id().run();
  _evEventNumber = iEvent.id().event();
  return 0;
}

// Missing transverse energy (MET) selection
int Analyzer::SelectMET(const edm::Handle<edm::View<reco::PFMET> >& pfmets)
{
  _metPx = (pfmets->front()).px();
  _metPy = (pfmets->front()).py();
  return 0;
}

// muon selection
int Analyzer::SelectMu(const edm::Handle<reco::MuonCollection>& muons, const reco::VertexCollection::const_iterator& pv)
{
  _Nmu = 0;
  // loop over muons
  for (reco::MuonCollection::const_iterator it = muons->begin(); it != muons->end(); it++)
  {
    if(_Nmu == _maxNmu)
    {
      printf("Maximum number of muons %d reached, skipping the rest\n", _maxNmu);
      return 0;
    }
    // selection: pT > 20 GeV, |eta| < 2.4
    if(it->pt() < 20)
      continue;
    if(TMath::Abs(it->eta()) > 2.4)
      continue;
    if((it->globalTrack()).isNull())
      continue;
    // fill isolation variables
    const reco::MuonPFIsolation& iso03 = it->pfIsolationR03();
    _muIso03[_Nmu] = ( iso03.sumChargedParticlePt + iso03.sumNeutralHadronEt ) / it->pt();
    const reco::MuonPFIsolation& iso04 = it->pfIsolationR04();
    _muIso04[_Nmu] = ( iso04.sumChargedParticlePt + iso04.sumNeutralHadronEt ) / it->pt();
    if(_muIso03[_Nmu] > 0.15)
      continue;
    // fill number of hits variables
    _muHitsValid[_Nmu] = 0;
    _muHitsPixel[_Nmu] = 0;
    const reco::HitPattern& p = (it->globalTrack())->hitPattern();
    for (int i = 0; i < p.numberOfHits(); i++) 
    {
      uint32_t hit = p.getHitPattern(i);
      if (p.validHitFilter(hit) && p.pixelHitFilter(hit))
        _muHitsPixel[_Nmu]++;
      if (p.validHitFilter(hit))
        _muHitsValid[_Nmu]++;
    }
    // fill three momentum (pT, eta, phi)
    _muPt[_Nmu] = it->pt() * it->charge();
    _muEta[_Nmu] = it->eta();
    _muPhi[_Nmu] = it->phi();
    // fill chi2/ndof
    _muTrackChi2NDOF[_Nmu] = it->globalTrack()->chi2() / it->globalTrack()->ndof();
    // fill distance to primary vertex
    _muDistPV0[_Nmu] = TMath::Sqrt(TMath::Power(pv->x() - it->globalTrack()->vx(), 2.0) + TMath::Power(pv->y() - it->globalTrack()->vy(), 2.0));
    _muDistPVz[_Nmu] = TMath::Abs(pv->z() - it->globalTrack()->vz());
    // store muon
    _Nmu++;
    // determine muon sign (in the end the event will be stored only there are opposite signed leptons)
    if(it->charge() == +1)
        _signLeptonP = 1;
    if(it->charge() == -1)
        _signLeptonM = 1;
  }
  return 0;
}

// electron selection
int Analyzer::SelectEl(const edm::Handle<reco::GsfElectronCollection>& electrons, const reco::VertexCollection::const_iterator& pv)
{
  _Nel = 0;
  // loop over electrons
  for (reco::GsfElectronCollection::const_iterator it = electrons->begin(); it != electrons->end(); it++)
  {
    if(_Nel == _maxNel)
    {
      printf("Maximum number of electrons %d reached, skipping the rest\n", _maxNel);
      return 0;
    }
    // selection: pT > 20 GeV, |eta| < 2.4
    if(it->pt() < 20)
      continue;
    if(TMath::Abs(it->eta()) > 2.4)
      continue;
    // fill isolation
    _elIso03[_Nel] = (it->dr03TkSumPt() + it->dr03EcalRecHitSumEt() + it->dr03HcalTowerSumEt()) / it->pt();
    _elIso04[_Nel] = (it->dr04TkSumPt() + it->dr04EcalRecHitSumEt() + it->dr04HcalTowerSumEt()) / it->pt();
    if(_elIso03[_Nel] > 0.15)
      continue;
    // fill three momentum (pT, eta, phi)
    _elPt[_Nel] = it->pt() * it->charge();
    _elEta[_Nel] = it->eta();
    _elPhi[_Nel] = it->phi();
    // fill conversion variables (however they are not used)
    _elConvFlag[_Nel] = it->convFlags();
    _elConvDist[_Nel] = it->convDist();
    _elConvDcot[_Nel] = it->convDcot();
    _elMissHits[_Nel] = ((it->gsfTrack())->trackerExpectedHitsInner()).numberOfHits();
    // distance to primary vertex
    //_elDistPV0[_Nel] = TMath::Sqrt(TMath::Power(pv->x() - it->vx(), 2.0) + TMath::Power(pv->y() - it->vy(), 2.0));
    //_elDistPVz[_Nel] = TMath::Abs(pv->z() - it->vz());
    // store electron
    _Nel++;
    // determine electron sign (in the end the event will be stored only there are opposite signed leptons)
    if(it->charge() == +1)
        _signLeptonP = 1;
    if(it->charge() == -1)
        _signLeptonM = 1;
  }
  return 0;
}

// jet matching (for b-tagging)
reco::PFJetCollection::const_iterator Analyzer::MatchJet(const edm::Handle<reco::PFJetCollection>& jets, const edm::RefToBase<reco::Jet>& jettag, double& diff, double& nextdiff)
{
  // initialise eta-phi difference with very large initial value
  diff = nextdiff = 1000.0;
  // iterator to the best matched jet
  reco::PFJetCollection::const_iterator bestit = jets->begin();
  // loop over jet collection
  for(reco::PFJetCollection::const_iterator it = jets->begin(); it != jets->end(); it++)
  {
    // calculate et-phi difference to the current jet
    double ldiff = TMath::Power(it->eta() - jettag->eta(), 2.0) + TMath::Power(it->phi() - jettag->phi(), 2.0);
    if(ldiff < nextdiff && ldiff > diff)
      nextdiff = ldiff;
    // skip this jet, if the difference is larger than the minimum difference found already
    if(ldiff > diff)
      continue;
    // if this jet is not skipped, update needed variables with the current eta-phi difference
    nextdiff = diff;
    diff = ldiff;
    bestit = it;
  }
  // return iterator to the bset matched jet
  return bestit;
}

// jet selection
int Analyzer::SelectJet(const edm::Handle<reco::PFJetCollection>& jets, const reco::JetTagCollection& bTags, const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  _Njet = 0;
  // Loop over jets and study b-tagging info
  std::vector<std::pair<reco::PFJetCollection::const_iterator, std::vector<double> > > vItBTagMatched;
  for (unsigned int i = 0; i != bTags.size(); ++i) 
  {
    // b-tagging discriminator
    double discriminator = bTags[i].second;
    if(discriminator < 0.2)
      continue;
    // these are variables for debugging purpose: study eta-phi difference when matching jets
    std::vector<double> btag(3);
    btag[1] = btag[2] = -1.0;
    reco::PFJetCollection::const_iterator itBTagMatched = MatchJet(jets, bTags[i].first, btag[1], btag[2]);
    //printf("%.3f  %.3f\n", btag[1], btag[2]);
    btag[0] = discriminator;
    vItBTagMatched.push_back(std::pair<reco::PFJetCollection::const_iterator, std::vector<double> >(itBTagMatched, btag));
  }
  
  // Load jet energy correction service
  const JetCorrector* corrector = JetCorrector::getJetCorrector(mJetCorr, iSetup);

  // Loop over needed jets
  int status = 1;
  for (reco::PFJetCollection::const_iterator it = jets->begin(); it != jets->end(); it++)
  {
    if(_Njet == _maxNjet)
    {
      printf("Maximum number of jets %d reached, skipping the rest\n", _maxNjet);
      return 0;
    }
    // Apply jet energy correction (JEC)
    double jec = corrector->correction(*it, iEvent, iSetup);
    // copy original (uncorrected) jet;
    reco::PFJet corjet = *it;
    // apply JEC
    corjet.scaleEnergy(jec);
    // select jet: pT > 30 GeV, |eta| < 2.4
    if(corjet.pt() < 30)
      continue;
    if(TMath::Abs(corjet.eta()) > 2.4)
      continue;
    // fill jet four momentum (pT, eta, phi, mass)
    _jetPt[_Njet] = corjet.pt();
    _jetEta[_Njet] = corjet.eta();
    _jetPhi[_Njet] = corjet.phi();
    _jetMass[_Njet] = corjet.mass();
    // fill jet muon and electron energy fractions
    _jetMuEn[_Njet] = corjet.muonEnergy();
    _jetElEn[_Njet] = corjet.electronEnergy();
    // find and fill b-tagging info
    _jetBTagDiscr[_Njet] = _jetBTagMatchDiff1[_Njet] = _jetBTagMatchDiff2[_Njet] = -1.0;
    for(std::vector<std::pair<reco::PFJetCollection::const_iterator, std::vector<double> > >::const_iterator ittag = vItBTagMatched.begin(); ittag != vItBTagMatched.end(); ittag++)
    {
      if(it == ittag->first)
      {
        status = 0;
        _jetBTagDiscr[_Njet] = (ittag->second)[0];
        _jetBTagMatchDiff1[_Njet] = (ittag->second)[1];
        _jetBTagMatchDiff2[_Njet] = (ittag->second)[2];
        break;
      }
    }
    _Njet++;
  }
  // if there are less then two jets selected, this can be skipped (return status 1)
  if(_Njet < 2)
    status = 1;
  return status;
}

// returns vector of integers which are trigger bits needed in the analysis
// (called in the beginning of each run)
void Analyzer::FindTriggerBits(const HLTConfigProvider& trigConf)
{
  // reset containers
  _vecTriggerBits.clear();
  _vecTriggerBits.resize(_vecTriggerNames.size());
  std::vector<std::string> trigNames;
  trigNames = trigConf.triggerNames();
  // for interesting trigger names find corresponding trigger bits
  for(unsigned int i = 0; i < trigNames.size(); i++)
  {
    std::string currentName = trigConf.triggerNames()[i];
    //printf("%5d  %s\n", i, currentName.c_str());
    for(unsigned int n = 0; n < _vecTriggerNames.size(); n++)
    {
      if(currentName.find(_vecTriggerNames[n]) != std::string::npos)
      {
        _vecTriggerBits[n].push_back(i);
      }
    }
  }
}

// print found trigger indices
// (for debugging purpose)
void Analyzer::PrintTriggerBits()
{
  printf("********* Trigger Bits: **********\n");
  for(unsigned int n = 0; n < _vecTriggerNames.size(); n++)
  {
    printf("%s: ", _vecTriggerNames[n].c_str());
    for(unsigned int i = 0; i < _vecTriggerBits[n].size(); i++)
      printf(" %d ", _vecTriggerBits[n][i]);
    printf("\n");
  }
}

// fill trigger bits
void Analyzer::SelectTriggerBits(const edm::Handle<edm::TriggerResults>& HLTR)
{
  for(unsigned int i = 0; i < _vecTriggerBits.size(); i++)
  {
    int status = 0;
    for(unsigned int j = 0; j < _vecTriggerBits[i].size(); j++)
    {
      status = status || HLTR->accept(_vecTriggerBits[i][j]);
    }
    //printf("TRIGGER:  %d %s\n", status, _vecTriggerNames[i].c_str());
    // set ith bit of _triggers integer to the current trigger bit status
    _triggers ^= (-status ^ _triggers) & (1 << i);
  }
  //printf("*************\n");
}

// select primary vertex
int Analyzer::SelectPrimaryVertex(const edm::Handle<reco::VertexCollection>& primVertex)
{
  // if no primary vertices in the event, return false status
  if(primVertex->size() == 0)
    return false;
  // take the first primary vertex
  reco::VertexCollection::const_iterator pv = primVertex->begin();
  // fill z and rho (projection on transverse plane)
  _pvZ = pv->z();
  _pvRho = TMath::Sqrt(TMath::Power(pv->x(), 2.0) + TMath::Power(pv->y(), 2.0));
  // fill number of primary veritces
  _Npv = primVertex->size();
  // fill number of degrees of freedom
  _pvNDOF = pv->ndof();
  // return true status
  return true;
}

// get final-state stable generator level particle with required id
// (if not found, return NULL pointer)
const reco::Candidate* Analyzer::GetFinalState(const reco::Candidate* particle, const int id)
{
  // loop over daughters
  for(unsigned int i = 0; i < particle->numberOfDaughters(); i++)
  {
    const reco::Candidate* daughter = particle->daughter(i);
    // if this daughter has required id, return its pointer
    if(daughter->pdgId() == id && daughter->status() == 1)
      return daughter;
    // otherwise call itself recursively
    const reco::Candidate* result = GetFinalState(daughter, id);
    // return the result of the recursive call
    if(result)
      return result;
  }
  // if gets here, there are no daughter with required id: return NULL pointer
  return NULL;
}

// fill 4-momentum (p) with provided particle pointer
void Analyzer::FillFourMomentum(const reco::Candidate* particle, float* p)
{
  // if NULL pointer provided, initialise with default (zero)
  if(particle == NULL)
  {
    p[0] = p[1] = p[2] = p[3] = 0.0;
    return;
  }
  
  p[0] = particle->px();
  p[1] = particle->py();
  p[2] = particle->pz();
  p[3] = particle->mass();
}

// select MC generator level information
// (analysis specific ttbar dileptonic decay)
void Analyzer::SelectMCGen(const edm::Handle<reco::GenParticleCollection>& genParticles)
{
  // initialise all particle pointers with NULL
  const reco::Candidate* genT = NULL;
  const reco::Candidate* genTbar = NULL;
  const reco::Candidate* genB = NULL;
  const reco::Candidate* genBbar = NULL;
  const reco::Candidate* genWp = NULL;
  const reco::Candidate* genWm = NULL;
  const reco::Candidate* genLp = NULL;
  const reco::Candidate* genNu = NULL;
  const reco::Candidate* genLm = NULL;
  const reco::Candidate* genNubar = NULL;
  // loop over generated particeles
  for(unsigned int p = 0; p < genParticles->size(); p++)
  {
    const reco::Candidate* particle = &genParticles->at(p);
    const bool sign = (particle->pdgId() > 0); // true for top, false for antitop
    
    // find hard-scattering top
    if(particle->pdgId() == (sign ? 6 : -6) && particle->status() == 3) 
    {
      // determine sign, use it to set references (t, b, W, l, nu) to either particles or antiparticles
      const reco::Candidate*& t = (sign ? genT : genTbar);
      const reco::Candidate*& b = (sign ? genB : genBbar);
      const reco::Candidate*& W = (sign ? genWp : genWm);
      const reco::Candidate*& l = (sign ? genLp : genLm);
      const reco::Candidate*& nu = (sign ? genNu : genNubar);
      // there should be no more than 1 top and 1 antitop in the event but who knows
      if(t)
        printf("Error: multiple hard-scattering t\n");
      // determine top decay
      t = particle;
      // find t -> bW decay
      for(unsigned int d = 0; d < t->numberOfDaughters(); d++)
      {
        const reco::Candidate* daughter = t->daughter(d);
        if(daughter->status() != 3)
          continue;
        if(daughter->pdgId() == (sign ? 5 : -5))
        {
          // again there should be no more than 1 b from top
          if(b) 
            printf("Error: multiple hard-scattering b\n");
          b = daughter;
        }
        if(daughter->pdgId() == (sign ? 24 : -24))
        {
          // and no more than 1 W from top
          if(W) 
            printf("Error: multiple hard-scattering W\n");
          W = daughter;
        }
      }
      // if either b or W not found, skip this top (although could even return here: there should be no other top in the event of course)
      if(!b || !W)
        continue;
      // find W -> lnu decay
      for(unsigned int d = 0; d < W->numberOfDaughters(); d++)
      {
        const reco::Candidate* daughter = W->daughter(d);
        if(daughter->status() != 3)
          continue;
        // electron
        if(daughter->pdgId() == (sign ? -11 : 11))
        {
          // should be no more than one electron from W
          if(l) 
            printf("Error: multiple hard-scattering l\n");
          l = daughter;
        }
        if(daughter->pdgId() == (sign ? 12 : -12))
        {
          // and no more than one (e)neutrino from W
          if(nu) 
            printf("Error: multiple hard-scattering nu\n");
          nu = daughter;
        }
        // muon
        if(daughter->pdgId() == (sign ? -13 : 13))
        {
          // should be no more than one muon from W
          if(l) 
            printf("Error: multiple hard-scattering l\n");
          l = daughter;
        }
        if(daughter->pdgId() == (sign ? 14 : -14))
        {
          // should be no more than one mu(neutrino) from W
          if(nu) 
            printf("Error: multiple hard-scattering nu\n");
          nu = daughter;
        }
      }
      // if no leptonic W decay, continue
      if(!l || !nu)
        continue;
      // if gets here, this is inetersting top decay into e or mu: get final states
      l = GetFinalState(l, l->pdgId());
      nu = GetFinalState(nu, nu->pdgId());
    }
  }

  // fill branch variables
  FillFourMomentum(genT, _mcT);
  FillFourMomentum(genTbar, _mcTbar);
  FillFourMomentum(genB, _mcB);
  FillFourMomentum(genBbar, _mcBbar);
  FillFourMomentum(genWp, _mcWp);
  FillFourMomentum(genWm, _mcWm);
  FillFourMomentum(genLp, _mcLp);
  FillFourMomentum(genNu, _mcNu);
  FillFourMomentum(genLm, _mcLm);
  FillFourMomentum(genNubar, _mcNubar);
  
  // now classify this generated event
  _mcEventType = 0;
  if(genT && genTbar)
    _mcNTTbar++;
  if(genWp && genB)
    _mcNTWb++;
  if(genWm && genBbar)
    _mcNTWb++;
  // below are interesting dileptionic ttbar decays: 1 ttbar decay into ee, 2 ttbar decay into mumu, 3 ttbar decay into emu, 0 anything else
  if(genLp && genLm && genNu && genNubar)
  {
    _mcNTtbarDilepton++;
    if(genLp->pdgId() == -11 && genLm->pdgId() == 11 && genNu->pdgId() == 12 && genNubar->pdgId() == -12)
    {
      _mcEventType = 1;
      _mcNTtbarDileptonEE++;
    }
    if(genLp->pdgId() == -13 && genLm->pdgId() == 13 && genNu->pdgId() == 14 && genNubar->pdgId() == -14)
    {
      _mcEventType = 2;
      _mcNTtbarDileptonMuMu++;
    }
    if(genLp->pdgId() == -11 && genLm->pdgId() == 13 && genNu->pdgId() == 12 && genNubar->pdgId() == -14)
    {
      _mcEventType = 3;
      _mcNTtbarDileptonEMu++;
    }
    if(genLp->pdgId() == -13 && genLm->pdgId() == 11 && genNu->pdgId() == 14 && genNubar->pdgId() == -12)
    {
      _mcEventType = 3;
      _mcNTtbarDileptonEMu++;
    }
    //if(genLp->pdgId() == -15 && genLm->pdgId() == 15 && genNu->pdgId() == 16 && genNubar->pdgId() == -16)
      //_mcNTtbarDileptonTauTau++;
  }
}

// ------------ method called for each event  ------------
void Analyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
	using namespace reco;
	using namespace std;

  // event counting, printout after each 1K processed events
  _nevents++;
  //printf("*** EVENT %6d ***\n", _nevents);
  if( (_nevents % 1000) == 0)
  {
    //printf("*****************************************************************\n");
    printf("************* NEVENTS = %d K, selected = %d *************\n", _nevents / 1000, _neventsSelected);
    //printf("*****************************************************************\n");
  }
  //return;
  
  // declare event contents
  edm::Handle<reco::GenParticleCollection> genParticles;
  Handle<reco::VertexCollection> primVertex;
  edm::Handle<reco::MuonCollection> muons;
  edm::Handle<reco::GsfElectronCollection> electrons;
  edm::Handle<reco::PFJetCollection> jets;
  edm::Handle<reco::JetTagCollection> bTagHandle;
  edm::Handle<edm::View<reco::PFMET> > pfmets;
  Handle<TriggerResults> HLTR;

  // read event contents immediately (may be needed because of strange perfomance issues)
  /*if(_flagGEN)
    iEvent.getByLabel(_inputTagMCgen, genParticles);
  if(_flagRECO)
  {
    iEvent.getByLabel(_inputTagPrimaryVertex, primVertex);
    iEvent.getByLabel(_inputTagElectrons, electrons);
    iEvent.getByLabel(_inputTagMuons, muons);
    iEvent.getByLabel(_inputTagJets, jets);
    iEvent.getByLabel(_inputTagBtags, bTagHandle);
    iEvent.getByLabel(_inputTagMet,pfmets);
    iEvent.getByLabel(_inputTagTriggerResults, HLTR);
  }*/

  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // >>>>>>>>> event selection >>>>>>>>>
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  //
  // initialise event variables with default values
  InitBranchVars();
  // process generator level, if needed
  bool selGEN = false;
  if(_flagGEN)
  {
    iEvent.getByLabel(_inputTagMCgen, genParticles);
    SelectMCGen(genParticles);
    if(_mcEventType != 0)
      selGEN = true;
    // if nothing interesting at generator level and not required to process reco level, return here
    if(!selGEN && !_flagRECO)
      return;
  }
  // process reco level, if needed
  bool selRECO = false;
  if(_flagRECO)
  {
    // primary vertex
    iEvent.getByLabel(_inputTagPrimaryVertex, primVertex);
    reco::VertexCollection::const_iterator pv = primVertex->begin();
    // electrons
    _signLeptonP = _signLeptonM = 0;
    iEvent.getByLabel(_inputTagElectrons, electrons);
    SelectEl(electrons, pv);
    // muons
    iEvent.getByLabel(_inputTagMuons, muons);
    SelectMu(muons, pv);
    // require pair of opposite signed leptons
    if( _signLeptonP && _signLeptonM )
      selRECO = true;
    if(!selRECO && !selGEN)
      return;
    // jets and b-tagging
    iEvent.getByLabel(_inputTagJets, jets);
    iEvent.getByLabel(_inputTagBtags, bTagHandle);
    const reco::JetTagCollection& bTags = *(bTagHandle.product());
    SelectJet(jets, bTags, iEvent, iSetup);
    // require two jets
    if( _Njet >= 2)
      selRECO = true;
    // if nothing interesting at both generator and reco levels, return here
    if(!selRECO && !selGEN)
      return;
    // fill MET
    iEvent.getByLabel(_inputTagMet, pfmets);
    SelectMET(pfmets);
    // fill primary vertex
    SelectPrimaryVertex(primVertex);
    // fill triggers
    iEvent.getByLabel(_inputTagTriggerResults, HLTR);
    SelectTriggerBits(HLTR);
  }
  // fill event info
  SelectEvent(iEvent);
  // all done: store event
  _tree->Fill();
  _neventsSelected++;
}


// ------------ method called when starting to processes a run  ------------
void Analyzer::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{
  // trigger stuff
  HLTConfigProvider triggerConfig;
  bool changed = true;
  triggerConfig.init(iRun, iSetup, _inputTagTriggerResults.process(), changed);
  FindTriggerBits(triggerConfig);
}

// below is some default stuff, was not modified

// ------------ method called once each job just before starting event loop  ------------
void Analyzer::beginJob() {;}

// ------------ method called once each job just after ending the event loop  ------------
void Analyzer::endJob() {;}

// ------------ method called when ending the processing of a run  ------------
void Analyzer::endRun(edm::Run const& run, edm::EventSetup const& setup) {;}

// ------------ method called when starting to processes a luminosity block  ------------
void Analyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) {;}

// ------------ method called when ending the processing of a luminosity block  ------------
void Analyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) {;}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void Analyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(Analyzer);
