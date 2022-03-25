#include "gas/GasMixtureProperties.hh"
#include "gas/GasMixturePropertiesException.hh"


#include <stdexcept>

using namespace GasMixturePropertiesErrorNum;
using namespace Garfield;
using namespace std;

GasMixtureProperties::GasMixtureProperties()
    : GasMixtureProperties({}, {})
{
}

GasMixtureProperties::GasMixtureProperties(
    const std::vector<string> &comps, const std::vector<G4double> &frac,
    const G4double _temperature, const G4double _pressure)
    : components(comps), fractions(frac),
    nComponents(comps.size()),
    temperature(_temperature), pressure(_pressure),
    eField(0.), bField(0.),
    gasMixtureName(""),
    gasMixture(make_unique<MediumMagboltz>())
{
    if(nComponents != fractions.size())
        throw invalid_argument("The sizes of mixture components and fractions are not same.");
    else if(nComponents > 0)
        InitMagboltz();
}

const string GasMixtureProperties::MakeGasMixtureName() const
{
    try {
        string gasStr = components.at(0);
        string fracStr = std::to_string(int(100*fractions.at(0)));
        for(size_t i = 1;i < nComponents;++i)
        {
            gasStr += "/" +components.at(i);
            fracStr += "/" + std::to_string(int(100*fractions.at(i)));
        }
        char pressureStr[256], temperatureStr[256];
        snprintf(pressureStr, 256, "_%04dTorr", (int)gasMixture->GetPressure());
        snprintf(temperatureStr, 256, "_%03.2fK", gasMixture->GetTemperature());
        return gasStr + "(" + fracStr + ")" + pressureStr + temperatureStr;
    }
    catch(out_of_range const &e)
    {
        throw GasMixturePropertiesException("MakeGasMixtureName()", NOT_INITIALIZED);
    }
    return "";
}

void GasMixtureProperties::InitMagboltz()
{
    vector<string> gas = components;
    vector<G4double> f = fractions;
    gas.resize(6, "");
    f.resize(6, 0.);
    bool validity =  gasMixture->SetComposition(
        gas[0], f[0], gas[1], f[1], gas[2], f[2],
        gas[3], f[3], gas[4], f[4], gas[5], f[5]);
    if(!validity)
        throw GasMixturePropertiesException("MakeGasMixtureName()", INVALID_GAS_COMPONENT_NAME, gasMixtureName);
    gasMixture->ResetTables();
}

void GasMixtureProperties::SetGasMixture(const std::vector<std::string> &comps, const std::vector<G4double> &fracs)
{
    if(comps.size() != fracs.size())
        throw invalid_argument("The sizes of mixture components and fractions are not same.");
    nComponents = comps.size();
    components = comps;
    fractions = fracs;
    gasMixtureName = MakeGasMixtureName();
    InitMagboltz();
}

void GasMixtureProperties::SetTemperature(G4double t)
{
    temperature = t;
    gasMixture->SetTemperature(t/kelvin);
    gasMixture->ResetTables();
    gasMixtureName = MakeGasMixtureName();
}

void GasMixtureProperties::SetPressure(G4double p)
{
    pressure = p;
    gasMixture->SetPressure(760.*p/atmosphere);
    gasMixture->ResetTables();
    gasMixtureName = MakeGasMixtureName();
}

void GasMixtureProperties::SetElectricField(const G4ThreeVector &_eField)
{
    eField = _eField;
}

void GasMixtureProperties::SetElectricField(const G4double Ex, const G4double Ey, const G4double Ez)
{
    eField = G4ThreeVector(Ex, Ey, Ez);
}

void GasMixtureProperties::SetMagneticField(const G4ThreeVector &_bField)
{
    bField = _bField;
}

void GasMixtureProperties::SetMagneticField(const G4double Bx, const G4double By, const G4double Bz)
{
    bField = G4ThreeVector(Bx, By, Bz);
}

G4double GasMixtureProperties::GetTemprature() const
{
    return gasMixture->GetTemperature()*kelvin;
}

