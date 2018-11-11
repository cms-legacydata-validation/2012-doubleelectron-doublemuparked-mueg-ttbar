// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>>>>>>>>>>> Helper for plotter >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// additional files from this analysis 
#include "settings.h"
// C++ library or ROOT header files
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TGaxis.h>
#include <TFile.h>
#include <TLegend.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TMath.h>

// produce graph from histogram
// Argumnets:
//    TH1* h: input histogram
//    double xpos = 0.5: relative position for y point of the graph (for default 0.5 in the middle ob the bin)
// Returns produced graph
TGraphAsymmErrors* HtoGragh(const TH1* h, double xpos = 0.5)
{
  TGraphAsymmErrors* g = new TGraphAsymmErrors;
  for(int b = 0; b < h->GetNbinsX(); b++)
  {
    double xmin = h->GetXaxis()->GetBinLowEdge(b + 1);
    double xmax = h->GetXaxis()->GetBinUpEdge(b + 1);
    double bw = xmax - xmin;
    double x = xmin + xpos * bw;
    double y = h->GetBinContent(b + 1);
    double yer = h->GetBinError(b + 1);
    int point = g->GetN();
    g->SetPoint(point, x, y);
    double xerl = 0.0;
    double xerh = 0.0;
    g->SetPointError(point, 0.0, 0.0, yer, yer);
  }
  return g;
}

// helper class which contains input for x-section plotting
class ZPlotCSInput
{
  public:
    // directories
    TString baseDir;
    TString plotDir;
    // MC background samples
    std::vector<TString> VecMCBackgr;
    // variables for x-sections and their 2D "range" histograms
    std::vector<TString> VecVar;
    std::vector<TH2F*> VecHR;
    // channels
    std::vector<TString> VecTitle;
    std::vector<int> VecColor;
    std::vector<int> VecStyle;
    // if true, normalised cross section are plotted
    bool Norm;
    // if true, reference x-sections from paper are plotted
    bool Paper;
    // for double-differnetial cross sections: extra file name suffix (to store plottedcross sections in different files)
    TString ExtraFileName;
    // for double-differnetial cross sections: extra label with 1st variable bin
    std::vector<TString> VecExtraTitle;
    // for double-differnetial cross sections:  extra normalisation factor (1st variable bin width)
    std::vector<double> VecExtraNorm;
};

// routine to get x-sections from TOP-12-028 and TOP-14-013 (they are hardcoded inside)
// Arguments:
//   const TString& var: variable name (input)
//   TGraphAsymmErrors* gstat: produced graph with statistical uncertainties (output)
//   TGraphAsymmErrors* gsyst: produced graph with total (stat. + systematical) uncertainties (output)
void GetPaperCS(const TString& var, TGraphAsymmErrors* gstat, TGraphAsymmErrors* gsyst)
{
  double *sig, *stat, *syst;
  if(var == "ptt")
  {
    sig = new double[5]  {4.33e-3,6.40e-3,3.08e-3,8.62e-4,1.88e-4};
    stat = new double[5] {1.1   ,0.9    ,1.0    ,1.3    ,2.7    };
    syst = new double[5] {2.3   ,1.4    ,2.1    ,3.6    ,7.4    };
  }
  else if(var == "yt")
  {
    sig = new double[8]  {0.0763,0.197,0.282,0.335,0.328,0.285,0.203,0.074};
    stat = new double[8] {2.0   ,1.0  ,1.1  ,1.0  ,1.0  ,1.1  ,1.1  ,2.0  };
    syst = new double[8] {5.3   ,1.5  ,1.5  ,1.5  ,1.9  ,1.1  ,1.1  ,5.3  };
  }
  else if(var == "ytt")
  {
    sig = new double[8]  {0.0471,0.182,0.309,0.418,0.409,0.315,0.179,0.0459};
    stat = new double[8] {4.0,   1.8,  1.4,  1.2,  1.2,  1.5,  1.8,  4.0};
    syst = new double[8] {6.6,   1.5,  1.5,  1.7,  1.2,  1.3,  1.8,  6.0};
  }
  else if(var == "pttt")
  {
    sig = new double[4]  {1.43e-2,6.90e-3,1.91e-3,3.47e-4};
    stat = new double[4] {0.8    ,1.0    ,1.1    ,2.1};
    syst = new double[4] {6.1    ,4.7    ,5.4    ,4.7};
  }
  else if(var == "mtt")
  {
    sig = new double[7]  {0.0, 4.14e-3,4.50e-3,1.95e-3,5.25e-4,1.00e-4,7.28e-6};
    stat = new double[7] {0.0, 3.0    ,1.7    ,1.8    ,2.8    ,3.7    ,14.4};
    syst = new double[7] {0.0, 8.6    ,5.3    ,2.9    ,3.2    ,7.3    ,28.2};
  }
  else if(var == "ytptt1")
  {
    sig = new double[4]  {3.08e-3, 3.71e-3, 1.36e-3, 1.11e-4};
    stat = new double[4] {4.4    , 3.6    , 5.0    , 7.7    };
    syst = new double[4] {7.4    , 6.2    , 4.8    ,11.7    };
  }
  else if(var == "ytptt2")
  {
    sig = new double[4]  {2.90e-3, 3.17e-3, 1.17e-3, 8.78e-5};
    stat = new double[4] {3.2    , 3.0    , 4.5    , 8.1    };
    syst = new double[4] {3.0    , 4.2    , 7.3    , 8.3    };
  }
  else if(var == "ytptt3")
  {
    sig = new double[4]  {2.25e-3, 2.32e-3, 8.85e-4, 5.58e-5};
    stat = new double[4] {3.4    , 3.3    , 4.8    , 9.6    };
    syst = new double[4] {4.9    , 4.8    , 7.6    ,13.3    };
  }
  else if(var == "ytptt4")
  {
    sig = new double[4]  {9.08e-4, 1.03e-3, 3.14e-4, 1.55e-5};
    stat = new double[4] {5.6    , 4.5    , 7.9    ,17.3    };
    syst = new double[4] {6.5    , 7.3    , 6.7    ,16.6    };
  }
  else if(var == "mttyt1")
  {
    sig = new double[4]  {3.21e-3, 2.92e-3, 2.06e-3, 6.58e-4};
    stat = new double[4] {4.9    , 4.0    , 4.5    , 9.3    };
    syst = new double[4] {9.8    , 5.7    , 3.8    , 7.4    };
  }
  else if(var == "mttyt2")
  {
    sig = new double[4]  {2.92e-3, 2.39e-3, 1.67e-3, 5.99e-4};
    stat = new double[4] {3.2    , 2.8    , 3.2    , 5.4    };
    syst = new double[4] {8.3    , 2.9    , 4.2    ,10.8    };
  }
  else if(var == "mttyt3")
  {
    sig = new double[4]  {1.01e-3, 8.73e-4, 6.50e-4, 2.91e-4};
    stat = new double[4] {5.0    , 4.5    , 4.8    , 6.7    };
    syst = new double[4] {7.7    , 6.4    , 6.5    , 8.7    };
  }
  else if(var == "mttyt4")
  {
    sig = new double[4]  {6.19e-5, 6.77e-5, 7.02e-5, 4.42e-5};
    stat = new double[4] {7.8    , 6.5    , 5.4    , 6.2    };
    syst = new double[4] {19.6   , 8.3    , 6.3    ,14.2    };
  }
  else if(var == "mttytt1")
  {
    sig = new double[4]  {3.17e-3, 3.07e-3, 2.44e-3, 9.14e-4};
    stat = new double[4] {4.5    , 4.4    , 5.1    , 4.9    };
    syst = new double[4] {6.6    , 5.0    , 6.1    , 3.9    };
  }
  else if(var == "mttytt2")
  {
    sig = new double[4]  {3.06e-3, 2.76e-3, 2.05e-3, 6.43e-4};
    stat = new double[4] {2.8    , 2.8    , 3.6    , 3.6    };
    syst = new double[4] {6.4    , 5.4    , 3.9    , 6.0    };
  }
  else if(var == "mttytt3")
  {
    sig = new double[4]  {1.34e-3, 1.17e-3, 7.66e-4, 1.85e-4};
    stat = new double[4] {3.8    , 4.0    , 5.8    , 8.0    };
    syst = new double[4] {4.8    , 3.2    , 5.0    , 9.6    };
  }
  else if(var == "mttytt4")
  {
    sig = new double[4]  {1.49e-4, 1.18e-4, 6.53e-5, 9.50e-6};
    stat = new double[4] {4.2    , 5.4    , 8.3    ,17.2    };
    syst = new double[4] {6.8    , 5.0    ,13.6    ,35.6    };
  }
  else if(var == "mttdetatt1")
  {
    sig = new double[3]  {3.35e-3, 2.53e-3, 2.31e-4};
    stat = new double[3] {7.3    , 3.2    , 8.4    };
    syst = new double[3] {9.7    , 6.1    ,10.4    };
  }
  else if(var == "mttdetatt2")
  {
    sig = new double[3]  {1.60e-3, 1.69e-3, 3.87e-4};
    stat = new double[3] {6.2    , 3.3    , 2.3    };
    syst = new double[3] {5.9    , 3.9    , 4.1    };
  }
  else if(var == "mttdetatt3")
  {
    sig = new double[3]  {3.56e-4, 3.55e-4, 2.29e-4};
    stat = new double[3] {10.7   , 7.7    , 2.4    };
    syst = new double[3] {9.0    ,15.0    , 6.2    };
  }
  else if(var == "mttdetatt4")
  {
    sig = new double[3]  {2.08e-5, 2.42e-5, 2.31e-5};
    stat = new double[3] {13.3   , 8.6    , 3.1    };
    syst = new double[3] {11.4   ,12.9    , 7.4    };
  }
  else if(var == "mttdphitt1")
  {
    sig = new double[3]  {5.68e-4, 2.68e-3, 6.67e-3};
    stat = new double[3] {5.5    , 3.3    , 5.6    };
    syst = new double[3] {10.1   , 6.4    ,22.1    };
  }
  else if(var == "mttdphitt2")
  {
    sig = new double[3]  {2.59e-4, 2.09e-3, 8.90e-3};
    stat = new double[3] {5.9    , 2.5    , 2.3    };
    syst = new double[3] {12.1   , 6.8    , 4.9    };
  }
  else if(var == "mttdphitt3")
  {
    sig = new double[3]  {7.04e-5, 7.23e-4, 4.11e-3};
    stat = new double[3] {11.0   , 4.4    , 2.9    };
    syst = new double[3] {19.6   ,13.8    , 7.4    };
  }
  else if(var == "mttdphitt4")
  {
    sig = new double[3]  {6.06e-6, 6.40e-5, 4.21e-4};
    stat = new double[3] {13.3   , 6.0    , 3.6    };
    syst = new double[3] {22.1   ,10.5    , 5.2    };
  }
  else if(var == "mttpttt1")
  {
    sig = new double[4]  {6.63e-5, 3.51e-5, 9.61e-6, 6.19e-7};
    stat = new double[4] {3.0    , 3.5    , 6.6    ,14.8    };
    syst = new double[4] {6.7    , 7.5    ,13.1    ,17.2    };
  }
  else if(var == "mttpttt2")
  {
    sig = new double[4]  {5.57e-5, 2.82e-5, 8.34e-6, 9.02e-7};
    stat = new double[4] {2.0    , 2.5    , 4.7    , 7.6    };
    syst = new double[4] {6.7    , 5.5    ,14.3    , 7.5    };
  }
  else if(var == "mttpttt3")
  {
    sig = new double[4]  {2.09e-5, 1.11e-5, 3.24e-6, 2.91e-7};
    stat = new double[4] {3.4    , 4.1    , 7.1    ,11.8    };
    syst = new double[4] {11.1   , 7.7    ,16.6    , 6.9    };
  }
  else if(var == "mttpttt4")
  {
    sig = new double[4]  {1.68e-6, 1.09e-6, 3.80e-7, 3.96e-8};
    stat = new double[4] {5.8    , 6.4    , 7.8    , 9.2    };
    syst = new double[4] {10.0   ,14.1    ,10.4    ,18.3    };
  }
  else
  {
    printf("Error: unknown variable %s\n", var.Data());
    exit(1);
  }
  for(int p = 0; p < gstat->GetN(); p++)
  {
    gstat->SetPoint(p, (gstat->GetX())[p], sig[p]);
    gsyst->SetPoint(p, (gsyst->GetX())[p], sig[p]);
    double tot = TMath::Sqrt(stat[p] * stat[p] + syst[p] * syst[p]);
    gstat->SetPointError(p, 0.0, 0.0, (gstat->GetY())[p] * stat[p] / 100.0, (gstat->GetY())[p] * stat[p] / 100.0);
    gsyst->SetPointError(p, 0.0, 0.0, (gsyst->GetY())[p] * syst[p] / 100.0, (gsyst->GetY())[p] * tot / 100.0);
  }
}

// routine to calculate and plot x-xsections
// Arguments:
//   const ZPlotCSInput& in: steering (see class ZPlotCSInput above)
//          (not flexible: should contain variables in the needed
//           order, see ttbarMakePlots.cxx)
void PlotCS(const ZPlotCSInput& in)
{
  TCanvas* c_cs;
  c_cs = new TCanvas("ccs", "", 1200, 800);
  c_cs->Divide(3, 2, 0.0001);
  // loop over variables
  for(int v = 0; v < 5; v++)
  {
    c_cs->cd(v + 1);
    if(v == 4)
      gPad->SetLogy();
    in.VecHR[v]->Draw();
    TString var = in.VecVar[v];
    // histograms for combined dilepton channel (to be obtained)
    TH1D *hcombsig, *hcombreco, *hcombgen;
    double nsig = 0;
    double nreco = 0;
    double ngen = 0;
    TLegend* leg = new TLegend(0.46, 0.68, 0.92, 0.90);
    leg->SetTextSize(0.045);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    for (int ch = 1; ch < 4; ch++)
    {
      // MC background
      TH1D* hbackgr;
      for(int mc = 0; mc < in.VecMCBackgr.size(); mc++)
      {
        TFile* f = TFile::Open(TString::Format("%s/mc%sReco-c%d.root", in.baseDir.Data(), in.VecMCBackgr[mc].Data(), ch));
        TH1D* h = (TH1D*)f->Get(TString::Format("h_%s_cs", var.Data()));
        if(mc == 0)
          hbackgr = new TH1D(*h);
        else
          hbackgr->Add(h);
      }
      // data
      TFile* f = TFile::Open(TString::Format("%s/data-c%d.root", in.baseDir.Data(), ch));
      TH1D* hsig = new TH1D(*(TH1D*)f->Get(TString::Format("h_%s_cs", var.Data())));
      hsig->Add(hbackgr, -1.0);
      nsig += hsig->Integral(0, hsig->GetNbinsX());
      if(ch == 1)
        hcombsig = new TH1D(*hsig);
      else
        hcombsig->Add(hsig);
      // MC reconstruction level
      f = TFile::Open(TString::Format("%s/mcSigReco-c%d.root", in.baseDir.Data(), ch));
      TH1D* hacc = new TH1D(*(TH1D*)f->Get(TString::Format("h_%s_cs", var.Data())));
      nreco += hacc->Integral(0, hacc->GetNbinsX());
      if(ch == 1)
        hcombreco = new TH1D(*hacc);
      else
        hcombreco->Add(hacc);
      // MC generatir level
      f = TFile::Open(TString::Format("%s/mcSigGen-c%d.root", in.baseDir.Data(), ch));
      TH1D* hgen = (TH1D*)f->Get(TString::Format("h_%s_cs", var.Data()));
      ngen += hgen->Integral(0, hgen->GetNbinsX());
      if(ch == 1)
        hcombgen = new TH1D(*hgen);
      else
        hcombgen->Add(hgen);
      // acceptance (also referred to as detector efficiency)
      hacc->Divide(hgen);
      // cross section
      TH1D* hcs = hsig;
      hcs->Divide(hacc);
      double cs, cserr;
      cs = hcs->IntegralAndError(0, hcs->GetNbinsX() + 1, cserr);
      double br = 0.0115 * 11580.0; // branching ratio (ee, mumu) times luminosity
      if(ch == 3)
        br *= 2.0; // branching ratio 2 times larger for the emu channel
      // print calculated cross section
      // (channel code is: c1 ee, c2 mumu, c3 emu)
      // print only x-section in the emu channel (most precise and the only available in the paper TOP-13-004)
      //if(ch == 3)
        printf("x-section c%d %s:  %.1f +- %.1f\n", ch, var.Data(), cs / br, cserr / br);
      if(in.Norm)
        hcs->Scale(1.0 / hcs->Integral(), "width");
      // draw plot
      TGraphAsymmErrors* gcs = HtoGragh(hcs, 0.10 + 0.1 * ch);
      gcs->SetLineColor(in.VecColor[ch]);
      gcs->SetMarkerStyle(in.VecStyle[ch]);
      gcs->SetMarkerColor(in.VecColor[ch]);
      gcs->SetMarkerSize(0.9);
      leg->AddEntry(gcs, in.VecTitle[ch], "p");
      gcs->Draw("pz0");
    }
    // combined channel
    TH1D* hcombacc = hcombreco;
    hcombacc->Divide(hcombgen);
    TH1D* hcombcs = hcombsig;
    hcombcs->Divide(hcombacc);
    double cs, cserr;
    cs = hcombcs->IntegralAndError(0, hcombcs->GetNbinsX() + 1, cserr);
    double br = 0.046 * 9850.0;
    // the total x-section in the combined dilepton channel is calculated,
    // but not plotted by default: no reference number in CMS papers
    //printf("x-section dilepton %s:  %.1f +- %.1f\n", var.Data(), cs / br, cserr / br, nsig / nreco * ngen / br);
    if(in.Norm)
      hcombcs->Scale(1.0 / hcombcs->Integral(), "width");
    TGraphAsymmErrors* gcombcs = HtoGragh(hcombcs);
    gcombcs->SetLineColor(in.VecColor[0]);
    gcombcs->SetMarkerStyle(in.VecStyle[0]);
    gcombcs->SetMarkerColor(in.VecColor[0]);
    gcombcs->SetMarkerSize(0.9);
    // plot data
    leg->AddEntry(gcombcs, in.VecTitle[0], "pel");
    gcombcs->Draw("pz0");
    hcombcs->SetLineColor(1);
    hcombcs->Draw("hist same");
    hcombcs->Print("all");
    // plot paper results
    if(in.Paper)
    {
      TGraphAsymmErrors* gstat = HtoGragh(hcombcs, 0.7);
      gstat->SetLineColor(1);
      gstat->SetMarkerStyle(24);
      gstat->SetMarkerColor(1);
      gstat->SetMarkerSize(0.9);
      TGraphAsymmErrors* gsyst = HtoGragh(hcombcs, 0.7);
      gsyst->SetLineColor(1);
      gsyst->SetMarkerStyle(24);
      gsyst->SetMarkerColor(1);
      gsyst->SetMarkerSize(0.9);
      GetPaperCS(var, gstat, gsyst);
      gstat->Draw("p0");
      gsyst->Draw("pz0");
      leg->AddEntry(gstat, "CMS-TOP-12-028", "pe");
    }
    leg->Draw();
    in.VecHR[v]->Draw("axis same");
  }
  // save plot
  TString name = TString::Format("%s/cs%s", in.plotDir.Data(), (in.Norm) ? "_norm" : "");
  c_cs->SaveAs(name + ".eps");
  c_cs->SaveAs(name + ".pdf");
}

// routine to calculate and plot double-differential x-xsections (emu channel only, as in TOP-14-013)
// Arguments:
//   const ZPlotCSInput& in: steering (see class ZPlotCSInput above)
//          (not flexible: should contain variables in the needed
//           order, see ttbarMakePlots.cxx)
void PlotCS2D(const ZPlotCSInput& in)
{
  std::vector<TH1D*> hcs(in.VecVar.size());
  // loop over variables
  for(int v = 0; v < in.VecVar.size(); v++)
  {
    TString var = in.VecVar[v];
    double nsig = 0;
    double nreco = 0;
    double ngen = 0;
    const int ch = 3;
    // MC background
    TH1D* hbackgr;
    for(int mc = 0; mc < in.VecMCBackgr.size(); mc++)
    {
      TFile* f = TFile::Open(TString::Format("%s/mc%sReco-c%d.root", in.baseDir.Data(), in.VecMCBackgr[mc].Data(), ch));
      TH1D* h = (TH1D*)f->Get(TString::Format("h_%s_cs", var.Data()));
      if(mc == 0)
        hbackgr = new TH1D(*h);
      else
        hbackgr->Add(h);
    }
    // data
    TFile* f = TFile::Open(TString::Format("%s/data-c%d.root", in.baseDir.Data(), ch));
    TH1D* hsig = new TH1D(*(TH1D*)f->Get(TString::Format("h_%s_cs", var.Data())));
    hsig->Add(hbackgr, -1.0);
    nsig += hsig->Integral(0, hsig->GetNbinsX());
    // MC reconstruction level
    f = TFile::Open(TString::Format("%s/mcSigReco-c%d.root", in.baseDir.Data(), ch));
    TH1D* hacc = new TH1D(*(TH1D*)f->Get(TString::Format("h_%s_cs", var.Data())));
    nreco += hacc->Integral(0, hacc->GetNbinsX());
    // MC generatir level
    f = TFile::Open(TString::Format("%s/mcSigGen-c%d.root", in.baseDir.Data(), ch));
    TH1D* hgen = (TH1D*)f->Get(TString::Format("h_%s_cs", var.Data()));
    ngen += hgen->Integral(0, hgen->GetNbinsX());
    // acceptance (also referred to as detector efficiency)
    hacc->Divide(hgen);
    // cross section
    hcs[v] = hsig;
    hcs[v]->Divide(hacc);
  }
  // get normalisation
  double integral = 0.0;
  for(int v = 0; v < in.VecVar.size(); v++)
    integral += hcs[v]->Integral();

  // plot
  TCanvas* c_cs;
  c_cs = new TCanvas("ccs", "", 800, 800);
  c_cs->Divide(2, 2, 0.0001);
  // graphs to store reference paper cross sections
  std::vector<TGraphAsymmErrors*> gstat(in.VecVar.size());
  std::vector<TGraphAsymmErrors*> gsyst(in.VecVar.size());
  for(int v = 0; v < in.VecVar.size(); v++)
  {
    c_cs->cd(v + 1);
    gPad->SetLogy();
    in.VecHR[v]->Draw();
    TString var = in.VecVar[v];
    TLegend* leg = new TLegend(0.46, 0.68, 0.92, 0.90);
    leg->SetHeader(in.VecExtraTitle[v]);
    leg->SetTextSize(0.045);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    const int ch = 3;
    if(in.Norm)
    {
      // get normalised cross sections
      hcs[v]->Scale(1.0 / integral, "width");
      // in addition divide by 1st variable bin width
      hcs[v]->Scale(in.VecExtraNorm[v]);
      //for(int b = 0; b < hcs[v]->GetNbinsX(); b++)
      //  hcs[v]->SetBinError(b + 1, hcs[v]->GetBinError(b + 1) * 10.0);
    }
    leg->AddEntry(hcs[v], in.VecTitle[ch], "lpe");
    hcs[v]->SetLineColor(in.VecColor[ch]);
    hcs[v]->SetMarkerStyle(in.VecStyle[ch]);
    hcs[v]->SetMarkerColor(in.VecColor[ch]);
    hcs[v]->SetMarkerSize(0.9);
    hcs[v]->Draw("hist e1 same");
    // plot paper results
    if(in.Paper)
    {
      gstat[v] = HtoGragh(hcs[v], 0.7);
      gstat[v]->SetLineColor(1);
      gstat[v]->SetMarkerStyle(24);
      gstat[v]->SetMarkerColor(1);
      gstat[v]->SetMarkerSize(0.9);
      gsyst[v] = HtoGragh(hcs[v], 0.7);
      gsyst[v]->SetLineColor(1);
      gsyst[v]->SetMarkerStyle(24);
      gsyst[v]->SetMarkerColor(1);
      gsyst[v]->SetMarkerSize(0.9);
      GetPaperCS(var, gstat[v], gsyst[v]);
      gstat[v]->Draw("p0");
      gsyst[v]->Draw("pz0");
      leg->AddEntry(gstat[v], "CMS-TOP-14-013", "pe");
    }
    leg->Draw();
    in.VecHR[v]->Draw("axis same");
  }
  // save plot
  TString name = TString::Format("%s/cs%s", in.plotDir.Data(), (in.Norm) ? "_norm" : "") + "_" + in.ExtraFileName;
  c_cs->SaveAs(name + ".eps");
  c_cs->SaveAs(name + ".pdf");

  // ratio plot
  if(in.Paper)
  {
    TCanvas* c_cs_ratio;
    c_cs_ratio = new TCanvas("ccsr", "", 800, 800);
    c_cs_ratio->Divide(2, 2, 0.0001);
    const int ch = 3;
    for(int v = 0; v < in.VecVar.size(); v++)
    {
      c_cs_ratio->cd(v + 1);
      TH2D* hrr = new TH2D("", "", 1, in.VecHR[v]->GetXaxis()->GetBinLowEdge(1), in.VecHR[v]->GetXaxis()->GetBinLowEdge(2), 1, 0.5, 1.5);
      hrr->GetXaxis()->SetTitle(in.VecHR[v]->GetXaxis()->GetTitle());
      hrr->GetXaxis()->SetNdivisions(in.VecHR[v]->GetXaxis()->GetNdivisions());
      hrr->GetYaxis()->SetTitle("Ratio");
      hrr->Draw();
      TH1D* hxsecr = new TH1D(*hcs[v]);
      for(int b = 0; b < hcs[v]->GetNbinsX(); b++)
      {
        double ref = hcs[v]->GetBinContent(b + 1);
        hxsecr->SetBinContent(b + 1, hcs[v]->GetBinContent(b + 1) / ref);
        hxsecr->SetBinError(b + 1, hcs[v]->GetBinError(b + 1) / ref);
        gstat[v]->GetY()[b] /= ref;
        gsyst[v]->GetY()[b] /= ref;
        gstat[v]->GetEYhigh()[b] /= ref;
        gsyst[v]->GetEYhigh()[b] /= ref;
        gstat[v]->GetEYlow()[b] /= ref;
        gsyst[v]->GetEYlow()[b] /= ref;
      }
      hxsecr->Draw("hist e1 same");
      gstat[v]->Draw("p0");
      gsyst[v]->Draw("pz0");
      TLegend* leg = new TLegend(0.46, 0.68, 0.92, 0.90);
      leg->SetHeader(in.VecExtraTitle[v]);
      leg->SetTextSize(0.045);
      leg->SetBorderSize(0);
      leg->SetFillStyle(0);
      leg->AddEntry(hxsecr, in.VecTitle[ch], "lpe");
      leg->AddEntry(gstat[v], "CMS-TOP-14-013", "pe");
      leg->Draw();
      hrr->Draw("axis same");
    }
    // save plot
    TString name = TString::Format("%s/cs%s", in.plotDir.Data(), (in.Norm) ? "_norm" : "") + "_" + in.ExtraFileName + "_ratio";
    c_cs_ratio->SaveAs(name + ".eps");
    c_cs_ratio->SaveAs(name + ".pdf");
  }
}
