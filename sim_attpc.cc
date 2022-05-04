/// \file sim_attpc.cc
/// \brief Main program of the sim_attpc

#include "detector_construction/DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "PhysicsList.hh"
#include "config/ParamContainerTable.hh"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "G4RunManagerFactory.hh"

#include "G4UImanager.hh"
#include "G4StepLimiterPhysics.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4ios.hh"

#include "Randomize.hh"
void PrintUsage()
{
    G4cout << "Usage : ./sim_attpc [-t nThreads] [-m macroName] [-s seed] [-p configure file]" << G4endl;
}

long GetRandomLong()
{
    int fd;
    long seed;
    if((fd = open("/dev/random", O_RDONLY)) == -1)
    {
        perror("Failed open /dev/random to generate a random seed.");
        exit(1);
    }
    if((read(fd, (void*)&seed, 4)) == -1)
    {
        perror("Failed open /dev/random to generate a random seed.");
        exit(1);
    }
    return seed;
}


int main(int argc, char **argv)
{
    // variables dependent to user inputs
    int nThreads = 0;
    long seed = GetRandomLong();           // # of threads , seed
    G4String macroName = "";    // if left empty, run as interactive mode.
    G4String configureFile = "parameters/config.txt";
    for(int i = 1; i < argc;i += 2)
    {
        if(G4String(argv[i]) == "-t")
            nThreads = atoi(argv[i + 1]);
        else if(G4String(argv[i]) == "-m")
            macroName = argv[i + 1];
        else if(G4String(argv[i]) == "-s")
            seed = atoi(argv[i + 1]);
        else if(G4String(argv[i]) == "-p")
            configureFile = argv[i + 1];
        else if(G4String(argv[i]) == "--help")
        {
            PrintUsage();
            return -1;
        }
        else
        {
            G4cerr << "Invalid argument : " << argv[i] << G4endl;
            return -1;
        }
    }
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
    CLHEP::HepRandom::setTheSeed(seed);
    // Load parameter files
    ParamContainerTable::GetBuilder()->BuildFromConfigFile(configureFile);
    ParamContainerTable::Instance()->DumpTable();

    // Construct the default run manager (sequential or multi-thread mode)
    // running with a single thread has no meaning
    G4RunManagerType runType;
    if(nThreads > 1)
        runType = G4RunManagerType::MTOnly;
    else
        runType = G4RunManagerType::SerialOnly;
    auto *runManager = G4RunManagerFactory::CreateRunManager(runType, nThreads);

    // Mandatory user initialization classes
    runManager->SetUserInitialization(new DetectorConstruction);

    runManager->SetUserInitialization(new PhysicsList);

    // User action initialization
    runManager->SetUserInitialization(new ActionInitialization);

    // Visualization manager construction
    auto visManager = new G4VisExecutive;
    // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
    // G4VisManager* visManager = new G4VisExecutive("Quiet");
    visManager->Initialize();

    // Get the pointer to the User Interface manager
    auto UImanager = G4UImanager::GetUIpointer();
    // execute macro for initialization
    UImanager->ApplyCommand("/control/execute init.mac");

    G4UIExecutive *ui = nullptr;
    if(!macroName.empty())
    {
        // execute an argument macro file if exist
        G4String command = "/control/execute ";
        UImanager->ApplyCommand(command + macroName);
    }
    else
    {
        ui = new G4UIExecutive(argc, argv);
        UImanager->ApplyCommand("/control/execute vis.mac");
        // start interactive session
        ui->SessionStart();
        delete ui;
    }
    // Job termination
    // Free the store: user actions, physics_list and detector_description are
    // owned and deleted by the run manager, so they should not be deleted 
    // in the main() program !
    delete visManager;
    delete runManager;
}