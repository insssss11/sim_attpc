/// \file GenTrainingData.hh
/// \brief Definition of the GenTrainingData class

#ifndef GenTrainingData_h
#define GenTrainingData_h 1

#include "training_data/TrainingDataTypes.hh"

#include "globals.hh"

#include <fstream>
#include <string>
#include <memory>

#include "TFile.h"
#include "TTreeReader.h"

// this class reads a pair of root file saved by pad_charges.mac and generates training data for DNN.
class GenTrainingData
{
    public:
    GenTrainingData(const std::string &evtFileName, const std::string &bgFileName);
    virtual ~GenTrainingData();

    G4bool IsOpen();
    void WriteTrainingData(const std::string &inputDataName, const std::string &outputDataName);
    void IncludeGamma(G4bool gammaIncluded = true);
    
    // set the size of buffer for outputs(vertice) of DNN
    // If the size of output is smaller than the buffer size, the last are filled with "Nothing" particle output.
    void SetNtrkSecondaryMax(size_t bufferSize);
    Long64_t GetEventNum() const;
    
    private:
    void Init();
    
    void InitDataInfo();
    
    void InitDataReaders();
    void InitDataReader(TrainingDataTypes::DataReader &dataReader, TFile *file);
    
    void InitDataReaderValues();
    void InitDataReaderValue(TrainingDataTypes::DataReaderValue &dataReaderValue, TrainingDataTypes::DataReader &dataReader);
    
    G4bool CheckEventNum() const;
    G4bool CheckReaders();
    G4bool CheckReaderValues();
    void ResetDataReaders();
    void ResetDataReader(TrainingDataTypes::DataReader &dataReader);
    void ResetOutputs();

    void OpenRootFiles(const std::string &evtFileName, const std::string &bgFileName);
    TFile *OpenRootFile(const std::string &fileName);

    void CloseRootFile(TFile *file);
    void CloseRootFiles();

    void ReadEvent();
    G4bool NextAll();

    void ReadAndMergeInput();
    void ReadTracks(TrainingDataTypes::DataReaderValue &dataReaderValue, TrainingDataTypes::DataReader &dataReader);
    void SortOutputLabel();

    void ListParticleLabels(std::ofstream &stream);
    void WriteInputHeader(std::ofstream &stream);
    void WriteOutputHeader(std::ofstream &stream);

    void WriteInputData(std::ofstream &stream);
    void WriteOutputData(std::ofstream &stream);

    private:
    G4float fullScaleRange;

    Long64_t nEvents;
    size_t inputSize;
    std::vector<std::pair<size_t, G4float> > input;
    
    TFile *evtRootFile, *bgRootFile;

    // buffer size of output output, the last are filled with EmptyTrack defined in "TrainingDataTypes.hh"
    size_t nSecondaryTrkMax;

    TrainingDataTypes::Label output;
    TrainingDataTypes::DataReader evtReaders, bgReaders;
    TrainingDataTypes::DataReaderValue evtReaderValues, bgReaderValues;

    G4bool gammaIncluded;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    