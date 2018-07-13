// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// This code processes ROOT ntuples for ttbar analysis (see 
// Analyzer/src/Analyzer.cc) and produces histograms, which are 
// further used to make final plots (see ttbarMakePlots.cxx).
// Run: ./ttbarMakeHist
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//
// additional files from this analysis (look there for description) 
#include "eventReco.h"
#include "settings.h"
//
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>>>>>>>>>>>> Main function >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
int main(int argc, char** argv)
{
  //
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // >>>>>>>>>>>>>>>>>>>>> Settings >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  //
  // set directories to data and MC ntuples
  TString dataDir = gDataDir;
  TString mcDir = gMcDir;
  //
  // flags what to run
  bool flagData    = 1; // if 1, data will be processed
  bool flagMCsig   = 1; // if 1, signal MC (dileptonic decay channel) will be processed
  bool flagMCstop  = 1; // if 1, MC single top (background) will be processed
  bool flagMCdy    = 1; // if 1, MC Drell-Yan (background) will be processed
  //
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  //
  
  // histograms
  TH1::SetDefaultSumw2(); // keep histogram weights by default
  // ZVarHisto is a simple class which incorporates a histogram and a variable name. 
  // This class is used to store needed input settings (variable names, binning) 
  // for control plots and cross sections (as in TOP-12-028)
  std::vector<ZVarHisto> vecVH, vecVHGen; // vecVH for reconstruction level, vecVHGen for generator level
  // histograms and variables for control plots
  vecVHGen.push_back(ZVarHisto("ptt", new TH1D("h_ptt", "pT top", 20, 0.0, 400.0))); // pT(top) (pT = transeverse momentum)
  vecVHGen.push_back(ZVarHisto("ptat", new TH1D("h_ptat", "pT atop", 20, 0.0, 400.0))); // pT(antitop)
  vecVHGen.push_back(ZVarHisto("pttat", new TH1D("h_pttat", "pT tatop", 20, 0.0, 400.0))); // pT(top)+pT(antitop)
  vecVHGen.push_back(ZVarHisto("pttt", new TH1D("h_pttt", "pT ttbar", 15, 0.0, 300.0))); // pT(ttbar), ttbar = top + antitop
  vecVHGen.push_back(ZVarHisto("yt", new TH1D("h_yt", "y top", 26, -2.6, 2.6))); // y(top) (y = rapidity)
  vecVHGen.push_back(ZVarHisto("yat", new TH1D("h_yat", "y atop", 26, -2.6, 2.6))); // y(antitop)
  vecVHGen.push_back(ZVarHisto("ytat", new TH1D("h_ytat", "y tatop", 26, -2.6, 2.6))); // y(top)+y(antitop)
  vecVHGen.push_back(ZVarHisto("ytt", new TH1D("h_ytt", "y ttbar", 26, -2.6, 2.6))); // y(ttbar)
  // histograms and variables for cross sections
  {
    double bins[] = {0.,65.,125.,200.,290.,400.};
    vecVHGen.push_back(ZVarHisto("ptt", new TH1D("h_ptt_cs", "pT top", 5, bins)));
    vecVHGen.push_back(ZVarHisto("ptat", new TH1D("h_ptat_cs", "pT atop", 5, bins)));
    vecVHGen.push_back(ZVarHisto("pttat", new TH1D("h_pttat_cs", "pT tatop", 5, bins)));
  }
  {
    double bins[] = {-2.5,-1.6,-1.0,-0.5,0.0,0.5,1.0,1.6,2.5};
    vecVHGen.push_back(ZVarHisto("yt", new TH1D("h_yt_cs", "y top", 8, bins)));
    vecVHGen.push_back(ZVarHisto("yat", new TH1D("h_yat_cs", "y atop", 8, bins)));
    vecVHGen.push_back(ZVarHisto("ytat", new TH1D("h_ytat_cs", "y tatop", 8, bins)));
  }
  {
    double bins[] = {0.,30.,80.,170.,300.};
    vecVHGen.push_back(ZVarHisto("pttt", new TH1D("h_pttt_cs", "pT ttbar", 4, bins)));
  }
  {
    double bins[] = {-2.5,-1.5,-1.0,-0.5,0.0,0.5,1.0,1.5,2.5};
    vecVHGen.push_back(ZVarHisto("ytt", new TH1D("h_ytt_cs", "y ttbar", 8, bins)));
  }
  {
    double bins[] = {0.,340.,380.,470.,620.,820.,1100.,1600.};
    vecVHGen.push_back(ZVarHisto("mtt", new TH1D("h_mtt_cs", "M ttbar", 7, bins)));
  }

  // for reconstruction level the same binning is needed
  vecVH = vecVHGen;
  // add lepton pT histogram at reconstruction level
  // (here you can add more reconstruction level histograms)
  vecVH.push_back(ZVarHisto("ptl", new TH1D("h_ptl", "pT leptons", 23, 30.0, 260.0)));
  
  // loop over decay channels (ch = 1 ee, ch = 2 mumu, ch = 3 emu)
  for(int ch = 1; ch <= 3; ch++)
  {
    //if(ch != 3) continue; // if you need only emu (for test purpose e.g.)
    // 
    // below similar pieces of code come for data and several MC samples, 
    // detailed description is given for the first piece, while later on 
    // only new features are described
    //
    // *****************************************
    // **************** DATA *******************
    // *****************************************
    if(flagData)
    {
      // ZEventRecoInput is a class for event reconstruction, see its description in eventReco.h
      ZEventRecoInput in;
      //in.MaxNEvents = 100; // if you need to limit the number of processed events
      in.Name = "data"; // name pattern for output histograms
      in.Type = 1; // type = 1 for data, 2 for MC signal, 3 for MC 'ttbar other', 4 for the rest of MC background samples
      in.Channel = ch; // decay channel
      in.VecVarHisto = vecVH; // need to copy it, because further will be changed
      // input ROOT ntuples
      if(ch == 1) // ee
      {
        in.AddToChain(dataDir + "/DoubleElectron/*.root");
        in.AddToChain(dataDir + "/DoubleElectron-RunC/*.root");
      }
      else if(ch == 2) // mumu
      {
        in.AddToChain(dataDir + "/DoubleMuParked/*.root");
        in.AddToChain(dataDir + "/DoubleMuParked-RunC/*.root");
      }
      else if(ch == 3) // emu
      {
        in.AddToChain(dataDir + "/MuEG/*.root");
        in.AddToChain(dataDir + "/MuEG-RunC/*.root");
      }
      // main part: event reconstruction call
      eventreco(in);
    }
    
    // *****************************************
    // ************** MC signal ****************
    // *****************************************
    //
    // MC event weights need to be changed to most precise theoretical predictions, 
    // the formula is:
    // weight = lumi / (nevents / sigma_MC) * (sigma_theory / sigma_MC) = lumi * nevents / sigma_theory
    //
    // Number of events can be obtained from webpage (see http://opendata.cern.ch/collection/CMS-Simulated-Datasets), 
    // but it should be checked that all events have been processed at the Analyzer step (see end of log files)
    //
    // MC cross section can be obtained from any ROOT file in the mC sample: open the ROOT file, create TBrowser, navigate to
    // Runs -> GenRunInfoProduct_generator__SIM. -> GenRunInfoProduct_generator__SIM.obj -> InternalXSec -> value_
    // (nevertheless sigma_MC cancels)
    //
    // number of events: 62131965
    // MC cross section theory: 245.8
    // weight: 9850.0 / 62131965 * 245.8 = 0.038967543
    if(flagMCsig)
    {
      // MC signal reco level
      ZEventRecoInput in;
      //in.MaxNEvents = 1000;
      in.Weight = 0.038967543; // weight (see above)
      in.Name = "mcSigReco";
      in.Type = 2;
      in.Channel = ch;
      in.VecVarHisto = vecVH;
      in.AddToChain(mcDir + "/TTJets_MSDecays_central_TuneZ2star_8TeV-madgraph-tauola/*.root");
      eventreco(in);
      // MC ttbar other (background): re-use existing ZEventRecoInput, just change type
      in.Name = "mcSigOtherReco";
      in.Type = 3;
      eventreco(in);
      // MC ttbar signal, generator level: again re-use existing ZEventRecoInput, change type and set proper flag (see below)
      in.Name = "mcSigGen";
      in.Type = 2;
      in.VecVarHisto = vecVHGen;
      in.Gen = true; // flag to notify that generator level should be processed
      eventreco(in);
    }
    // *****************************************
    // ************ MC single top **************
    // *****************************************
    // number of events: XXX
    // MC cross section theory: 11.1 (times factor 2 because there is top and antitop)
    // weight: 9850.0 / (497658 + 493460) * 11.1 * 2 = 0.2206
    if(flagMCstop)
    {
      ZEventRecoInput in;
      //in.MaxNEvents = 1000;
      in.Name = "mcSingleTopReco";
      in.Type = 4;
      in.Weight = 0.2206;
      in.Channel = ch;
      in.VecVarHisto = vecVH;
      in.AddToChain(mcDir + "/Tbar_tW-channel-DR_TuneZ2star_8TeV-powheg-tauola/*.root");
      in.AddToChain(mcDir + "/T_tW-channel-DR_TuneZ2star_8TeV-powheg-tauola/*.root");
      eventreco(in);
    }
    // *****************************************
    // **************** MC DY ******************
    // *****************************************
    // only high mass DY sample is included
    // Events: 30458871
    // MC cross section theory: 3503.71 (multiplied by Z -> ll branching ratio)
    // weight: 9850.0 / 30458871 * 3503.71 = 1.13305393
    if(flagMCdy)
    {
      ZEventRecoInput in;
      //in.MaxNEvents = 1000;
      in.Type = 4;
      in.Channel = ch;
      in.VecVarHisto = vecVH;
      in.Name = "mcDYhmReco";
      in.Weight = 1.13305393;
      in.ClearChain();
      in.AddToChain(mcDir + "/DYJetsToLL_M-50_TuneZ2Star_8TeV-madgraph-tarball/*.root");
      eventreco(in);
    }
  }

  return 0;
}
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
