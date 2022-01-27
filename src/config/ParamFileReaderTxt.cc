/// \file ParamFileReaderTxt.cc
/// \brief Implementation of the ParamFileReaderTxt class

#include "config/ParamFileReaderTxt.hh"


#include "G4ios.hh"

#include <sstream>

ParamFileReaderTxt::ParamFileReaderTxt()
    :ParamFileReader()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ParamFileReaderTxt::~ParamFileReaderTxt()
{
    if(fileIn.is_open())
        fileIn.close();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool ParamFileReaderTxt::OpenFile(const G4String &fileName)
{
    fileIn.open(fileName.data(), std::ios_base::in);
    if(!fileIn.is_open())
    {
        FileOpenFailureError("ParamFileReaderTxt::OpenFile(const G4String &)", fileName);
        return false;
    }
    return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ParamFileReaderTxt::FillContainer(ParamContainer *containerToFill)
{
    container = containerToFill;
    std::stringstream ss;
    std::string command;
    lineNum = 0;
    lineStr = "";
    do {
        getline(fileIn, lineStr);
        ++lineNum;
        command = ReplaceSeparator(command);
        command = TrimAndRemoveComments(lineStr);
        if(command.empty())
            continue;
        ReadLine(command);
    } while(!fileIn.eof());
    container = nullptr;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ParamFileReaderTxt::ReadLine(std::string command)
{
    std::string parName, parType, parValue;
    std::stringstream ss;
    ss.str(command);
    ss >> parName >> parType;
    getline(ss, parValue, '\n');
    // G4cout << parName << ":" << parType << ":" << parValue << G4endl;
    AddParamToContainer(parName, parType, trim(parValue));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ParamFileReaderTxt::AddParamToContainer(std::string parName, std::string parType, std::string parValue)
{
    if(parName.empty() || parValue.empty())
        LineReadingFailureWarning("ParamFileReaderTxt::AddParamToContainer(std::string, std::string, std::string, ParamContainer *)");
    else if(parType == "double")
        container->AddParam(parName, atof(parValue.data()));
    else if(parType == "int")
        container->AddParam(parName, atoi(parValue.data()));
    else if(parType == "bool")
        container->AddParam(parName, parValue == "true" || parValue == "True" || parValue == "TRUE");
    else if(parType == "string")
        container->AddParam(parName, parValue);
    else if(parType == "VectorD")
        container->AddParam(parName, ConvertToVecD(parValue));
    else if(parType == "VectorI")
        container->AddParam(parName, ConvertToVecI(parValue));
    else
        LineReadingFailureWarning("ParamFileReaderTxt::AddParamToContainer(std::string, std::string, std::string, ParamContainer *)");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::string ParamFileReaderTxt::TrimAndRemoveComments(std::string line)
{
    // G4cout << line.substr(0, line.find_first_of('#')) << G4endl;
    return line.substr(0, line.find_first_of('#'));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::string ParamFileReaderTxt::ReplaceSeparator(std::string line)
{
    line = line.replace(line.begin(), line.end(), '\t', ' ');
    line = line.replace(line.begin(), line.end(), ',', ' ');
    return line;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ParamFileReaderTxt::LineReadingFailureWarning(const G4String &where)
{
    std::ostringstream message;
    message << "Cannot read line " << lineNum << " : " << std::endl << lineStr;
    G4Exception(where.data(), "ParamFile0001", JustWarning, message);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::vector<double> ParamFileReaderTxt::ConvertToVecD(std::string parValue)
{
    std::vector<double> output;
    std::stringstream ss(parValue);
    std::string token;
    while(ss >> token)
        output.push_back(atof(token.data()));
    return output;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::vector<int> ParamFileReaderTxt::ConvertToVecI(std::string parValue)
{
    std::vector<int> output;
    std::stringstream ss(parValue);
    std::string token;
    while(ss >> token)
        output.push_back(atoi(token.data()));
    return output;
}