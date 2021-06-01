// aco2.C
// This macro puts the 2018 dimuon LbyL root files in a chain and makes an
// acoplanarity histogram.

#include <iostream>
#include <fstream>
#include "TString.h"
#include "TTree.h"
#include "TFile.h"
#include <TTree.h>
#include <TChain.h>
#include "TMath.h"
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


void aco2()
{
  //Set up TChain
  string inputfile = "/eos/user/c/clemahie/TauTau/data/data_mu_mu/*.root";
  string outputfile = "dimuon_aco.root";
  TChain* chain = new TChain("ggHiNtuplizer/EventTree");
  chain->Add(inputfile.c_str());

  int entries = chain->GetEntries();
  cout << "Entries in chain = " << entries << endl;

  float diLepton_aco = 0;
  int nMu = 0;
  vector<float> *muPt = nullptr;
  vector<float> *muEta = nullptr;
  vector<float> *muPhi = nullptr;

  chain->SetBranchStatus("*",0);
  chain->SetBranchStatus("nMu",1);
  chain->SetBranchStatus("muPt",1);
  chain->SetBranchStatus("muEta",1);
  chain->SetBranchStatus("muPhi",1);
  chain->SetBranchAddress("nMu",&nMu);
  chain->SetBranchAddress("muPt",&muPt);
  chain->SetBranchAddress("muEta",&muEta);
  chain->SetBranchAddress("muPhi",&muPhi);
  TFile newfile(outputfile.c_str(),"recreate");
  auto newtree = chain->CloneTree(1);
  
  TH1F *h1 = new TH1F("h1","Dimuon Acoplanarity",100,0.,0.05);

  //Loop through the events
  int muon_total = 0;
  for(int i=0; i<entries; i++)
    {
      chain->GetEntry(i);
      muon_total += nMu;

      diLepton_aco = GetAcoplanarity(muPhi->at(0), muPhi->at(1));
      newtree->Fill();
      h1->Fill(diLepton_aco);
    }

  h1->GetXaxis()->SetTitle("1 - #frac{#Delta #phi}{#pi}");
  h1->GetYaxis()->SetTitle("Entries");
  h1->GetXaxis()->CenterTitle();
  h1->GetYaxis()->CenterTitle();
  newfile.Write();
  cout << "Total number of muons is " << muon_total << endl;
}
