/// \file ParamFileReaderTxt.hh
/// \brief Definition of the ParamFileReaderTxt class

#ifndef ParamFileReaderTxt_h
#define ParamFileReaderTxt_h 1

#include "config/ParamFileReader.hh"

#include "G4String.hh"
#include "G4ThreeVector.hh"

#include <fstream>

/// This class is derived from ParamFileReader and reads text parameter file(can read txt file with separator as white space or comma).
class ParamFileReaderTxt : public ParamFileReader
{
    public:
    ParamFileReaderTxt();
    virtual ~ParamFileReaderTxt();
    virtual G4bool OpenFile(const G4String &fileName) override;
    virtual void FillContainer(ParamContainer *containerToFill) override;

    private:
    void ReadLine(std::string command);
    void AddParamToContainer(std::string parName, std::string parType, std::string parValue);
    G4bool CheckParValidity(std::string parName, std::string parType, std::string parValue);
    
    std::vector<double> ConvertToVecD(std::string parValue);
    std::vector<int> ConvertToVecI(std::string parValue);

    std::string TrimAndRemoveComments(std::string line);
    // replace all separotor to space
    std::string ReplaceSeparator(std::string line);
    
    void LineReadingFailureWarning(const G4String &where);    
    private:
    std::ifstream fileIn;
    int lineNum;
    std::string lineStr;
};

// copied from https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring
// Thanks to Galik
// trim from end of string (right)
inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from beginning of string (left)
inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from both ends of string (right then left)
inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
{
    return ltrim(rtrim(s, t), t);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif