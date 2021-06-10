// analyze_aco.C

#include <iostream>
#include <fstream>
#include "TString.h"
#include "TTree.h"
#include "TBranch.h"
#include "TFile.h"
#include <TTree.h>
#include <TChain.h>
#include "TCanvas.h"
#include "THStack.h"
#include "TMath.h"
#include "TLegend.h"
#include "TF1.h"
#include "TH1.h"
#include "TLorentzVector.h"
using namespace std;

void analyze_aco()
{
  // Retrieve histograms
  TFile *file = new TFile("hist_aco.root");
  TH1F *h_mc = (TH1F*)file->Get("h1");
  TH1F *h_data = (TH1F*)file->Get("h2");

  // Histogram Modifications
  h_mc->Scale(1/(h_mc->Integral()));
  h_data->Scale(1/(h_data->Integral()));

  h_mc->SetMarkerStyle(kFullDotLarge);
  h_mc->SetMarkerColor(kBlue);
  h_data->SetMarkerStyle(kFullDotLarge);
  h_data->SetMarkerColor(kRed);

  // Fits
  TF1 *fit = new TF1("fit","[0]*exp(-x/[1])",0,0.05);
  fit->SetParName(0,"A_s");
  fit->SetParName(1,"alpha_s");
  fit->SetParLimits(0,-10,10);
  fit->SetParLimits(1,0.00001,10);
  fit->SetLineColor(kBlack);
  h_mc->Fit(fit,"B");
  float alpha_s = fit->GetParameter(1);

  TF1 *fit2 = new TF1("fit2","[0]*exp(-x/[1])+[2]*exp(-x/[3])",0,0.05);
  fit2->SetParName(0,"A_s");
  fit2->SetParName(1,"alpha_s");
  fit2->SetParName(2,"A_b");
  fit2->SetParName(3,"alpha_b");
  fit2->SetParameter(1,alpha_s);
  fit2->SetParLimits(2,-10,10);
  fit2->SetParLimits(3,0.00001,10);
  fit2->SetLineColor(kBlack);
  h_data->Fit(fit2,"B");

  // Plotting
  TCanvas *c1 = new TCanvas("c1","stacked hists",1000,800);
  TLegend* legend = new TLegend(0.3,0.7,0.2,0.6);
  legend->AddEntry(h_mc, "Starlight", "P");
  legend->AddEntry(h_data, "Data", "P");
  legend->SetBorderSize(1);
  c1->SetLogy();
  c1->SetLogx();
  h_mc->Draw("P");
  h_mc->GetXaxis()->SetTitle("1 - #frac{#Delta #phi}{#pi}");
  h_mc->GetYaxis()->SetTitle("Entries (Distributions Normalized to 1)");
  h_mc->GetXaxis()->CenterTitle();
  h_mc->GetYaxis()->CenterTitle();
  h_data->Draw("PSAME");
  legend->Draw("SAME");
  c1->SaveAs("stacks.pdf");
}
