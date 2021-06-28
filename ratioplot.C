// ratioplot.C

#include <iostream>
#include <fstream>
#include "TString.h"
#include "TTree.h"
#include "TBranch.h"
#include "TFile.h"
#include <TROOT.h>
#include <TStyle.h>
#include <TLine.h>
#include <TTree.h>
#include <TChain.h>
#include "TCanvas.h"
#include "TMath.h"
#include "TLegend.h"
#include "TH1.h"
#include "TF1.h"
#include "TPad.h"
#include "TLorentzVector.h"
using namespace std;

void ratioplot()
{
   // Retrieve histograms
  //TFile *file = new TFile("hist_aco.root");
  TFile *file = new TFile("hist_SL_aco.root");
  TH1F *h_mc = (TH1F*)file->Get("h1");
  // TFile *file = new TFile("hist_data_aco.root");
  TFile *file2 = new TFile("hist_data_aco.root");
  TH1F *h_data = (TH1F*)file2->Get("h1");
  TFile *file3 = new TFile("hist_mc_tautau.root");
  TH1F *h_mc2 = (TH1F*)file3->Get("h3");
  TH1F *h_ratio = (TH1F*)h_data->Clone();

  // Histogram Modifications
  h_mc->Scale(1/(h_mc->Integral()));
  h_data->Scale(1/(h_data->Integral()));
  h_mc2->Scale(1/(h_mc2->Integral()));

  h_mc->SetMarkerStyle(kFullDotLarge);
  h_mc->SetMarkerColor(kBlue);
  h_data->SetMarkerStyle(kFullDotLarge);
  h_data->SetMarkerColor(kRed);
  h_mc2->SetMarkerStyle(kFullDotLarge);
  h_mc2->SetMarkerColor(418);
  h_ratio->SetMarkerColor(kBlack);
  h_ratio->SetMarkerStyle(kFullDotLarge);

  // calculate data to mc ratio to plot
  float data_content;
  float mc_content;
  float ratio_val;
  for (int ibin=1; ibin<h_data->GetNbinsX()+1; ibin++)
    {
      data_content = h_data->GetBinContent(ibin);
      mc_content = h_mc->GetBinContent(ibin);
      if(data_content > 0 && mc_content > 0)
	{
	  ratio_val = (data_content/mc_content);
	  h_ratio->SetBinContent(ibin,ratio_val);
	}
      else
	{
	  h_ratio->SetBinContent(ibin,0.0);
	}
    }

  // Fits
  TF1 *fit = new TF1("fit","[0]*exp(-x/[1])",0,0.1);
  fit->SetParName(0,"A_s");
  fit->SetParName(1,"alpha_s");
  fit->SetParLimits(0,-10,10);
  fit->SetParLimits(1,0.00001,10);
  fit->SetLineColor(kBlack);
  h_mc->Fit(fit,"B");
  float alpha_0 = fit->GetParameter(1);

  TF1 *fit2 = new TF1("fit2","[0]*exp(-x/[1])+[2]*exp(-x/[3])",0,1.5);
  fit2->SetParName(0,"A_s");
  fit2->SetParName(1,"alpha_s");
  fit2->SetParName(2,"A_b");
  fit2->SetParName(3,"alpha_b");
  fit2->SetParameter(1,alpha_0);
  fit2->SetParLimits(2,-50,50);
  fit2->SetParLimits(3,0.00001,50);
  fit2->SetLineColor(kBlack);
  h_data->Fit(fit2,"B");
  float A_s = fit2->GetParameter(0);
  float alpha_s = fit2->GetParameter(1);
  float A_b = fit2->GetParameter(2);
  float alpha_b = fit2->GetParameter(3);

  // Plotting
  auto *c1 = new TCanvas("c1","Title",600,600);
  TPad *main = new TPad("main", "newpad",0.01,0.315,0.99,0.99);
  TPad *ratio = new TPad("ratio", "newpad2",0.01,0.01,0.99,0.38);

  TLegend* legend = new TLegend(0.3,0.8,0.2,0.7);
  legend->AddEntry(h_mc, "Starlight", "P");
  legend->AddEntry(h_data, "Data", "P");
  legend->SetBorderSize(1);
  
  main->Draw();
  ratio->Draw();
  main->cd();
  main->SetLogy();
  main->SetLogx();
  h_data->GetYaxis()->SetTitle("Events (Hist Norm to Unity)");
  h_data->GetXaxis()->SetRangeUser(0.00055,1.2);
  h_data->GetXaxis()->SetLabelOffset(9999999);
  h_data->Draw("PSAME");
  h_mc->Draw("PSAME");
  fit2->Draw("SAME");
  legend->Draw("SAME");
  ratio->cd();
  h_ratio->SetStats(0);
  ratio->SetLogx();
  TLine *line = new TLine(0,1,1.5,1);
  line->SetLineColor(kGreen+1);
  line->SetLineWidth(4);
  h_ratio->GetYaxis()->SetTitleFont(22);
  h_ratio->GetYaxis()->SetTitle("data/mc");
  h_ratio->GetXaxis()->SetTitle("Acoplanarity");
  h_ratio->GetYaxis()->SetRangeUser(-0.2,1.4);
  h_ratio->SetTitle("");
  h_ratio->Draw("PSAME");
  line->Draw();
  c1->SaveAs("ratioplot.png");
}
