#include "config/ParamContainerTable.hh"
#include "training_data/GenTrainingData.hh"

#include <iostream>
#include <string>

#include "TFile.h"
#include "TTree.h"

using namespace std;

void PrintUsage()
{
    cout << "Usage : gen_training_data <name prefix> [<parameter configure name> = parameters/config.txt]" << endl;
}

// this program read root file generated by pad_charges.mac Geant4 macro and generate traning data file.
int main(int argc, char **argv)
{

    string prefix, parFileName = "parameters/config.txt";
    if(argc == 2)
        prefix = argv[1];
    else if(argc == 3)
    {
        prefix = argv[1];
        parFileName = argv[2];
    }
             
    ParamContainerTable::GetBuilder()->BuildFromConfigFile(parFileName);
    ParamContainerTable::Instance()->DumpTable();

    GenTrainingData genTrain(prefix + "_evt.root", prefix + "_bg.root");
    genTrain.SetNtrkSecondaryMax(4);
    genTrain.WriteTrainingData("training_input.dat", "training_output.dat");
    return -1;
}