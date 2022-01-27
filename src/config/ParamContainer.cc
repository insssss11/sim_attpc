/// \file ParamContainer.cc
/// \brief Implementation of the ParamContainer class

#include "config/ParamContainer.hh"
#include "G4Exception.hh"
#include <iomanip>

ParamContainer::ParamContainer(const G4String &name)
    :fName(name)
{
    fParamMapType = new std::unordered_map<string, string>{};
    fParamMapD = new std::unordered_map<string, G4double>{};
    fParamMapI = new std::unordered_map<string, G4int>{};
    fParamMapB = new std::unordered_map<string, G4bool>{};
    fParamMapS = new std::unordered_map<string, G4String>{};
    fParamMapVecD = new std::unordered_map<string, vector<G4double> >{};
    fParamMapVecI = new std::unordered_map<string, vector<G4int> >{};
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ParamContainer::~ParamContainer()
{
    delete fParamMapType;
    delete fParamMapD;
    delete fParamMapI;
    delete fParamMapB;
    delete fParamMapS;
    delete fParamMapVecD;
    delete fParamMapVecI;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double ParamContainer::GetParamD(const string &parName) const
{
    if(!CheckParamExist(parName, "double"))
        ParamNotFoundError("ParamContainer::GetParamD(const string &)", parName, "double");
    return fParamMapD->at(parName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int ParamContainer::GetParamI(const string &parName) const
{
    if(!CheckParamExist(parName, "int"))
        ParamNotFoundError("ParamContainer::GetParamD(const string &)", parName, "int");
    return fParamMapI->at(parName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool ParamContainer::GetParamB(const string &parName) const
{
    if(!CheckParamExist(parName, "bool"))
        ParamNotFoundError("ParamContainer::GetParamD(const string &)", parName, "boolean");
    return fParamMapB->at(parName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String ParamContainer::GetParamS(const string &parName) const
{
    if(!CheckParamExist(parName, "string"))
        ParamNotFoundError("ParamContainer::GetParamD(const string &)", parName, "string");
    return fParamMapS->at(parName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const vector<G4double> ParamContainer::GetParamVecD(const string &parName) const
{
    if(!CheckParamExist(parName, "VectorD"))
        ParamNotFoundError("ParamContainer::GetParamD(const string &)", parName, "vector<double>");
    return fParamMapVecD->at(parName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const vector<G4int> ParamContainer::GetParamVecI(const string &parName) const
{
    if(!CheckParamExist(parName, "VectorI"))
        ParamNotFoundError("ParamContainer::GetParamD(const string &)", parName, "vector<int>");
    return fParamMapVecI->at(parName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ParamContainer::AddParam(const string &parName, G4double value)
{
    if(!CheckParamDulicated(parName))
    {
        fParamMapD->insert(std::make_pair(parName, value));
        fParamMapType->insert(std::make_pair(parName, "double"));
    }
    else
        ParamDuplicatedWarning("ParamContainer::AddParam(const string &, G4double)", parName, fParamMapType->at(parName));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ParamContainer::AddParam(const string &parName, G4int value)
{
    if(!CheckParamDulicated(parName))
    {
        fParamMapI->insert(std::make_pair(parName, value));
        fParamMapType->insert(std::make_pair(parName, "int"));
    }
    else
        ParamDuplicatedWarning("ParamContainer::AddParam(const string &, G4int)", parName, fParamMapType->at(parName));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ParamContainer::AddParam(const string &parName, G4bool value)
{
    if(!CheckParamDulicated(parName))
    {
        fParamMapB->insert(std::make_pair(parName, value));
        fParamMapType->insert(std::make_pair(parName, "bool"));
    }
    else
        ParamDuplicatedWarning("ParamContainer::AddParam(const string &, G4bool)", parName, fParamMapType->at(parName));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ParamContainer::AddParam(const string &parName, G4String value)
{
    if(!CheckParamDulicated(parName))
    {
        fParamMapS->insert(std::make_pair(parName, value));
        fParamMapType->insert(std::make_pair(parName, "string"));
    }
    else
        ParamDuplicatedWarning("ParamContainer::AddParam(const string &, G4String)", parName, fParamMapType->at(parName));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ParamContainer::AddParam(const string &parName, const vector<G4double> &value)
{
    if(!CheckParamDulicated(parName))
    {
        fParamMapVecD->insert(std::make_pair(parName, value));
        fParamMapType->insert(std::make_pair(parName, "VectorD"));
    }
    else
        ParamDuplicatedWarning("ParamContainer::AddParam(const string &, const vector<G4double> &)", parName, fParamMapType->at(parName));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ParamContainer::AddParam(const string &parName, const vector<G4int> &value)
{
    if(!CheckParamDulicated(parName))
    {
        fParamMapVecI->insert(std::make_pair(parName, value));
        fParamMapType->insert(std::make_pair(parName, "VectorI"));
    }
    else
        ParamDuplicatedWarning("ParamContainer::AddParam(const string &, const vector<G4int> &)", parName, fParamMapType->at(parName));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ParamContainer::ListParams() const
{
    G4cout << "Parameter List of Container " << fName << G4endl;
    G4cout << "    parName" << std::setw(13) << "  parType" << std::setw(13)  << G4endl;
    for(auto p : *fParamMapD)
        G4cout << std::setw(13) << p.first << "  "
            << std::setw(8) << "double" << std::setw(13) << p.second << G4endl;
    for(auto p : *fParamMapI)
        G4cout << std::setw(13) << p.first <<  "  "
            << std::setw(8) << "int" << std::setw(13) << p.second << G4endl;
    for(auto p : *fParamMapB)
        G4cout << std::setw(13) << p.first << "  "
            << std::setw(8) << "bool" << std::setw(13) << p.second << G4endl;
    for(auto p : *fParamMapS)
        G4cout << std::setw(13) << p.first << "  "
            << std::setw(8) << "string" << std::setw(13) << p.second << G4endl;
    for(auto p : *fParamMapVecD)
    {
        G4cout << std::setw(13) << p.first << "  "
            << std::setw(8) << "VectorD     ";
        auto prec = G4cout.precision(5);
        for(size_t i = 0;i < p.second.size();++i)
            if(i == 6)
            {
                G4cout << " ...";
                break;
            }
            else
                G4cout << std::setw(8) << p.second.at(i) << "  ";
        G4cout << G4endl;
        G4cout.precision(prec);
    }
    for(auto p : *fParamMapVecI)
    {
        G4cout << std::setw(13) << p.first << "  "
            << std::setw(8) << " VectorI     ";  
        for(size_t i = 0;i < p.second.size();++i)
            if(i == 6)
            {
                G4cout << " ...";
                break;
            }
            else
                G4cout << std::setw(8) << p.second.at(i) << "  ";
        G4cout << G4endl;
    }
       
}   

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool ParamContainer::CheckParamDulicated(const string &parName) const
{
    return fParamMapType->find(parName) != fParamMapType->end();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool ParamContainer::CheckParamExist(const string &parName, const string &parType) const
{
    if(fParamMapType->find(parName) == fParamMapType->end())
        return false;
    return parType == fParamMapType->at(parName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ParamContainer::ParamNotFoundError(const G4String &where,
    const G4String &parName, const G4String &parType) const
{
    std::ostringstream message;
    message << "A parameter " << parName << " with type " << parType << " does not exists.";
    G4Exception(where.data(), "ParamContainer0000", FatalException, message);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ParamContainer::ParamDuplicatedWarning(const G4String &where,
    const G4String &parName, const G4String &parType) const
{
    std::ostringstream message;
    message << "A parameter " << parName << " with type " << parType << " already exists.";
    G4Exception(where.data(), "ParamContainer0001", JustWarning, message);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


