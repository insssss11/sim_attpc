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
#include "detector_construction/DetectorConstructionException.hh"
#include "detector_construction/GasMaterialTable.hh"
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

using namespace std;
using namespace DetectorConstructionErrorNum;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal G4UniformMagField *DetectorConstruction::fMagneticField = nullptr;
G4ThreadLocal G4FieldManager *DetectorConstruction::fFieldManager = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
    : G4VUserDetectorConstruction(),
    fMessenger(nullptr), fUserLimits(nullptr),
    fCheckOverlaps(true),
    fLogicWorld(nullptr), fLogicGas(nullptr), fLogicChamber(nullptr),
    fVisAttributes(),
    fGasMat(nullptr), gasMixture(std::make_unique<GasMixtureProperties>())
{
    Initialize();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::Initialize()
{
    GasMaterialTable::Initialize();
    InitializeGasMixture();

    paramContainer = ParamContainerTable::Instance()->GetContainer("gas_chamber");
    fUserLimits = new G4UserLimits(1*mm);
    fMessenger = new DetectorConstructionMessenger(this);

    fGeoRotation = new G4RotationMatrix();
    fGeoRotation->rotateY(90. * deg);
}

void DetectorConstruction::InitializeGasMixture()
{
    gasMixture->SetGasMixture({"He", "iC4H10"}, {0.9, 0.1});
    gasMixture->SetElectricField(150, 0., 0.);
    gasMixture->SetMagneticField(2.5*tesla, 0., 0.);
    gasMixture->SetPressure(0.1*atmosphere);
    gasMixture->SetTemperature(273.15*kelvin);
    UpdateGasMaterial();
}

DetectorConstruction::~DetectorConstruction()
{
    delete fMessenger;
    delete fUserLimits;
    delete fGeoRotation;
    delete fMagneticField;
    for(auto visAttributes : fVisAttributes)
        delete visAttributes;
}

G4VPhysicalVolume *DetectorConstruction::Construct()
{
    auto vacuum = FindGasMaterial("Vacuum");
    auto solidWorld = new G4Box("SolidWorld", 1000 * mm, 1000 * mm, 1000 * mm);
    fLogicWorld = new G4LogicalVolume(solidWorld, vacuum, "LogicWorld");
    fPhysWorld = new G4PVPlacement(
        0, G4ThreeVector(), fLogicWorld, "PhysWorld", 0,
        false, 0, fCheckOverlaps);
    ConstructGeometry();
    return fPhysWorld;
}

void DetectorConstruction::ConstructGeometry()
{
    BuildMagnet();
    BuildMagField();
    BuildGas();
    BuildChamber();
    BuildBeamPipe();
    SetVisAttributes();
}

void DetectorConstruction::BuildMagnet()
{
    const G4double dRim = 45*mm;
    const G4double dRimChamfer = 100*mm, dBodyChamfer = 80*mm, dMidChamferPosY = dBodyChamfer, dMidChamferNegY = 200*mm;

    const G4double xBody = 750*mm/2, yPosBody = 625*mm, yNegBody = -370*mm;
    const G4double xMid = xBody, yPosMid = yPosBody, yNegMid = 160*mm;

    const G4double zRim = 35*mm/2, zMid = 150*mm/2, zBody = (560*mm - 4*zMid - 4*zRim)/2;

    const G4double rVoidTube = 150*mm;

    // defining vertice on the top surface of volume in clockwise.
    vector<G4TwoVector> verticeMid;
    verticeMid.emplace_back(xMid - dMidChamferPosY, yPosMid);
    verticeMid.emplace_back(xMid, yPosMid - dMidChamferPosY);
    verticeMid.emplace_back(xMid, yNegMid + dMidChamferNegY);
    verticeMid.emplace_back(xMid - dMidChamferNegY, yNegMid);
    verticeMid.emplace_back(-xMid + dMidChamferNegY, yNegMid);
    verticeMid.emplace_back(-xMid, yNegMid + dMidChamferNegY);
    verticeMid.emplace_back(-xMid, yPosMid - dMidChamferPosY);
    verticeMid.emplace_back(-xMid + dMidChamferPosY, yPosMid);
    auto solidMid = new G4ExtrudedSolid("SolidMagnetMid", verticeMid, zMid + 2*zBody + 2*zRim, G4TwoVector(), 1., G4TwoVector(), 1.);

    vector<G4TwoVector> verticeBody;
    verticeBody.emplace_back(xBody - dBodyChamfer, yPosBody);
    verticeBody.emplace_back(xBody, yPosBody - dBodyChamfer);
    verticeBody.emplace_back(xBody, yNegBody + dBodyChamfer);
    verticeBody.emplace_back(xBody - dBodyChamfer, yNegBody);
    verticeBody.emplace_back(-xBody + dBodyChamfer, yNegBody);
    verticeBody.emplace_back(-xBody, yNegBody + dBodyChamfer);
    verticeBody.emplace_back(-xBody, yPosBody - dBodyChamfer);
    verticeBody.emplace_back(-xBody + dBodyChamfer, yPosBody);
    auto solidBody = new G4ExtrudedSolid("SolidMagnetBody", verticeBody, zBody, G4TwoVector(), 1., G4TwoVector(), 1.);

    vector<G4TwoVector> verticeRim;
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

    auto iron = G4NistManager::Instance()->FindOrBuildMaterial("G4_Fe");
    fLogicMagnet = new G4LogicalVolume(solidMagnet, iron, "LogicMagnet");
    fPhysMagnet = new G4PVPlacement(fGeoRotation, G4ThreeVector(), fLogicMagnet, "PhysMagnet", fLogicWorld, false, 0, fCheckOverlaps);
}

void DetectorConstruction::BuildMagField()
{
    auto vacuum = FindGasMaterial("Vacuum");
    const G4double rMagField = 150*mm, pDzMagField = 200*mm/2;
    auto solidMagField = new G4Tubs("solidMagField", 0., rMagField, pDzMagField, 0., 360. * deg);
    fLogicMagField = new G4LogicalVolume(solidMagField, vacuum, "LogicMagField");
    fLogicMagField->SetUserLimits(fUserLimits);
    fPhysMagField = new G4PVPlacement(
        fGeoRotation, G4ThreeVector(), fLogicMagField, "PhysMagField", fLogicWorld,
        false, 0, fCheckOverlaps);
}

void DetectorConstruction::BuildGas()
{
    const G4double
        xGas = paramContainer->GetParamD("gasX")/2,
        yGas = paramContainer->GetParamD("gasY")/2,
        zGas = paramContainer->GetParamD("gasZ")/2;

    G4ThreeVector position(
        paramContainer->GetParamD("gasPosX"),
        paramContainer->GetParamD("gasPosY"),
        paramContainer->GetParamD("gasPosZ"));

    auto solidGas = new G4Box("SolidGas", xGas, yGas, zGas);

    fLogicGas = new G4LogicalVolume(solidGas, fGasMat, "LogicGas");
    fLogicGas->SetUserLimits(fUserLimits);

    fPhysGas = new G4PVPlacement(
        fGeoRotation, position, fLogicGas, "PhysGas", fLogicMagField,
        false, 0, fCheckOverlaps);
}

void DetectorConstruction::BuildChamber()
{
    const G4double
        xChamber = paramContainer->GetParamD("chamberX")/2,
        yChamber = paramContainer->GetParamD("chamberY")/2,
        zChamber = paramContainer->GetParamD("chamberZ")/2;

    G4ThreeVector position(
        paramContainer->GetParamD("chamberPosX"),
        paramContainer->GetParamD("chamberPosY"),
        paramContainer->GetParamD("chamberPosZ"));

    auto solidChamber = new G4Box("SolidChamber", xChamber, yChamber, zChamber);

    fLogicChamber = new G4LogicalVolume(solidChamber, fGasMat, "LogicChamber");
    fLogicChamber->SetUserLimits(fUserLimits);
    G4Region *region = new G4Region("gas_chamber");
    fLogicChamber->SetRegion(region);
    region->AddRootLogicalVolume(fLogicChamber);

    fPhysChamber = new G4PVPlacement(
        fGeoRotation, G4ThreeVector(), fLogicChamber, "PhysChamber", fLogicGas,
        false, 0, fCheckOverlaps);
}

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

    visAttributes = new G4VisAttributes(G4Colour(1, 1, 1, 0.1));
    fLogicGas->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);

    visAttributes = new G4VisAttributes(G4Colour(0.9, 0.5, 0.3, 0.2));
    fLogicChamber->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);

    visAttributes = new G4VisAttributes(G4Colour(0.9, 0.1, 0.1, 0.2));
    fLogicPipe->SetVisAttributes(visAttributes);
    fVisAttributes.push_back(visAttributes);
}

void DetectorConstruction::ConstructSDandField()
{
    auto sdManager = G4SDManager::GetSDMpointer();
    auto gasChamberSD = new GasChamberSD("/gasChamber");
    sdManager->AddNewDetector(gasChamberSD);
    fLogicChamber->SetSensitiveDetector(gasChamberSD);

    fMagneticField = new G4UniformMagField(G4ThreeVector(2.5*tesla, 0., 0.));
    fFieldManager = new G4FieldManager();
    fFieldManager->SetDetectorField(fMagneticField);
    fFieldManager->CreateChordFinder(fMagneticField);
    G4bool forceToAllDaughters = true;
    fLogicMagField->SetFieldManager(fFieldManager, forceToAllDaughters);
}

G4UserLimits *DetectorConstruction::GetUserLimits() const
{
    return fUserLimits;
}


void DetectorConstruction::SetBeamPipePosY(G4double posY)
{
    G4ThreeVector pos = fPhysPipe->GetTranslation();
    pos.setY(posY);
    fPhysPipe->SetTranslation(pos);
}


void DetectorConstruction::SetMagneticField(const G4ThreeVector &bField)
{
    gasMixture->SetMagneticField(bField);
    fMagneticField->SetFieldValue(bField);
}


void DetectorConstruction::SetElectricField(const G4ThreeVector &eField)
{
    gasMixture->SetElectricField(eField);
}


