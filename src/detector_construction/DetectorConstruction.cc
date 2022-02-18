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

#include "detector_construction/DetectorConstruction.hh"
#include "gas_chamber/GasChamberSD.hh"
#include "config/ParamContainerTable.hh"

#include "G4Exception.hh"

#include "G4TransportationManager.hh"
#include "G4Mag_UsualEqRhs.hh"

#include "G4Element.hh"
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"

#include "G4MultiUnion.hh"
#include "G4SubtractionSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4ExtrudedSolid.hh"

#include "G4FieldManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"

#include "G4Colour.hh"

#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal MagneticField *DetectorConstruction::fMagneticField = 0;
G4ThreadLocal G4FieldManager *DetectorConstruction::fFieldManager = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
    : G4VUserDetectorConstruction(),
    fMessenger(nullptr), fUserLimits(nullptr),
    fCheckOverlaps(true),
    fLogicWorld(nullptr), fLogicGas(nullptr), fLogicChamber(nullptr),
    fVisAttributes(),
    fGasMat(nullptr), fGasName1("He"), fGasName2("iC4H10"), fFrac1(90.), fFrac2(10.), fPressure(0.1*atmosphere),
    fNbOfGasMat(0),
    fGasMatMap(nullptr)
{
    fGasMatMap = new std::unordered_map<std::string, std::string>;

    fUserLimits = new G4UserLimits(1*mm);

    fMessenger = new DetectorConstructionMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
    delete fMessenger;
    delete fUserLimits;
    delete fGeoRotation;
    // delete fLogicChamber;
    for(auto visAttributes : fVisAttributes)
        delete visAttributes;
    // pointers to G4Material of the map are deleted at the end of the program.
    delete fGasMatMap;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume *DetectorConstruction::Construct()
{
    ConstructMaterials();
    SetGas(fGasName1, fFrac1, fGasName2, fFrac2, fPressure);
    ConstructGeometry();
    return fPhysWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructMaterials()
{
    auto nist = G4NistManager::Instance();

    nist->FindOrBuildMaterial("G4_AIR");

    // Vacuum "Hydrogen gas with very low density"
    G4double atomicNumber = 1.;
    G4double massOfMole = 1.008*g/mole;
    G4double density = 1.e-25*g/cm3;
    G4double temperature = 2.73*kelvin;
    G4double pressure = 3.e-18*pascal;
    new G4Material(
        "Vacuum", atomicNumber, massOfMole,
        density, kStateGas, temperature, pressure);

    // ref [1] : Coupled cluster calculations of mean excitation energies of the noble gas atoms He, Ne and Ar and of the H2 molecule
    // ref [2] : Techiniques for Nuclear and Particle Physics Experiments 
    // ref [3] : Mean excitation energies and energy deposition characteristics of bio-organic molecules.
    // ref [4] : Geant4 simulation of DRAGON's hybrid detector

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

void DetectorConstruction::ConstructGeometry()
{
    auto vacuum = G4Material::GetMaterial("Vacuum");

    // geometries --------------------------------------------------------------
    auto solidWorld = new G4Box("SolidWorld", 1000 * mm, 1000 * mm, 1000 * mm);
    fLogicWorld = new G4LogicalVolume(solidWorld, vacuum, "LogicWorld");
    fPhysWorld = new G4PVPlacement(
        0, G4ThreeVector(), fLogicWorld, "PhysWorld", 0,
        false, 0, fCheckOverlaps);

    fGeoRotation = new G4RotationMatrix();
    fGeoRotation->rotateY(90. * deg);
    BuildMagnet();
    BuildMagField();
    BuildGas();
    BuildChamber();
    BuildBeamPipe();

    SetVisAttributes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::BuildMagnet()
{
    const G4double dRim = 45*mm;
    const G4double dRimChamfer = 100*mm, dBodyChamfer = 80*mm, dMidChamferPosY = dBodyChamfer, dMidChamferNegY = 200*mm;
    
    const G4double xBody = 750*mm/2, yPosBody = 625*mm, yNegBody = -370*mm;
    const G4double xMid = xBody, yPosMid = yPosBody, yNegMid = 160*mm;
    
    const G4double zRim = 35*mm/2, zMid = 150*mm/2, zBody = (560*mm - 4*zMid - 4*zRim)/2;
    
    const G4double rVoidTube = 150*mm;

    // defining vertice on the top surface of volume in clockwise.
    std::vector<G4TwoVector> verticeMid;
    verticeMid.emplace_back(xMid - dMidChamferPosY, yPosMid);
    verticeMid.emplace_back(xMid, yPosMid - dMidChamferPosY);
    verticeMid.emplace_back(xMid, yNegMid + dMidChamferNegY);
    verticeMid.emplace_back(xMid - dMidChamferNegY, yNegMid);
    verticeMid.emplace_back(-xMid + dMidChamferNegY, yNegMid);
    verticeMid.emplace_back(-xMid, yNegMid + dMidChamferNegY);
    verticeMid.emplace_back(-xMid, yPosMid - dMidChamferPosY);
    verticeMid.emplace_back(-xMid + dMidChamferPosY, yPosMid);
    auto solidMid = new G4ExtrudedSolid("SolidMagnetMid", verticeMid, zMid + 2*zBody + 2*zRim, G4TwoVector(), 1., G4TwoVector(), 1.);

    std::vector<G4TwoVector> verticeBody;
    verticeBody.emplace_back(xBody - dBodyChamfer, yPosBody);
    verticeBody.emplace_back(xBody, yPosBody - dBodyChamfer);
    verticeBody.emplace_back(xBody, yNegBody + dBodyChamfer);
    verticeBody.emplace_back(xBody - dBodyChamfer, yNegBody);
    verticeBody.emplace_back(-xBody + dBodyChamfer, yNegBody);
    verticeBody.emplace_back(-xBody, yNegBody + dBodyChamfer);
    verticeBody.emplace_back(-xBody, yPosBody - dBodyChamfer);
    verticeBody.emplace_back(-xBody + dBodyChamfer, yPosBody);
    auto solidBody = new G4ExtrudedSolid("SolidMagnetBody", verticeBody, zBody, G4TwoVector(), 1., G4TwoVector(), 1.);

    std::vector<G4TwoVector> verticeRim;
    verticeRim.emplace_back(xBody + dRim - dRimChamfer, yPosBody+ dRim);
    verticeRim.emplace_back(xBody + dRim, yPosBody + dRim - dRimChamfer);
    verticeRim.emplace_back(xBody + dRim, yNegBody - dRim + dRimChamfer);
    verticeRim.emplace_back(xBody + dRim - dRimChamfer, yNegBody - dRim);
    verticeRim.emplace_back(-xBody - dRim + dRimChamfer, yNegBody - dRim);
    verticeRim.emplace_back(-xBody - dRim, yNegBody - dRim + dRimChamfer);
    verticeRim.emplace_back(-xBody - dRim, yPosBody + dRim - dRimChamfer);
    verticeRim.emplace_back(-xBody - dRim + dRimChamfer, yPosBody + dRim);
    auto solidRim = new G4ExtrudedSolid("SolidMagnetRim", verticeRim, zRim, G4TwoVector(), 1., G4TwoVector(), 1.);

    G4Transform3D transMid;
    G4MultiUnion *solidMagnetWithOutHole = new G4MultiUnion("SolidMagnetWithOutHole");
    solidMagnetWithOutHole->AddNode(*solidMid, transMid);

    G4RotationMatrix emptyRotation;
    G4Transform3D transBodyUp(emptyRotation, G4ThreeVector(0., 0., zMid + zBody));
    G4Transform3D transBodyDown(emptyRotation, G4ThreeVector(0., 0., -zMid - zBody));
    solidMagnetWithOutHole->AddNode(*solidBody, transBodyUp);
    solidMagnetWithOutHole->AddNode(*solidBody, transBodyDown);
    
    G4Transform3D transRimUp(emptyRotation, G4ThreeVector(0., 0., zMid + 2*zBody + zRim));
    G4Transform3D transRimDown(emptyRotation, G4ThreeVector(0., 0., -zMid - 2*zBody - zRim));
    solidMagnetWithOutHole->AddNode(*solidRim, transRimUp);
    solidMagnetWithOutHole->AddNode(*solidRim, transRimDown);
    
    solidMagnetWithOutHole->Voxelize();

    auto solidVolidTube = new G4Tubs("SolidMagnetVolidTube", 0., rVoidTube, 1.1*(zMid + 2*zBody + 2*zRim), 0., twopi);    
    auto solidMagnet = new G4SubtractionSolid("solidMagnet", solidMagnetWithOutHole, solidVolidTube);

    fLogicMagnet = new G4LogicalVolume(solidMagnet, fGasMat, "LogicMagnet");
    fPhysMagnet = new G4PVPlacement(fGeoRotation, G4ThreeVector(), fLogicMagnet, "PhysMagnet", fLogicWorld, false, 0, fCheckOverlaps);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::BuildMagField()
{
    const G4double rMagField = 150*mm, pDzMagField = 200*mm/2;
    auto solidMagField = new G4Tubs("solidMagField", 0., rMagField, pDzMagField, 0., 360. * deg);
    fLogicMagField = new G4LogicalVolume(solidMagField, fGasMat, "LogicMagField");
    fLogicMagField->SetUserLimits(fUserLimits);
    fPhysMagField = new G4PVPlacement(
        fGeoRotation, G4ThreeVector(), fLogicMagField, "PhysMagField", fLogicWorld,
        false, 0, fCheckOverlaps);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::BuildGas()
{
    // For now, magnetic field volume and gas volume completly coincide.
    fLogicGas = fLogicMagField;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::BuildChamber()
{
    const auto params = ParamContainerTable::Instance()->GetContainer("gas_chamber");
    const G4double
        xChamber = params->GetParamD("chamberX")/2,
        yChamber = params->GetParamD("chamberY")/2,
        zChamber = params->GetParamD("chamberZ")/2;
    
    G4ThreeVector position(
        params->GetParamD("posX"),
        params->GetParamD("posY"),
        params->GetParamD("posZ"));

    auto solidChamber = new G4Box("SolidChamber", zChamber, yChamber, xChamber);
    
    fLogicChamber = new G4LogicalVolume(solidChamber, fGasMat, "LogicChamber");
    fLogicChamber->SetUserLimits(fUserLimits);
    
    fPhysChamber = new G4PVPlacement(
        fGeoRotation, G4ThreeVector(), fLogicChamber, "PhysMagField", fLogicMagField,
        false, 0, fCheckOverlaps);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::BuildBeamPipe()
{
    const G4double rPipe = 25*mm;
    const G4double zPipe = 150*mm/2;
    const G4double xPos = 0.*mm, yPos = -70*mm, zPos = -150*mm - zPipe;

    auto Vacuum = G4NistManager::Instance()->FindOrBuildMaterial("Vacuum");

    auto solidPipe = new G4Tubs("SolidPipe", 0.*mm, rPipe, zPipe, 0, twopi);
    fLogicPipe = new G4LogicalVolume(solidPipe, Vacuum, "LogicPipe");
    fPhysPipe = new G4PVPlacement(
        nullptr, G4ThreeVector(xPos, yPos, zPos), fLogicPipe, "PhysPipe",
        fLogicWorld, false, 0, fCheckOverlaps
    );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetVisAttributes()
{
    auto visAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
    visAttributes->SetVisibility(false);
    fLogicWorld->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);

    visAttributes = new G4VisAttributes(G4Colour(0.9, 0.9, 0.9, 0.5)); // light grey
    fLogicMagnet->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);

    visAttributes = new G4VisAttributes(G4Colour(1, 0.5, 1, 0.2));
    fLogicMagField->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);

    visAttributes = new G4VisAttributes(G4Colour(0.9, 0.5, 0.3, 0.2));
    fLogicChamber->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);

    visAttributes = new G4VisAttributes(G4Colour(0.9, 0.1, 0.1, 0.2));
    fLogicPipe->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
    auto sdManager = G4SDManager::GetSDMpointer();
    G4String SDname;
    auto gasChamberSD = new GasChamberSD(SDname = "/gasChamber");
    sdManager->AddNewDetector(gasChamberSD);
    fLogicChamber->SetSensitiveDetector(gasChamberSD);

    fMagneticField = new MagneticField();
    fFieldManager = new G4FieldManager();
    fFieldManager->SetDetectorField(fMagneticField);
    fFieldManager->CreateChordFinder(fMagneticField);
    G4bool forceToAllDaughters = true;
    fLogicMagField->SetFieldManager(fFieldManager, forceToAllDaughters);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetGas(const G4String &gas1, G4double frac1, const G4String &gas2, G4double frac2, G4double pressure)
{
    G4Material *gasMat1 = FindGasMat(gas1);
    G4Material *gasMat2 = FindGasMat(gas2);
    if(gasMat1 == nullptr || gasMat2 == nullptr)
        return;
    else if((frac1 + frac2 - 100)*(frac1 + frac2 - 100)/10000 > 1e-6)
    {
        std::ostringstream message;
        message << "Sum of frac1 and frac2 is not close enough to 100%%.";
        G4Exception("DetectorConstruction::SetGas(const G4String &gas1, G4double frac1, const G4String &gas2, G4double frac2, G4double pressure)",
            "GeomGasMat0000", JustWarning, message);
        return;
    }
    else
    {
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
    // if SetGas is called after Construct (by UI command)
    if(fLogicGas)
    {
        fLogicGas->SetMaterial(fGasMat);
        G4RunManager::GetRunManager()->PhysicsHasBeenModified();
    }
    G4cout << "Gas mixture : " << GetGasMixtureStat() << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetLimitStep(G4double ustepMax)
{
    fUserLimits->SetMaxAllowedStep(ustepMax);
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetLimitTrack(G4double utrakMax)
{
    fUserLimits->SetUserMaxTrackLength(utrakMax);
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetLimitTime(G4double utimeMax)
{
    fUserLimits->SetUserMaxTime(utimeMax);
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetMinKinE(G4double uekinMax)
{
    fUserLimits->SetUserMinEkine(uekinMax);
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
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