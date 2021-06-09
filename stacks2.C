// stacks2.C

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
#include "TH1.h"
#include "TLorentzVector.h"
using namespace std;


//Acoplanarity Function
float GetAcoplanarity(float Phi_1, float Phi_2)
{
  float phi1 = Phi_1;
  float phi2 = Phi_2;

  //Make sure the angles are in range [0, 2pi)
  while (phi1 < 0)
    {
      phi1 += 2*TMath::Pi();
    }
  while (phi1 > 2*TMath::Pi())
    {
      phi1 -= 2*TMath::Pi();
    }
  while (phi2 < 0)
    {
      phi2 += 2*TMath::Pi();
    }
  while (phi2 > 2*TMath::Pi())
    {
      phi2 -= 2*TMath::Pi();
    }

  float deltaPhi = fabs(phi2-phi1);

  //Make sure deltaPhi is in range [0, pi)
  if (deltaPhi > TMath::Pi())
    {
      deltaPhi = 2*TMath::Pi() - deltaPhi;
    }

  float aco = 1 - deltaPhi/TMath::Pi();
  return aco;
}


void stacks2()
{
  
  //Set up TChain for MC
  string inputfile_mc = "/eos/user/c/clemahie/TauTau/mc/mc_mu_mu/*.root"; 
  //string outputfile = "dimuon_aco_mc.root";
  TChain* chain_mc = new TChain("ggHiNtuplizer/EventTree");
  chain_mc->Add(inputfile_mc.c_str());

  int entries_mc = chain_mc->GetEntries();
  cout << "Entries in MC chain = " << entries_mc << endl;

  float diLepton_aco_mc = 0;
  int nMu_mc = 0;
  vector<float> *muPt_mc = nullptr;
  vector<float> *muEta_mc = nullptr;
  vector<float> *muPhi_mc;
  TBranch *b_mcphi;
  muPhi_mc=0; 

  chain_mc->SetBranchStatus("*",0);
  chain_mc->SetBranchStatus("nMu",1);
  chain_mc->SetBranchStatus("muPt",1);
  chain_mc->SetBranchStatus("muEta",1);
  chain_mc->SetBranchStatus("muPhi",1);
  chain_mc->SetBranchAddress("nMu",&nMu_mc);
  chain_mc->SetBranchAddress("muPt",&muPt_mc);
  chain_mc->SetBranchAddress("muEta",&muEta_mc);
  chain_mc->SetBranchAddress("muPhi",&muPhi_mc, &b_mcphi);
  //TFile newfile(outputfile.c_str(),"recreate");
  //auto newtree = chain->CloneTree(1);

  TH1F *h1 = new TH1F("h1","Dimuon Acoplanarity",100,0.,0.05);
  //Loop through the mc events
  //int muon_total = 0;
  for(int i=0; i<entries_mc; i++)
    {
      chain_mc->GetEntry(i);
      //muon_total += nMu;

      diLepton_aco_mc = GetAcoplanarity(muPhi_mc->at(0), muPhi_mc->at(1));
      h1->Fill(diLepton_aco_mc);
    }

  
  //Set up the data TChain
  string inputfile = "/eos/user/c/clemahie/TauTau/data/data_mu_mu/*.root"; 
  TChain* chain = new TChain("ggHiNtuplizer/EventTree");
  chain->Add(inputfile.c_str());

  int entries = chain->GetEntries();
  cout << "Entries in data chain = " << entries << endl;

  float diLepton_aco = 0;
  int nMu = 0;
  vector<float> *muPt = nullptr;
  vector<float> *muEta = nullptr;
  vector<float> *muPhi;
  TBranch *b_phi;
  muPhi = 0;

  chain->SetBranchStatus("*",0);
  chain->SetBranchStatus("nMu",1);
  chain->SetBranchStatus("muPt",1);
  chain->SetBranchStatus("muEta",1);
  chain->SetBranchStatus("muPhi",1);
  chain->SetBranchAddress("nMu",&nMu);
  chain->SetBranchAddress("muPt",&muPt);
  chain->SetBranchAddress("muEta",&muEta);
  chain->SetBranchAddress("muPhi",&muPhi, &b_phi);
  
  TH1F *h2 = new TH1F("h2","Dimuon Acoplanarity",100,0.,0.05);
  
  //Loop through the data events
  int muon_total = 0;
  for(int i=0; i<entries; i++)
    {
      chain->GetEntry(i);
      //muon_total += nMu;

      diLepton_aco = GetAcoplanarity(muPhi->at(0), muPhi->at(1));
      h2->Fill(diLepton_aco);
    }

  // Histogram Modifications
  h1->SetMarkerStyle(kFullDotLarge);
  h1->SetMarkerColor(kBlue);
  h2->SetMarkerStyle(kFullDotLarge);
  h2->SetMarkerColor(kRed);

  float lum = 1.5;
  float cross = 320000;
  h1->Scale(1/(h1->Integral()));
  h2->Scale(1/(h2->Integral()));

  //Plot
  TCanvas *c1 = new TCanvas("c1","stacked hists",1000,800);
  TLegend* legend = new TLegend(0.9,0.7,0.8,0.6);
  legend->AddEntry(h1, "Starlight", "P");
  legend->AddEntry(h2, "Data", "P");
  legend->SetBorderSize(1);
  c1->SetLogy();
  
  h2->GetXaxis()->SetLimits(0,0.02);
  h2->Draw("P");
  h2->GetXaxis()->SetTitle("1 - #frac{#Delta #phi}{#pi}");
  h2->GetYaxis()->SetTitle("Entries (Distributions Normalized to 1)");
  h2->GetXaxis()->CenterTitle();
  h2->GetYaxis()->CenterTitle();
  h1->Draw("PSAME");
  legend->Draw("SAME");
  c1->SaveAs("stacks.pdf");
  // cout << "Total number of muons is " << muon_total << endl;
}

