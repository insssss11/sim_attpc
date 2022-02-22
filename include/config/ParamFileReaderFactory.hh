/// \file ParamFileReaderFactory.hh
/// \brief Definition of the ParamFileReaderFactory class

#ifndef ParamFileReaderFactory_h
#define ParamFileReaderFactory_h 1

#include "ParamFileReader.hh"
#include "ParamFileReaderTxt.hh"
#include "config/ParamFileReaderFactoryException.hh"

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
        throw ParamFileReaderFactoryException("CreateReaderByType(const G4String &)",
            ParamFileReaderFactoryErrorNum::INVALID_READER_TYPE, readerType);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif