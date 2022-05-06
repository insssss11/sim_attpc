/// \file GasChamberDigitizer.cc
/// \brief Implementation of the GasChamberDigitizer class

#include "gas_chamber/GasChamberDigitizer.hh"
#include "gas_chamber/GasChamberHit.hh"
#include "tuple/TupleVectorManager.hh"

#include "G4SDManager.hh"
#include "G4RunManager.hh"

#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"

#include <stdexcept>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

using namespace std;
using namespace CLHEP;

GasChamberDigitizer::GasChamberDigitizer(const G4String &name)
    : G4VDigitizerModule(name),
    totalChargeIntegrator(new ROOT::Math::IntegratorMultiDim(ROOT::Math::IntegrationMultiDim::kADAPTIVE, 1e-3)),
    diffusion(new DiffusionGaussian),
    gasMixtureProperties(nullptr), readoutPads(nullptr)
{
    Init();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


GasChamberDigitizer::~GasChamberDigitizer()
{
    delete readoutPads;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigitizer::Init()
{
    auto container = ParamContainerTable::Instance()->GetContainer("gas_chamber");
    InitPads(container);
    InitMulplier(container);
}

void GasChamberDigitizer::InitPads(const ParamContainer *container)
{
    padPlaneX = container->GetParamD("chamberX");
    padPlaneY = container->GetParamD("chamberY");
    chamberH = container->GetParamD("chamberZ");

    nPadX = container->GetParamI("nPadX");
    nPadY = container->GetParamI("nPadY");

    centerPos = G4ThreeVector(
        container->GetParamD("chamberPosX"),
        container->GetParamD("chamberPosY"),
        container->GetParamD("chamberPosZ"));

    padMargin = container->GetParamD("margin");

    const G4double xPad = padPlaneX/nPadX, yPad = padPlaneY/nPadY;
    const G4double xLowerLeft = centerPos[0] - (padPlaneX - xPad)/2;
    const G4double yLowerLeft = centerPos[1] - (padPlaneY - yPad)/2;
    if(readoutPads != nullptr)
        delete readoutPads;
    readoutPads = new std::vector<std::vector<GasChamberDigi>>;
    readoutPads->reserve(nPadY);

    // Fill out readoutPads
    for(int y = 0;y < nPadY;++y)
    {
        readoutPads->push_back(std::vector<GasChamberDigi>{});
        for(int x = 0;x < nPadX;++x)
        {
            readoutPads->at(y).emplace_back(
                G4TwoVector(xLowerLeft + xPad*x, yLowerLeft + yPad*y),
                xPad/2, yPad/2, padMargin);
            readoutPads->at(y).back().SetPadNum(y*nPadX + x);
        }
    }
}

void GasChamberDigitizer::InitMulplier(const ParamContainer *container)
{
    collEff = container->GetParamD("collEff");
    gainMean = container->GetParamD("gainMean");
    gainTheta = container->GetParamD("gainTheta");
    gainStd = gainMean/sqrt(gainTheta + 1);
    gainMean *= collEff;
    gainStd *= collEff;
    G4cout << "-------------------------------------------------------------------------------" << G4endl;  
    G4cout << "Triple GEM Gain and std : " << gainMean << " " << gainStd << G4endl;  
    G4cout << "-------------------------------------------------------------------------------" << G4endl;  
    // Diffusion by triple GEM
    diffusion->SetIntrinsicDiff(container->GetParamD("gemDiffusionStd"));
}

void GasChamberDigitizer::FillChargeOnPads(vector<G4float> &vec) const
{
    // thread-local data for hit histogram
    G4ThreadLocalStatic vector<G4int> &hitHistData = TupleVectorManager::Instance()->GetTupleVectorContainer("tree_gc4")->GetVectorRefI("hits");
    if(hitHistData.empty())
        hitHistData.assign(nPadX*nPadY, 0);
    
    try {
        vec.resize(nPadX*nPadY);
        for(int y = 0;y < nPadY;++y)
            for(int x = 0;x < nPadX;++x)
            {
                vec.at(y*nPadX + x) = readoutPads->at(y).at(x).GetCharge();
                if(readoutPads->at(y).at(x).IsHit())
                    ++hitHistData.at(readoutPads->at(y).at(x).GetPadNum());
            }
    }
    catch(exception const &e)
    {
        cerr << string("In GasChamberDigitizer::FillChargeOnPads(vector<G4float> &), ") + e.what() << endl;
    }
}

void GasChamberDigitizer::FillTimeOnPads(std::vector<G4float> &vec) const
{
    try {
        vec.resize(nPadX*nPadY);
        for(int x = 0;x < nPadX;++x)
            for(int y = 0;y < nPadY;++y)
                vec.at(y*nPadX + x) = readoutPads->at(y).at(x).GetTime();
    }
    catch(exception const &e)
    {
        cerr << string("In GasChamberDigitizer::FillChargeOnPads(vector<G4float> &), ") + e.what() << endl;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigitizer::Digitize()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigitizer::ClearPads()
{
    for(auto &yPads : *readoutPads)
        for(auto &pad : yPads)
            pad.Clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigitizer::FillPadsInfo(const GasChamberHitsCollection* hitsCollection)
{
    for(size_t trk = 0;trk < hitsCollection->GetSize();++trk)
        FillPadsTrack(static_cast<const GasChamberHit*>(hitsCollection->GetHit(trk)));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigitizer::FillPadsTrack(const GasChamberHit *hit)
{
    const std::vector<G4float> eDeps = hit->GetEdep();
    const std::vector<G4float>
        xPos = hit->GetPosX(),
        yPos = hit->GetPosY(),
        zPos = hit->GetPosZ();
    for(int step = 0;step < hit->GetNbOfStepPoints();++step)
        FillPadsStep(
        G4ThreeVector(xPos.at(step), yPos.at(step), zPos.at(step)),
        eDeps.at(step), hit->GetParticleEnum());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigitizer::FillPadsStep(const G4ThreeVector &ePos, G4double eDep, TrainingDataTypes::EParticle parEnum)
{
    // coordinate in the frame of gas chamber
    const G4double ePosX = ePos.getZ(), ePosY = ePos.getY(), ePosZ = ePos.getX();
    const G4double driftLen = ePosZ - centerPos.getZ() + chamberH/2;

    // ready to integrate over each pad    
    G4double charge = MultiplicatedCharge(eDep/gasMixtureProperties->GetW());
    diffusion->SetCharge(charge);
    diffusion->SetPosition(ePosX, ePosY);
    diffusion->SetDriftLen(driftLen);

    G4TwoVector pos;
    G4double driftTime = DiffusedDriftLen(driftLen)/driftVel;
    G4double partialCharge = 0;
    G4double min[2], max[2];
    for(auto &padRows : *readoutPads)
        for(auto &pad : padRows)
        {
            if(CheckWorthIntegrate(&pad, diffusion->GetClusterStd(), ePosX, ePosY))
            {
                // gain
                pad.GetRange(min, max);
                partialCharge = totalChargeIntegrator->Integral(*diffusion, min, max);

                // update readout pad info
                pad.AddCharge(partialCharge);
                pad.AddWeightedTime(driftTime, partialCharge);
                
                if(CheckFillHist(ePos, eDep, parEnum))
                    pad.Hit();
            }
        }
}

G4bool GasChamberDigitizer::CheckFillHist(const G4ThreeVector &, G4double, TrainingDataTypes::EParticle parEnum) const
{
    // if hit on pad is by Oxygen track.
    return true;
}


G4bool GasChamberDigitizer::CheckWorthIntegrate(const GasChamberDigi* pad, Double_t cluseterStd, Double_t xe, Double_t ye)
{
    static constexpr G4double intRange = 10.0; // increase this parameter to perform integration over pads further from cluster position.
    G4double min[2], max[2];
    auto pos = pad->GetPosition();
    pad->GetRange(min, max);
    G4double distdistCorner, distdistCenter, xD, yD;
    if(pos[0] < xe)
        xD = max[0] - xe;
    else
        xD = min[0] - xe;
    if(pos[1] < ye)
        yD = max[1] - ye;
    else
        yD = min[1] - ye;
    distdistCorner = xD*xD + yD*yD;
    distdistCenter = (pos[0] - xe)*(pos[0] - xe) + (pos[1] - ye)*(pos[1] - ye);
    return distdistCorner < cluseterStd*cluseterStd*intRange*intRange ||
        distdistCenter < cluseterStd*cluseterStd*intRange*intRange;
}

G4double GasChamberDigitizer::DiffusedDriftLen(G4double driftLen) const
{
    return RandGauss::shoot(driftLen, diffusionL*sqrt(driftLen));
}

G4double GasChamberDigitizer::MultiplicatedCharge(G4double clusterSize) const
{
    G4double charge = e_SI*RandGauss::shoot(clusterSize*gainMean, sqrt(clusterSize)*gainStd);
    if(charge < 0)
        return 0.;
    else
        return charge;
}

void GasChamberDigitizer::SetGasMixtureProperties(GasMixtureProperties *_gasMixtureProperties)
{
    gasMixtureProperties = _gasMixtureProperties;
    UpdateGasProperties();
}

void GasChamberDigitizer::UpdateGasProperties()
{
    diffusion->SetDiffusionCoef(gasMixtureProperties->GetTransverseDiffusion()*sqrt(cm));
    diffusionL = gasMixtureProperties->GetLongitudinalDiffusion()*sqrt(cm);
    driftVel = gasMixtureProperties->GetDriftVelocity()*cm/us;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigitizer::SetChargeMultiplication(G4double _gainMean, G4double _gainStd)
{
    gainMean = _gainMean;
    gainStd = _gainStd;
}

void GasChamberDigitizer::SetFullScaleRange(G4double _fsr)
{
    chargeFSR = _fsr;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigitizer::SetThreshold(G4double threshold)
{
    adcThreshold = threshold;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigitizer::SetPadPlaneCenter(const G4ThreeVector &pos)
{
    centerPos = pos;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigitizer::SetPadMargin(G4double margin)
{
    padMargin = margin;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

