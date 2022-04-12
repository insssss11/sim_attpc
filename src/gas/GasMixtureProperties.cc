#include "gas/GasMixtureProperties.hh"
#include "gas/GasMixturePropertiesException.hh"
#include "gas/GasMaterialTable.hh"

#include <stdexcept>
#include <filesystem>
#include <unistd.h>
namespace fs = std::filesystem;
using namespace GasMixturePropertiesErrorNum;
using namespace Garfield;
using namespace std;

string GasMixtureProperties::gasFileDir;
unordered_map<string, G4int> GasMixtureProperties::gasFileList;
unordered_map<string, G4int> GasMixtureProperties::magboltzGasList = {
    {"CF4", 0}, {"Ar", 0}, {"He", 0}, {"He-3", 0}, {"Ne", 0},
    {"Kr", 0}, {"Xe", 0}, {"CH4", 0}, {"C2H6", 0}, {"C3H8", 0},
    {"iC4H10", 0}, {"CO2", 0}, {"neoC5H12", 0}, {"H2O", 0}, {"O2", 0},
    {"N2", 0}, {"NO", 0}, {"N2O", 0}, {"C2H4", 0}, {"C2H2", 0}, {"H2", 0},
    {"D2", 0}, {"CO", 0}, {"Methylal", 0}, {"DME", 0}, {"Reid-Step", 0},
    {"Maxwell-Model", 0}, {"Reid-Ramp", 0}, {"C2F6", 0}, {"SF6", 0}, {"NH3", 0},
    {"C3H6", 0}, {"cC3H6", 0}, {"CH3OH", 0}, {"C2H5OH", 0}, {"C3H7OH", 0},
    {"Cs", 0}, {"F2", 0}, {"CS2", 0}, {"COS", 0}, {"CD4", 0},
    {"BF3", 0}, {"C2H2F4", 0}, {"TMA", 0}, {"paraH2", 0}, {"nC3H7OH", 0},
    {"orthoD2", 0}, {"Xe", 0}, {"CHF3", 0}, {"CF3Br", 0}, {"C3F8", 0},
    {"O3", 0}, {"Hg", 0}, {"H2S", 0}, {"nC4H10", 0}, {"nC5H12", 0},
    {"N2", 0}, {"GeH4", 0}, {"SiH4", 0}, {"CCl4", 0}
};


GasMixtureProperties::GasMixtureProperties()
    : GasMixtureProperties({}, {})
{
}

GasMixtureProperties::GasMixtureProperties(
    const vector<string> &comps, const vector<G4int> &fracs,
    const G4int _pressure, const G4double _temperature)
    : components(comps), fractions(fracs),
    nComponents(comps.size()),
    eField(0.), bField(0.),
    pressure(_pressure), temperature(_temperature),
    gasMagboltz(make_unique<MediumMagboltz>())
{
    try {
        if(gasFileList.empty())
            InitGasFileList();
        if(nComponents > 0)
            SetGasMixture(comps, fracs, pressure, temperature);
    }
    catch(Exception const &e)
    {
        e.WarnGeantKernel(FatalException);
    }
}

void GasMixtureProperties::InitGasFileList()
{
    char buffer[1024];
    ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer)/sizeof(char));
    buffer[length] = '\0';
    std::string path = buffer;
    gasFileDir = path.substr(0, path.find_last_of('/') + 1) + "resources/gas_file/";
    try {
        for(const auto & entry : fs::directory_iterator(gasFileDir))
            gasFileList[entry.path()];
    }
    catch(exception const &e)
    {
        throw GasMixturePropertiesException("InitGasFileList()", GASFILE_DIR_NOT_FOUND, path);
    }
}

void GasMixtureProperties::PrintGasFileList()
{
    G4cout << "----------------------------------------------" << G4endl;
    G4cout << "List of gas files available" << G4endl;
    G4cout << "[gas1_frac1]..[gas2_frac2]_[pressure in torr].gas" << G4endl;
    for(const auto &item : gasFileList)
        G4cout << item.first << G4endl;
    G4cout << "----------------------------------------------" << G4endl;
}

void GasMixtureProperties::SetGasMixture(
    const vector<string> &comps, const vector<G4int> &fracs,
    const G4int _pressure, const G4double _temperature)
{
    try {
        CheckArguments(comps, fracs, pressure, temperature);
        nComponents = comps.size();
        components = comps;
        fractions = fracs;
        pressure = _pressure;
        temperature = _temperature;
        gasMixtureName = MakeGasMixtureName(comps, fracs, pressure, temperature);
        gasFileName = MakeGasFileName(comps, fracs, pressure);
        Update();
    }
    catch(Exception const &e)
    {
        e.WarnGeantKernel(JustWarning);
    }
    catch(exception const &e)
    {
        G4cerr << "Error in GasMixtureProperties::SetGasMixture :" << e.what() << G4endl;
    }
}

const string GasMixtureProperties::MakeGasMixtureName(
    const vector<string> &comps, const vector<G4int> &fracs,
    const G4int pressure, const G4double temperature) const
{
    string compStr = comps.front();
    string fracStr = to_string(fracs.front());
    for(size_t i = 1;i < comps.size();i++)
    {
        compStr += "/" + comps.at(i);
        fracStr += "/" + to_string(fracs.at(i));
    }
    char buffer1[256], buffer2[256];
    sprintf(buffer1, "%04dtorr", pressure);
    sprintf(buffer2, "%4.2fK", temperature);
    return compStr + "(" + fracStr + ")_" + buffer1 + "_" + buffer2;
}

const string GasMixtureProperties::MakeGasFileName(
    const vector<string> &comps, const vector<G4int> &fracs,
    const G4int pressure) const
{
    string name = "";
    char buffer1[256], buffer2[256];
    for(size_t i = 0;i < comps.size();i++)
    {
        sprintf(buffer1, "%02d", fracs.at(i));
        name += comps.at(i) + "_" + buffer1 +"_";
    }
    sprintf(buffer2, "%04d", pressure);
    return gasFileDir + name + buffer2 + ".gas";
}

void GasMixtureProperties::Update()
{
    try {
        UpdateMagboltz();
        UpdateMaterial();
        UpdateParameters();
    }
    catch(Exception const &e) {
        G4cerr << "Unexpected error ocurred..." << G4endl;
        e.WarnGeantKernel(FatalException);
    }
}

void GasMixtureProperties::UpdateMagboltz()
{
    vector<string> gas = components;
    vector<G4int> f = fractions;
    gas.resize(6, "");
    f.resize(6, 0);
    gasMagboltz->SetComposition(
        gas[0], (G4double)f[0], gas[1], (G4double)f[1], gas[2], (G4double)f[2],
        gas[3], (G4double)f[3], gas[4], (G4double)f[4], gas[5], (G4double)f[5]);
    gasMagboltz->ResetTables();
    if(!gasMagboltz->LoadGasFile(gasFileName))
        throw GasMixturePropertiesException(
        "SetGasMixture(const vector<string> &comps, const vector<G4int> &, const G4int, const G4doublee)",
        GASFILE_LOAD_FAILURE, gasFileName);
    gasMagboltz->SetTemperature(temperature/kelvin);
}

void GasMixtureProperties::UpdateMaterial()
{
    auto gasTable = GasMaterialTable::Instance();
    // ref : Garfield++ User Guid by H. Schindler
    gasPhysicalProperties.density = gasMagboltz->GetMassDensity()*g/cm3;    
    gasPhysicalProperties.fanoFactor = gasMagboltz->GetFanoFactor();
    gasPhysicalProperties.meanEnergyPerIonPair = gasMagboltz->GetW()*eV;
    gasPhysicalProperties.meanExcitationEnergy = 0.188*gasMagboltz->GetW()*eV/(gasMagboltz->GetFanoFactor() + 0.15);
    // If a G4Material instance already built, reuse it.
    if(gasTable->GasMaterialExists(gasMixtureName))
        gasMaterial = gasTable->FindGasMaterial(gasMixtureName);
    else
    {
        gasMaterial = new G4Material(
            gasMixtureName, gasPhysicalProperties.density, nComponents,
            kStateGas, temperature, (G4double)pressure*atmosphere/760.);
        G4double effMass = 0.;
        for(size_t i = 0;i < nComponents;++i)
        {
            G4Material *compMat = gasTable->FindGasMaterial(components.at(i));
            G4double moleMass = gasTable->MolecularWeight(components.at(i));
            effMass += fractions.at(i)*moleMass/100;
            G4double massFrac = fractions.at(i)*moleMass/gasMagboltz->GetAtomicWeight();
            gasMaterial->AddMaterial(compMat, massFrac/100.);
        }
    }
    gasMaterial->GetIonisation()->SetMeanEnergyPerIonPair(gasPhysicalProperties.meanEnergyPerIonPair);
    gasMaterial->GetIonisation()->SetMeanExcitationEnergy(gasPhysicalProperties.meanExcitationEnergy);
}

