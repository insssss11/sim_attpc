/// \file TupleVectorContainerException.cc
/// \brief Implementation of the TupleVectorContainerException class

#include "analysis/TupleVectorContainerException.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TupleVectorContainerException TupleVectorContainerException::TupleNotFoundException(
    const std::string &where, const std::string &tName)
{
    message = std::string("Tuple " + tName + " does not exists.");
    whereAll = "TupleVectorContainer<T>::" + where;
    G4Exception(whereAll.c_str(), "TupleVecCon0000", JustWarning, message.c_str());
    return TupleVectorContainerException(message);
}

TupleVectorContainerException TupleVectorContainerException::TupleDuplicatedException(
    const std::string &where, const std::string &tName)
{
    message = "Tuple " + tName + " already exists.";
    whereAll = "TupleVectorContainer<T>::" + where;
    G4Exception(whereAll.c_str(), "TupleVecCon0001", JustWarning, message.c_str());
    return TupleVectorContainerException(message);
}

TupleVectorContainerException TupleVectorContainerException::VectorNotFoundException(
    const std::string &where, const std::string &tName, const std::string &vecName)
{
    message = "There is no vector " + vecName + " in the tuple " + tName + ".";
    whereAll = "TupleVectorContainer<T>::" + where;
    G4Exception(whereAll.c_str(), "TupleVecCon0002", JustWarning, message.c_str());
    return TupleVectorContainerException(message);
}

TupleVectorContainerException TupleVectorContainerException::VectorDuplicatedException(
    const std::string &where, const std::string &tName, const std::string &vecName)
{
    message = "Vector " + vecName + " already exists in the tuple " + tName + ".";
    whereAll = "TupleVectorContainer<T>::" + where;
    G4Exception(whereAll.c_str(), "TupleVecCon0003", JustWarning, message.c_str());
    return TupleVectorContainerException(message);
}