void DetectorConstruction::SetGasMixture(const vector<string> &comps, const vector<double> &fracs)
{
    gasMixture->SetGasMixture(comps, fracs);
    UpdateGasMaterial();
}


void DetectorConstruction::SetPressure(const G4double pressure)
{
    gasMixture->SetPressure(pressure);
    UpdateGasMaterial();
}


void DetectorConstruction::UpdateGasMaterial()
{
    try {
        const vector<string> comps = gasMixture->GetComponents();
        const vector<double> fracs = gasMixture->GetFractions();
        const size_t nComps = comps.size();
        string mixtureName = gasMixture->GetGasMixtureName();
        // if wanted gas material was already build
        if(GasMaterialExists(mixtureName))
            fGasMat = FindGasMaterial(mixtureName);
        else
        {
            std::cout << mixtureName << std::endl;
            fGasMat = new G4Material(
                mixtureName, gasMixture->GetDensity(), nComps,
                kStateGas, gasMixture->GetTemprature(), gasMixture->GetPressure());
            G4double effMass = 0;
            for(size_t i = 0;i < nComps;++i)
            {
                G4Material *compMat = FindGasMaterial(comps.at(i));
                G4double moleMass = GasMaterialTable::MolecularWeight(comps.at(i));
                effMass += fracs.at(i)*moleMass;
                double massFrac = fracs.at(i)*moleMass/gasMixture->GetMeanAtomicWeight();
                fGasMat->AddMaterial(compMat, massFrac);
            }
            fGasMat->GetIonisation()->SetMeanEnergyPerIonPair(gasMixture->GetMeanEnergyPerIonPair());
            fGasMat->GetIonisation()->SetMeanExcitationEnergy(gasMixture->GetMeanExcitationEnergy());
        }
        if(fLogicGas != nullptr)
            UpdateLogicVolumes();
        PrintGasMaterialStats();
    }
    catch(Exception const &e)
    {
        e.WarnGeantKernel(JustWarning);
    }
}

void DetectorConstruction::UpdateLogicVolumes()
{
    fLogicGas->SetMaterial(fGasMat);
    fLogicChamber->SetMaterial(fGasMat);
    // fLogicMagnet->SetMaterial(fGasMat);
}


#include <iomanip>
void DetectorConstruction::PrintGasMaterialStats()
{
    auto prec = G4cout.precision(4);
    string mixtureName = gasMixture->GetGasMixtureName();
    G4cout << "-------------------------------------------------------------------------------------------------" << G4endl;
    G4cout << "Gas Mixutre                    : " + mixtureName.substr(0, mixtureName.find_first_of('_')) << G4endl;
    G4cout << "-------------------------------------------------------------------------------------------------" << G4endl;
    G4cout << "Pressure                       : " << setw(7) << gasMixture->GetPressure()*760/atmosphere << " Torr" << G4endl;
    G4cout << "Temperature                    : " << setw(7) << gasMixture->GetTemprature()/kelvin << " K" << G4endl;
    G4cout << "Mass Density                   : " << setw(7) << gasMixture->GetDensity()*cm3/gram << " g/cm3" << G4endl;
    G4cout << "Mean Excitation Energy(Weff)   : " << setw(7) << gasMixture->GetMeanExcitationEnergy()/eV << " eV" << G4endl;
    G4cout << "Mean Energy Per Ion Pair(Ieff) : " << setw(7) << gasMixture->GetMeanEnergyPerIonPair()/eV << " eV" << G4endl;
    G4cout << "Drift Velocity                 : " << setw(7) << gasMixture->GetDriftVelocity().x() << " cm/us" << G4endl;
    G4cout << "Transverse Diffusion Coeff     : " << setw(7) << gasMixture->GetTransverseDiffusion() << " cm^(1/2)" << G4endl;
    G4cout << "Longitudinal Diffusion Coeff   : " << setw(7) << gasMixture->GetLongitudinalDiffusion() << " cm^(1/2)" << G4endl;
    G4cout << "-------------------------------------------------------------------------------------------------" << G4endl;
    G4cout.precision(prec);
}

G4Material *DetectorConstruction::FindGasMaterial(const std::string &gasName) const
{
    if(!GasMaterialExists(gasName))
        throw DetectorConstructionException("FindGasMaterial(const std::string &)", INVALID_GAS_MATERIAL_NAME, gasName);
    return G4Material::GetMaterial(gasName, false);
}

G4bool DetectorConstruction::GasMaterialExists(const std::string &gasName) const
{
    return G4Material::GetMaterial(gasName, false) != nullptr;
}