void GasMixtureProperties::UpdateParameters()
{
    G4double dummy1, dummy2;
    gasPhysicalProperties.fanoFactor = gasMagboltz->GetFanoFactor();
    gasPhysicalProperties.meanExcitationEnergy
        = 0.188*gasMagboltz->GetW()*eV/(gasMagboltz->GetFanoFactor() + 0.15);
    gasPhysicalProperties.meanEnergyPerIonPair = gasMagboltz->GetW()*eV;
    if(!gasMagboltz->ElectronVelocity(
        eField[0], eField[1], eField[2],
        bField[0]/tesla, bField[1]/tesla, bField[2]/tesla,
        gasPhysicalProperties.driftVel, dummy1, dummy2))
        G4cerr << "Failed to evaluate eletron drift velocity." << G4endl;
    if(!gasMagboltz->ElectronDiffusion(eField[0], eField[1], eField[2],
        bField[0]/tesla, bField[1]/tesla, bField[2]/tesla,
        gasPhysicalProperties.diffLongi, gasPhysicalProperties.diffTrans))
        G4cerr << "Failed to evaluate eletron diffusion coefficient." << G4endl;
    if(!gasMagboltz->ElectronAttachment(eField[0], eField[1], eField[2],
        bField[0]/tesla, bField[1]/tesla, bField[2]/tesla,
        gasPhysicalProperties.attachment))
        G4cerr << "Failed to evaluate eletron attach coefficient." << G4endl;
}

void GasMixtureProperties::SetElectricField(const G4ThreeVector &_eField)
{
    eField = _eField;
    UpdateParameters();
}

void GasMixtureProperties::SetElectricField(const G4double Ex, const G4double Ey, const G4double Ez)
{
    eField = G4ThreeVector(Ex, Ey, Ez);
    UpdateParameters();
}

void GasMixtureProperties::SetMagneticField(const G4ThreeVector &_bField)
{
    bField = _bField;
    UpdateParameters();
}

void GasMixtureProperties::SetMagneticField(const G4double Bx, const G4double By, const G4double Bz)
{
    bField = G4ThreeVector(Bx, By, Bz);
    UpdateParameters();
}

G4Material *GasMixtureProperties::GetGasMaterial() const
{
    return gasMaterial;
}

G4double GasMixtureProperties::GetFanoFactor() const
{
    return gasPhysicalProperties.fanoFactor;
}

G4double GasMixtureProperties::GetW() const
{
    return gasPhysicalProperties.meanEnergyPerIonPair;
}

G4double GasMixtureProperties::GetI() const
{
    return gasPhysicalProperties.meanExcitationEnergy;
}


G4double GasMixtureProperties::GetDriftVelocity() const
{
    return 1e3*gasPhysicalProperties.driftVel;
}

G4double GasMixtureProperties::GetTransverseDiffusion() const
{
    return gasPhysicalProperties.diffTrans;
}

G4double GasMixtureProperties::GetLongitudinalDiffusion() const
{
    return gasPhysicalProperties.diffTrans;
}

G4double GasMixtureProperties::GetElectronAttachment() const
{
    return gasPhysicalProperties.attachment;
}

const string GasMixtureProperties::GetGasMixtureName() const
{
    return gasMixtureName;
}