G4double GasMixtureProperties::GetPressure() const
{
    return atmosphere*gasMixture->GetPressure()/760.;
    gasMixture->ResetTables();
}

G4double GasMixtureProperties::GetDensity() const
{
    if(!CheckMagboltzInitialized())
        throw GasMixturePropertiesException("GetDriftVelocity()", NOT_INITIALIZED);
    else
        return gasMixture->GetMassDensity()*gram/cm3;
}

G4double GasMixtureProperties::GetMeanAtomicWeight() const
{
    if(!CheckMagboltzInitialized())
        throw GasMixturePropertiesException("GetMeanAtomicWeight()", NOT_INITIALIZED);
    else
        return gasMixture->GetAtomicWeight();
}

G4ThreeVector GasMixtureProperties::GetDriftVelocity() const
{
    G4ThreeVector vd;
    if(!CheckMagboltzInitialized())
        throw GasMixturePropertiesException("GetDriftVelocity()", NOT_INITIALIZED);
    else if(!gasMixture->ElectronVelocity(
            eField[0], eField[1], eField[2],
            bField[0]/tesla, bField[1]/tesla, bField[2]/tesla,
            vd[0], vd[1], vd[2]))
        throw GasMixturePropertiesException("GetDriftVelocity()", MAGBOLTZ_CALCULATION_FAILURE, "drift velocity");
    return 1e3*vd;
}

G4double GasMixtureProperties::GetTransverseDiffusion() const
{
    G4double dl, dt;
    if(!CheckMagboltzInitialized())
        throw GasMixturePropertiesException("GetTransverseDiffusion()", NOT_INITIALIZED);
    else if(!gasMixture->ElectronDiffusion(
            eField[0], eField[1], eField[2],
            bField[0]/tesla, bField[1]/tesla, bField[2]/tesla,
            dl, dt))
        throw GasMixturePropertiesException("GetTransverseDiffusion()", MAGBOLTZ_CALCULATION_FAILURE, "diffusion");
    return dt;
}

G4double GasMixtureProperties::GetLongitudinalDiffusion() const
{
    G4double dl, dt;
    if(!CheckMagboltzInitialized())
        throw GasMixturePropertiesException("GetTransverseDiffusion()", NOT_INITIALIZED);
    else if(!gasMixture->ElectronDiffusion(
            eField[0], eField[1], eField[2],
            bField[0]/tesla, bField[1]/tesla, bField[2]/tesla,
            dl, dt))
        throw GasMixturePropertiesException("GetTransverseDiffusion()", MAGBOLTZ_CALCULATION_FAILURE, "diffusion");
    return dl;
}

G4double GasMixtureProperties::GetFanoFactor() const
{
    if(CheckMagboltzInitialized())
        return gasMixture->GetFanoFactor();
    else
        throw GasMixturePropertiesException("GetFanoFactor()", NOT_INITIALIZED);
}

G4double GasMixtureProperties::GetMeanExcitationEnergy() const
{
    if(CheckMagboltzInitialized())
        return 0.188*eV*gasMixture->GetW()/(gasMixture->GetFanoFactor() + 0.15);
    else
        throw GasMixturePropertiesException("GetMeanExcitationEnergy()", NOT_INITIALIZED);
}

G4double GasMixtureProperties::GetMeanEnergyPerIonPair() const
{
    if(CheckMagboltzInitialized())
        return gasMixture->GetW()*eV;
    else
        throw GasMixturePropertiesException("GetMeanEnergyPerIonPair()", NOT_INITIALIZED);
}

const string GasMixtureProperties::GetGasMixtureName() const
{
    return gasMixtureName;
}

const vector<string> GasMixtureProperties::GetComponents() const
{
    return components;
}

const vector<G4double> GasMixtureProperties::GetFractions() const
{
    return fractions;
}

G4bool GasMixtureProperties::CheckMagboltzInitialized() const
{
    return gasMixture->IsGas() && gasMixture->IsIonisable() && gasMixture->IsDriftable();
}