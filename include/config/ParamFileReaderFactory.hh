/// \file ParamFileReaderFactory.hh
/// \brief Definition of the ParamFileReaderFactory class

#ifndef ParamFileReaderFactory_h
#define ParamFileReaderFactory_h 1

#include <unordered_map>
#include "G4String.hh"
#include "G4Exception.hh"
#include "ParamFileReader.hh"
#include "ParamFileReaderTxt.hh"

/// Factory class to create derived classes of ParamFileReader.
class ParamFileReaderFactory
{
    public:
    static ParamFileReader *CreateReaderByType(const G4String &readerType);
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ParamFileReader *ParamFileReaderFactory::CreateReaderByType(const G4String &readerType)
{
    if(readerType == "txt")
        return new ParamFileReaderTxt();
    else
    {
        std::ostringstream message;
        message << "Invalide argument for CreateReaderByType(type), returning a null pointer.";
        G4Exception("ParamFileReaderFactory::CreateReaderByType(const G4String &)", "ParamFileFactory0000", FatalException, message);
        return nullptr;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif