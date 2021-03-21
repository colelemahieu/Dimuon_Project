// This script plots triggers 

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
using namespace std;

void triggerplot()
{
  ifstream cleanfile("triggers_clean.txt");
  vector<string> hltnames;
  string hltname;
  int nTriggers = 0;

  // Puts our file of desired triggers into string array
  while(getline(cleanfile, hltname))
    {
      hltnames.push_back(hltname);
      nTriggers += 1;
    }

  int hltarraysize = hltnames.size();
  int hltarray[hltarraysize];
  vector<int> *triggers = nullptr;
  int trigger_total = 0;

  TFile *input = new TFile("data_HiForwardAOD_1-1.root");
  TTree *tree = (TTree*)input->Get("hltanalysis/HltTree");

  // Read in the desired trigger counts from ROOT file into integer array
  for (int i=0; i < hltarraysize; i++)
    {
      hltarray[i]=0;
      tree->SetBranchAddress(hltnames.at(i).c_str(), &hltarray[i]);
    }

  int events = tree->GetEntries();

  // Set up the output ROOT file
  TFile *output = new TFile("created.root", "recreate");
  TTree *triggertree = new TTree("triggertree", "Tree with Trigger Info");
  triggertree->Branch("triggers", &triggers);
  triggertree->Branch("TriggerTotal", &trigger_total);

  TH1F *h1 = new TH1F("h1", " ", nTriggers, 0, nTriggers);

  // Trigger Loop
  for (int i=0; i < events; i++)
    {
      trigger_total = 0;
      triggers->clear();
      tree->GetEntry(i);

      for (int j=0; j < hltarraysize; j++)
	{
	  if (hltarray[j] > 0)
	    {
	      triggers->push_back(hltarray[j]*(j+1));
	      h1->Fill(hltarray[j] * (j + 1));
	      trigger_total += hltarray[j];
	    }
	}
      triggertree->Fill();
    }

  h1->SetTitle("Trigger Information");
  h1->GetXaxis()->SetTitle("Triggers");
  h1->GetYaxis()->SetTitle("Count");
  h1->GetXaxis()->CenterTitle();
  h1->GetYaxis()->CenterTitle();
  h1->Write();

  // histogram key
  ofstream out2("Trigger_numbers.txt");
  for(int i=0; i < nTriggers; i++)
  {
    int tmp_bin = h1->GetXaxis()->FindBin(i+1);
    out2 << hltnames.at(i) << " Events    " << h1->GetBinContent(tmp_bin) << endl;
  }

  triggertree->Write();

  cout << nTriggers << endl;
  
}
