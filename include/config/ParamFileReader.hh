/// \file ParamFileReader.hh
/// \brief Definition of the ParamFileReader class

#ifndef ParamFileReader_h
#define ParamFileReader_h 1

#include "config/ParamContainer.hh"

#include "G4String.hh"
#include "G4Exception.hh"

#include <unordered_map>

/// Abstract class to read a general parameter file and fill parameter container.
class ParamFileReader
{
    public:
    ParamFileReader() {}
    virtual ~ParamFileReader(){}

    virtual G4bool OpenFile(const G4String &fileName) = 0;
    virtual void FillContainer(ParamContainer *containerToFill) = 0;
    protected:
    void FileOpenFailureError(const G4String &where, const G4String &fName)
    {
        std::ostringstream message;
        message << "Failed to open " << fName << ".";
        G4Exception(where.data(), "ParamFile0000", FatalException, message);
    }
    ParamContainer *container;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif