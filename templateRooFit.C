//templateRooFit.C
#include <iostream>
#include <fstream>
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooExponential.h"
#include "RooChebychev.h"
#include "RooPolynomial.h"
#include "RooAddPdf.h"
#include "RooHistPdf.h"
#include "RooDataHist.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h"
#include "TAxis.h"
#include "TTree.h"
#include "RooPlot.h"
#include <map>
#include <set>
#include <sstream>
using namespace RooFit;
using namespace std;

void templateRooFit()
{
  // Roofit variable
  RooRealVar aco("aco", "aco", 0, 1);

  // Import Data
  string dataName = "hist_data_4000tr.root";
  TFile* dataFile = TFile::Open(dataName.c_str(),"READ");
  TTree* tree = (TTree*)dataFile->Get("tree");
  RooDataSet* data = new RooDataSet("data","data set with aco", RooArgSet(aco), Import(*tree));

  // Import Starlight Signal
  string sigName = "hist_SL_4000tr.root";
  TFile* sigFile = TFile::Open(sigName.c_str(),"READ");
  TTree* tree4 = (TTree*)sigFile->Get("tree");
  RooDataSet* sig = new RooDataSet("sig","SL signal with aco", RooArgSet(aco), Import(*tree4));

  // Import FSR
  string FSRname = "hist_SL_FSR.root";
  TFile* FSRfile = TFile::Open(FSRname.c_str(),"READ");
  TTree* tree2 = (TTree*)FSRfile->Get("tree");
  RooDataSet* fsr = new RooDataSet("fsr","FSR with aco", RooArgSet(aco), Import(*tree2));

  // Import Background
  string bkgName = "hist_bkg_4000tr.root";
  TFile* bkgFile = TFile::Open(bkgName.c_str(),"READ");
  TTree* tree3 = (TTree*)bkgFile->Get("tree");
  RooDataSet* bkg = new RooDataSet("bkg","bkg with aco", RooArgSet(aco), Import(*tree3));

  // Set Bin Number
  aco.setBins(4000);

  // Binned Versions of Data Sets
  RooDataHist *dataBinned = new RooDataHist("dataBinned", "Binned Data", RooArgSet(aco), *data);
  RooDataHist *fsrBinned = new RooDataHist("fsrBinned", "Binned FSR", RooArgSet(aco), *fsr);
  RooDataHist *sigBinned = new RooDataHist("sigBinned", "Binned sig", RooArgSet(aco), *sig);
  RooDataHist *bkgBinned = new RooDataHist("bkgBinned", "Binned bkg", RooArgSet(aco), *bkg);

  // Create PDFs of binned datasets (templates)
  RooHistPdf fsrPDF("fsrPDF", "FSR PDF", aco, *fsrBinned);
  RooHistPdf bkgPDF("bkgPDF", "bkg PDF", aco, *bkgBinned);
  RooHistPdf signalPDF("sigPDF", "sig PDF", aco, *sigBinned);

  // Parameter initial values and ranges
  double sigExpected = (dataBinned->sumEntries())*.5;
  double fsrExpected = (dataBinned->sumEntries())*.28;
  //double bkgExpected = (dataBinned->sumEntries())*.28;
  double bkgExpected = (dataBinned->sumEntries())*.60;
  double max_sig = sigExpected + (20*sqrt(sigExpected));
  double min_sig = sigExpected - (20*sqrt(sigExpected));
  double max_fsr = fsrExpected + (20*sqrt(fsrExpected));
  double min_fsr = fsrExpected - (20*sqrt(fsrExpected));
  double max_bkg = bkgExpected + (20*sqrt(bkgExpected));
  double min_bkg = bkgExpected - (20*sqrt(bkgExpected));
  RooRealVar bkg_val("Bkg Initial Fraction","bkg",bkgExpected,min_bkg,max_bkg);
  RooRealVar sig_val("Sig Initial Fraction","sig",sigExpected,min_sig,max_sig);
  RooRealVar fsr_val("FSR Initial Fraction","fsr",fsrExpected,min_fsr,max_fsr);

  // Define the model (a 2 template fit)
  RooAddPdf model("model","model",RooArgList(signalPDF,bkgPDF),RooArgList(sig_val,bkg_val));
  RooAddPdf model2("model2","model2",RooArgList(signalPDF,bkgPDF,fsrPDF),RooArgList(sig_val,bkg_val,fsr_val));
  //RooAddPdf model("model","model",RooArgList(bkgPDF),RooArgList(bkg_val));

  // Minimize likelihood w.r.t all parameters before making plots
  model.fitTo(*dataBinned,Extended(kTRUE),RooFit::Save(kTRUE),SumW2Error(kTRUE));
  model2.fitTo(*dataBinned,Extended(kTRUE),RooFit::Save(kTRUE),SumW2Error(kTRUE));

  // Plotting
  TCanvas* c1 = new TCanvas("Canvas","Canvas",600,600); 
  c1->cd();
  gPad->SetLogx();
  gPad->SetLogy();
  RooPlot* frame = aco.frame(Title("Dimuon Acoplanarity"));
  dataBinned->plotOn(frame, Name("data"));
  model.plotOn(frame,Name("Fit"));
  model.plotOn(frame, Name("sig"), Components(RooArgList(signalPDF)), LineColor(kRed),LineStyle(1), LineWidth(4));
  model.plotOn(frame, Name("bkg"), Components(RooArgList(bkgPDF)), LineColor(kGreen),LineStyle(1), LineWidth(4));
  //model.paramOn(frame);
  //frame->GetYaxis()->SetRangeUser(0,10);
  TLegend* leg = new TLegend(0.75,0.55,0.85,0.85);
  leg->SetTextSize(0.037);
  leg->SetBorderSize(0);
  leg->SetLineStyle(0);
  leg->SetFillStyle(0);
  leg->SetFillColor(0);
  leg->AddEntry(frame->findObject("data"), "Data", "P");
  leg->AddEntry(frame->findObject("Fit"), "Fit", "l");
  leg->AddEntry(frame->findObject("sig"), "SL sig", "l");
  leg->AddEntry(frame->findObject("bkg"), "bkg", "l");
  frame->Draw();
  leg->Draw();
  c1->SaveAs("templateFit.png");
}
