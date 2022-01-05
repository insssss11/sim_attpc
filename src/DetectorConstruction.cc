//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"
#include "GasChamberSD.hh"

#include "G4Exception.hh"

#include "G4TransportationManager.hh"
#include "G4Mag_UsualEqRhs.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"

#include "G4RotationMatrix.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"

#include "G4FieldManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"

#include "G4Colour.hh"

#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal MagneticField *DetectorConstruction::fMagneticField = 0;
G4ThreadLocal G4FieldManager *DetectorConstruction::fFieldManager = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
    : G4VUserDetectorConstruction(),
    fMessenger(nullptr), fLogicWorld(nullptr), fLogicGas(nullptr), fLogicChamber(nullptr),
    fVisAttributes(),
    fGasMat(nullptr),
    fGasName1("He"), fGasName2("iC4H10"), fFrac1(90.), fFrac2(10.), fPressure(0.1*atmosphere),
    fNbOfGasMat(0),
    fGasMatMap(nullptr)
{
    fGasMatMap = new std::unordered_map<std::string, std::string>;
    // Construct materials
    ConstructMaterials();
    // initiaize gas material
    SetGas(fGasName1, fFrac1, fGasName2, fFrac2, fPressure);
    fMessenger = new DetectorConstructionMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
    delete fMessenger;
    delete fLogicWorld;
    delete fLogicGas;
    delete fLogicMag;
    // delete fLogicChamber;
    for(auto visAttributes : fVisAttributes)
        delete visAttributes;
    // pointers to G4Material of the map are deleted at the end of the program.
    delete fGasMatMap;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume *DetectorConstruction::Construct()
{
    auto vacuum = G4Material::GetMaterial("Vacuum");
    // Option to switch on/off checking of volumes overlaps
    G4bool checkOverlaps = true;

    // geometries --------------------------------------------------------------
    auto solidWorld = new G4Box("SolidWorld", 500 * mm, 500 * mm, 500 * mm);
    fLogicWorld = new G4LogicalVolume(solidWorld, vacuum, "LogicWorld");
    auto physWorld = new G4PVPlacement(
        0, G4ThreeVector(),
        fLogicWorld, "PhysWorld", 0,
        false, 0, checkOverlaps);

    // Tube with Local Magnetic field

    auto solidMag = new G4Tubs("SolidMag", 0., 100 * mm, 100 * mm, 0., 360. * deg);
    fLogicMag = new G4LogicalVolume(solidMag, fGasMat, "LogicMag");
    fLogicGas = fLogicMag; // For now, magnetic field volume and gas volume completly coincide.
    // placement of Tube
    G4RotationMatrix *fieldRot = new G4RotationMatrix();
    fieldRot->rotateY(90. * deg);
    new G4PVPlacement(fieldRot, G4ThreeVector(),
        fLogicMag, "PhysMagnetic", fLogicWorld,
        false, 0, checkOverlaps);

    // set step limit in tube with magnetic field  
    G4UserLimits *userLimits = new G4UserLimits(1 * mm, 300*mm);
    fLogicMag->SetUserLimits(userLimits);

    // visualization attributes ------------------------------------------------
    auto visAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
    visAttributes->SetVisibility(false);
    fLogicWorld->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);

    visAttributes = new G4VisAttributes(G4Colour(0.9, 0.9, 0.9));   // LightGray
    fLogicMag->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);
    // return the world physical volume ----------------------------------------
    return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
    // sensitive detectors -----------------------------------------------------
    auto sdManager = G4SDManager::GetSDMpointer();
    G4String SDname;

    auto gasChamberSD = new GasChamberSD(SDname = "/gasChamber");
    sdManager->AddNewDetector(gasChamberSD);
    fLogicGas->SetSensitiveDetector(gasChamberSD);
    // magnetic field ----------------------------------------------------------
    fMagneticField = new MagneticField();
    fFieldManager = new G4FieldManager();
    fFieldManager->SetDetectorField(fMagneticField);
    fFieldManager->CreateChordFinder(fMagneticField);
    G4bool forceToAllDaughters = true;
    fLogicMag->SetFieldManager(fFieldManager, forceToAllDaughters);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructMaterials()
{
    auto nist = G4NistManager::Instance();

    // Vacuum "Air with low density"
    nist->FindOrBuildMaterial("G4_AIR");
    auto air = G4Material::GetMaterial("G4_AIR");
    nist->BuildMaterialWithNewDensity("Vacuum", "G4_AIR", 1.0e-7*air->GetDensity());
    
    // ref [1] : Coupled cluster calculations of mean excitation energies of the noble gas atoms He, Ne and Ar and of the H2 molecule
    // ref [2] : Techiniques for Nuclear and Particle Physics Experiments 
    // ref [3] : Mean excitation energies and energy deposition characteristics of bio-organic molecules.
    // ref [4] : Geant4 simulation of DRAGON's hybrid detector

    // gas definition
    G4Material *He = nist->FindOrBuildMaterial("G4_He");
    He->GetIonisation()->SetMeanExcitationEnergy(41.8*eV); // ref [1]
    He->GetIonisation()->SetMeanEnergyPerIonPair(41.*eV); // ref[2]
    RegisterGasMat("He", He->GetName());

    G4Material *Ne = nist->FindOrBuildMaterial("G4_Ne");
    Ne->GetIonisation()->SetMeanExcitationEnergy(137.2*eV); // ref [1]
    Ne->GetIonisation()->SetMeanEnergyPerIonPair(36*eV); // ref [2]
    RegisterGasMat("Ne", Ne->GetName());    

    G4Material *Ar = nist->FindOrBuildMaterial("G4_Ar");
    Ar->GetIonisation()->SetMeanExcitationEnergy(188.5*eV); // ref [1]
    Ar->GetIonisation()->SetMeanEnergyPerIonPair(26*eV); // ref [2]
    RegisterGasMat("Ar", Ar->GetName());

    G4Material *CO2 = nist->FindOrBuildMaterial("G4_CARBON_DIOXIDE");
    CO2->GetIonisation()->SetMeanExcitationEnergy(90.0*eV); // ref [3]
    CO2->GetIonisation()->SetMeanEnergyPerIonPair(33*eV); // ref[2]
    RegisterGasMat("CO2", CO2->GetName());    

    G4Material *CH4 = nist->FindOrBuildMaterial("G4_METHANE");
    CH4->GetIonisation()->SetMeanExcitationEnergy(43.3*eV); // ref [3]
    CH4->GetIonisation()->SetMeanEnergyPerIonPair(28*eV); // ref [2]
    RegisterGasMat("CH4", CH4->GetName());

    G4Material *iC4H10 = nist->FindOrBuildMaterial("G4_BUTANE");
    iC4H10->GetIonisation()->SetMeanExcitationEnergy(48.3*eV); // ref [4]
    iC4H10->GetIonisation()->SetMeanEnergyPerIonPair(23*eV); // ref [2]
    RegisterGasMat("iC4H10", iC4H10->GetName());

    G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;
    G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetGas(const G4String &gas1, G4double frac1, const G4String &gas2, G4double frac2, G4double pressure)
{
    G4Material *gasMat1 = FindGasMat(gas1);
    G4Material *gasMat2 = FindGasMat(gas2);
    if(gasMat1 == nullptr || gasMat2 == nullptr)
    {
        return;
    }
    else if((frac1 + frac2 - 100)*(frac1 + frac2 - 100)/10000 > 1e-6)
    {
        std::ostringstream message;
        message << "Sum of frac1 and frac2 is not close enough to 100%%.";
        G4Exception("DetectorConstruction::SetGas(const G4String &gas1, G4double frac1, const G4String &gas2, G4double frac2, G4double pressure)",
            "GeomGasMat0000", JustWarning, message);
        return;    
    }
    else{
        // update variables
        fGasName1 = gas1;
        fGasName2 = gas2;
        fFrac1 = frac1;
        fFrac2 = frac2;
        fPressure = pressure;
    }
    G4double density1 = gasMat1->GetDensity()*fFrac1*perCent;
    G4double density2 = gasMat2->GetDensity()*fFrac2*perCent;
    // calculate density at given pressure
    G4double density = (density1 + density2)*fPressure/atmosphere;
    // calcalate mass fraction
    G4double massFrac1 = density1/(density1 + density2);
    G4double massFrac2 = density2/(density1 + density2);
    ++fNbOfGasMat;
    
    // warning message if too many instances
    if(fNbOfGasMat > kNbOfGatMatWarning)
    {
        std::ostringstream message;
        message << "Changing gas properties creates new G4Material instances. It may occupy large memory.";
        G4Exception("DetectorConstruction::DetectorConstruction::UpdateGas()",
            "GeomGasMat0001", JustWarning, message);
        return;        
    }
    fGasMat = new G4Material("Gas", density, 2);
    fGasMat->AddMaterial(gasMat1, massFrac1);
    fGasMat->AddMaterial(gasMat2, massFrac2);
    if(fLogicGas != nullptr)
    {
        fLogicGas->SetMaterial(fGasMat);
        // tell G4RunManager that we change the geometry
        G4RunManager::GetRunManager()->PhysicsHasBeenModified();
        G4cout << "Gas mixture changed to " << GetGasMixtureStat() << G4endl;          
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::RegisterGasMat(const G4String &key, const G4String &val)
{
    fGasMatMap->insert(std::make_pair(key.data(), val.data()));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Material *DetectorConstruction::FindGasMat(const G4String &key)
{
    if(fGasMatMap->find(key) == fGasMatMap->end())
    {
        std::ostringstream message;
        message << "Cannot find gas material " << key << ".";
        G4Exception("DetectorConstruction::FindGasMat(const G4String &key)",
            "GeomGasMat0002", JustWarning, message);
        return nullptr;
    }
    else
        return G4NistManager::Instance()->FindMaterial(fGasMatMap->at(key));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String DetectorConstruction::GetGasMixtureStat()
{
    char strFrac1[32], strFrac2[32], strPressure[32];
    sprintf(strFrac1, "(%2.0f%%)/", fFrac1);
    sprintf(strFrac2, "(%2.0f%%) at ", fFrac2);
    sprintf(strPressure, "%1.2f atm.", fPressure/atmosphere);
    return fGasName1 + strFrac1
        + fGasName2 +  strFrac2 + strPressure;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......