void GasMixtureProperties::DumpProperties() const
{
    printf("-----------------------------------------------------------------------------------------------------------------------\n");
    printf("Gas Mixture Name                   : %20s\n", gasMixtureName.substr(0, gasMixtureName.find_first_of("_")).c_str());
    printf("Pressure                           : %20d torr\n", pressure);
    printf("Temperature                        : %20.2f K\n", temperature/kelvin);
    printf("Electric Field                     :      (%3.0f, %3.0f, %3.0f) V/cm\n", eField[0], eField[1], eField[2]);
    printf("Magnetic Field                     :      (%1.1f, %1.1f, %1.1f) T\n", bField[0]/tesla, bField[1]/tesla, bField[2]/tesla);
    printf("Garfield++ Gas File                : %20s\n", gasFileName.substr(gasFileName.find_last_of("/") + 1, string::npos).c_str());
    printf("Mean Energy per Electron Pair      : %20.2f eV\n", GetW()/eV);
    printf("Mean Excitation Energy             : %20.2f eV\n", GetI()/eV);
    printf("Drift Velocity                     : %20.2e cm/u\n", GetDriftVelocity());
    printf("Transverse Diffusion Coefficient   : %20.2e cm^(1/2)\n", GetLongitudinalDiffusion());
    printf("Longitudinal Diffusion Coefficient : %20.2e cm^(1/2)\n", GetTransverseDiffusion());
    printf("Attachment Coefficient             : %20.2e cm^(-1)\n", GetElectronAttachment());
    printf("-----------------------------------------------------------------------------------------------------------------------\n");
}

void GasMixtureProperties::CheckArguments(
    const vector<string> &comps, const vector<G4int> &fracs,
    const G4int pressure, const G4double temperature)
{
    string _gasMixtureName = MakeGasMixtureName(comps, fracs, pressure, temperature);
    string _gasFileName = MakeGasFileName(comps, fracs, pressure);
    if(comps.size() != fracs.size())
        throw invalid_argument("The sizes of components and fractions does not match.");
    else if(comps.empty())
        throw invalid_argument("There must be one gas component at least.");
    else if(!CheckFractionSum(fracs))
        throw invalid_argument("The fractions do not sum up to 100%%.");
    else if(!IsGasFileApplicable(comps, fracs, pressure))
        throw GasMixturePropertiesException(
        "SetGasMixture(const vector<string> &comps, const vector<G4int> &, const G4int, const G4doublee)",
        GASFILE_LOAD_FAILURE, _gasFileName);
    else if(!IsMagboltzApplicable(comps))
        throw GasMixturePropertiesException(
        "SetGasMixture(const vector<string> &comps, const vector<G4int> &, const G4int, const G4doublee)",
        MAGBOLTZ_UPDATE_FAILURE, _gasMixtureName);
    else if(!IsGeantApplicable(comps))
        throw GasMixturePropertiesException(
        "SetGasMixture(const vector<string> &comps, const vector<G4int> &, const G4int, const G4doublee)",
        G4MATERIAL_UPDATE_FAILURE, _gasMixtureName);
}

G4bool GasMixtureProperties::CheckFractionSum(const vector<G4int> &fracs) const
{
    G4int fracSum = 0;
    for(auto frac : fracs)
        fracSum += frac;
    return fracSum == 100;
}

G4bool GasMixtureProperties::IsGasFileApplicable(
    const vector<string> &comps, const vector<G4int> &fracs, const G4int pressure) const
{
    return gasFileList.find(MakeGasFileName(comps, fracs, pressure)) != gasFileList.end();
}

G4bool GasMixtureProperties::IsMagboltzApplicable(const vector<string> &comps) const
{
    for(auto comp : comps)
        if(magboltzGasList.find(comp) == magboltzGasList.end())
            return false;
    return true;
}

G4bool GasMixtureProperties::IsGeantApplicable(const vector<string> &comps) const
{
    for(auto comp : comps)
        if(!GasMaterialTable::Instance()->GasMaterialExists(comp))
            return false;
    return true;
}