/// \file ParamContainer.hh
/// \brief Definition of the ParamContainer class

#ifndef ParamContainer_h
#define ParamContainer_h 1

#include "config/ParamContainerException.hh"

#include "G4String.hh"

#include <unordered_map>
#include <string>
#include <vector>

using namespace std;

/// Class containing parameter information as maps.
class ParamContainer
{
    public:
    ParamContainer(const G4String &name);
    virtual ~ParamContainer();

    G4double GetParamD(const string &parName) const;
    G4int GetParamI(const string &parName) const;
    G4bool GetParamB(const string &parName) const;
    G4String GetParamS(const string &parName) const;
    const vector<G4double> GetParamVecD(const string &parName) const;
    const vector<G4int> GetParamVecI(const string &parName) const;

    void AddParam(const string &parName, G4double);
    void AddParam(const string &parName, G4int);
    void AddParam(const string &parName, G4bool);
    void AddParam(const string &parName, G4String);
    void AddParam(const string &parName, const vector<G4double> &);
    void AddParam(const string &parName, const vector<G4int> &);

    void ListParams() const;
    private:
    G4bool CheckParamDulicated(const string &parName) const;    
    G4bool CheckParamExist(const string &parName, const string &parType) const;

    private:
    G4String fName;
    
    std::unordered_map<string, string> *fParamMapType;
    std::unordered_map<string, G4double> *fParamMapD;
    std::unordered_map<string, G4int> *fParamMapI;
    std::unordered_map<string, G4bool> *fParamMapB;
    std::unordered_map<string, G4String> *fParamMapS;
    std::unordered_map<string, vector<G4double> > *fParamMapVecD;
    std::unordered_map<string, vector<G4int> > *fParamMapVecI;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif