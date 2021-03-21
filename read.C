// This script reads the copied mess from tree->Print(), picks out trigger names, and puts them in a clean file

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
using namespace std;

void read()
{
  ifstream file_mess("print.txt");
  ofstream outfile("triggers_clean.txt");
  string trig_name;

  while (file_mess >> trig_name)
    {
      if ((trig_name.find("HLT") != std::string::npos)) //|| (trig_name.find("L1") != std::string::npos))
	{
	  if(trig_name.at(0) == ':')
	    {
	      trig_name.erase(0,1);
	      outfile << trig_name << endl;
	    }
	}
    }

  outfile.close();
}
