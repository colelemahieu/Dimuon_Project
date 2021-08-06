// ZDCroofit.C

#include <iostream>
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooExponential.h"
#include "RooChebychev.h"
#include "RooPolynomial.h"
#include "RooAddPdf.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TAxis.h"
#include "RooPlot.h"
using namespace RooFit;
using namespace std;

void ZDCroofit()
{
  // Declare observable x
  RooRealVar ZDC_minus("ZDC_minus", "ZDC_minus", 0, 10000);

  // Create two Gaussian PDFs, an Exponential PDF, and their parameters
  RooRealVar mean("mean", "mean of gaussians",2600, 1800,4000);
  RooFormulaVar mean2("mean2","mean*2",RooArgSet(mean));
  RooFormulaVar mean3("mean3","mean*3",RooArgSet(mean));
  RooRealVar sigma1("sigma1", "width of gaussians",550, 100,1400);
  RooFormulaVar sigma2("sigma2","sigma1*sqrt(2)",RooArgSet(sigma1));
  RooFormulaVar sigma3("sigma3","sigma1*sqrt(3)",RooArgSet(sigma1));
  RooRealVar alpha0("alpha0", "exponetial decay", -0.00695454, -1, 1);
  RooExponential sig0("sig0", "Signal compoent 0", ZDC_minus, alpha0);
  RooGaussian sig1("sig1", "Signal component 1", ZDC_minus, mean, sigma1);
  RooGaussian sig2("sig2", "Signal component 2", ZDC_minus, mean2, sigma2);
  RooGaussian sig3("sig3", "Signal component 3", ZDC_minus, mean3, sigma3);

  // Build Chebychev polynomial pdf
  //RooRealVar a0("data_a0", "a0", 4.59207e-06, 0., 1);
  //RooRealVar a1("data_a1", "a1", 0.0033976, 0., 1);
  //RooPolynomial bkg("bkg", "Background", ZDC_minus, RooArgSet());

  // Sum the signal components into a composite signal pdf
  RooRealVar gaussfrac("gaussfrac", "fraction of gaussian components", 0.5, 0., 1.);
  RooAddPdf gauss_sum("gauss_sum", "Sum of 1n and 2n peaks", RooArgList(sig1,sig2), gaussfrac);
  RooRealVar gaussfrac2("gaussfrac2", "fraction of gaussian components", 0.5, 0., 1.);
  RooAddPdf gauss_sum2("gauss_sum2", "Sum of 1n,2n,3n peaks", RooArgList(gauss_sum,sig3), gaussfrac2);
  RooRealVar sigfrac("sigfrac", "fraction of guassian peaks and 0n exponetial", 0.9, 0., 1.);   
  RooAddPdf model("model", "Sum of guassian peaks and 0n exponetial", RooArgList(sig0,gauss_sum2), sigfrac);

  // Import Data
  string inputfile = "hist_data_ZDC.root";
  TFile* file = TFile::Open(inputfile.c_str(),"READ");
  TTree *eventTree = (TTree*)file->Get("EventTree");
  RooDataSet data("data", "data", RooArgSet(ZDC_minus), Import(*eventTree));

  // Fit model to data
  ZDC_minus.setRange("signal",500,10000);
  model.fitTo(data,Range("signal"));

  // Plot data and PDF overlaid
  RooPlot *xframe = ZDC_minus.frame(Title("ZDC- Fit"));
  data.plotOn(xframe);
  model.plotOn(xframe);

  // Overlay the components of model with a dotted line
  model.plotOn(xframe, Components(RooArgSet(sig3)), LineStyle(kDotted), LineColor(kPink));
  model.plotOn(xframe, Components(RooArgSet(sig2)), LineStyle(kDotted), LineColor(kBlue));
  model.plotOn(xframe, Components(RooArgSet(sig1)), LineStyle(kDotted), LineColor(kGreen+1));
  model.plotOn(xframe, Components(RooArgSet(sig0)), LineStyle(kDotted), LineColor(kRed)); 

  // Print structure of composite pdf
  model.Print("t");

  // Calculate Integrals
  double x0 = 1125;
  double xmax = 4200;
  ZDC_minus.setRange("my_range", x0, xmax);
  RooAbsReal *integral = sig1.createIntegral(ZDC_minus, NormSet(ZDC_minus), Range("my_range"));
  //RooAbsReal *integral = sig0.createIntegral(ZDC_minus);
  cout << "integraaaaaaaaaaaaaaaaaaaaaaaal = " << integral->getVal() << endl;
  
  // Draw the frame on the canvas
  new TCanvas("rf201_composite", "rf201_composite", 600, 600);
  gPad->SetLeftMargin(0.15);
  gPad->SetLogy();
  xframe->GetYaxis()->SetTitleOffset(1.4);
  xframe->Draw();
  gPad->SaveAs("ZDCminusFit.png");
}
