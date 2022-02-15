/// \file TupleVectorContainer.hh
/// \brief Definition of the TupleVectorContainer class

#ifndef TupleVectorContainer_h
#define TupleVectorContainer_h 1

#include "Exception.hh"

#include <unordered_map>
#include <string>
#include <vector>

using VectorNameMap = std::unordered_map<std::string, std::string>;
using VectorMapI = std::unordered_map<std::string, std::vector<G4int> >;
using VectorMapF = std::unordered_map<std::string, std::vector<G4float> >;
using VectorMapD = std::unordered_map<std::string, std::vector<G4double> >;
using VectorMapS = std::unordered_map<std::string, std::vector<G4String> >;


/// This class containes vectors for saving vector-type tuples.
class TupleVectorContainer
{
    public:
    TupleVectorContainer(const std::string &_containerName = "");
    TupleVectorContainer(const TupleVectorContainer &right);
    TupleVectorContainer(TupleVectorContainer &&right);

    virtual ~TupleVectorContainer();

    template<typename String, typename... Strings>
    void AddVectorsI(String vecName, Strings ... vecNames);
    template<typename String, typename... Strings>
    void AddVectorsF(String vecName, Strings ... vecNames);
    template<typename String, typename... Strings>
    void AddVectorsD(String vecName, Strings ... vecNames);
    template<typename String, typename... Strings>
    void AddVectorsS(String vecName, Strings ... vecNames);

    void AddVectorI(const std::string &vecName);
    void AddVectorF(const std::string &vecName);
    void AddVectorD(const std::string &vecName);
    void AddVectorS(const std::string &vecName);

    std::vector<G4int> &GetVectorRefI(const std::string &vecName);
    std::vector<G4float> &GetVectorRefF(const std::string &vecName);
    std::vector<G4double> &GetVectorRefD(const std::string &vecName);
    std::vector<G4String> &GetVectorRefS(const std::string &vecName);

    G4int GetNbOfVector() const;
    const std::string GetName() const;

    void FillVectorI(const std::string &vecName, const std::vector<G4int> &vec);
    void FillVectorF(const std::string &vecName, const std::vector<G4float> &vec);
    void FillVectorD(const std::string &vecName, const std::vector<G4double> &vec);
    void FillVectorS(const std::string &vecName, const std::vector<G4String> &vec);

    void ClearVectors();
    void Reset();

    G4bool VectorExists(const std::string &vecName) const;

    protected:
    const std::string containerName;
    std::string exceptionOriginClass;

    private:
    // for ends of recursive methods taking parameter packs
    void AddVectorsI(){}
    void AddVectorsF(){}
    void AddVectorsD(){}
    void AddVectorsS(){}

    Exception VectorNotFoundException(const std::string &originMethodName, const std::string &vecName) const;
    Exception VectorDuplicatedException(const std::string &originMethodName, const std::string &vecName) const;

    void DeleteMaps();
    void InitMaps();
    private:
    VectorNameMap *vectorNameMap;
    VectorMapI *vectorMapI;
    VectorMapF *vectorMapF;
    VectorMapD *vectorMapD;
    VectorMapS *vectorMapS;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename String, typename... Strings>
void TupleVectorContainer::AddVectorsI(String vecName, Strings ... vecNames)
{
    AddVectorI(vecName);
    AddVectorsI(vecNames...);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename String, typename... Strings>
void TupleVectorContainer::AddVectorsF(String vecName, Strings ... vecNames)
{
    AddVectorF(vecName);
    AddVectorsF(vecNames...);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename String, typename... Strings>
void TupleVectorContainer::AddVectorsD(String vecName, Strings ... vecNames)
{
    AddVectorD(vecName);
    AddVectorsD(vecNames...);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename String, typename... Strings>
void TupleVectorContainer::AddVectorsS(String vecName, Strings ... vecNames)
{
    AddVectorS(vecName);
    AddVectorsS(vecNames...);
}
#endif
