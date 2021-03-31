// get muon p_t

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
using namespace std;
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

void plot()
{
  TFile *input = new TFile("data_HiForwardAOD_1-6048.root");
  //TTree *trig_tree = (TTree*)input->Get("hltanalysis/HltTree");
  TTree *evt_tree = (TTree*)input->Get("ggHiNtuplizer/EventTree");

  int muontrigger;
  vector<Float_t> *mu_pt;
  //trig_tree->SetBranchAddress("HLT_HIUPC_SingleMuOpen_NotMBHF2AND_v1",&muontrigger);
  TBranch*b_mupt;
  mu_pt=0;
  evt_tree->SetBranchAddress("muPt", &mu_pt, &b_mupt);

  int entries = evt_tree->GetEntries();
  cout << entries << endl;
  //int entries2 = trig_tree->GetEntries();
  //cout << entries2 << endl;
  for (int i=0; i<entries; i++)
    {
      //trig_tree->GetEntry(i);
      evt_tree->GetEntry(i);
      //cout<<muontrigger<<endl;
      if (muontrigger==1)
	{
	  //cout << mu_pt << endl;
	}
      //cout<<mu_pt->size()<<endl;
      for (unsigned int muIter=0; muIter<mu_pt->size(); muIter++)cout<< "muPt=:" << mu_pt->at(muIter)<<endl;
    }

  
}
