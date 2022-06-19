/// \file GenTrainingData.hh
/// \brief Definition of the GenTrainingData class

#ifndef GenTrainingData_h
#define GenTrainingData_h 1

#include "training_data/TrainingDataTypes.hh"

#include "globals.hh"

#include <fstream>
#include <string>
#include <memory>
#include <array>

#include "TFile.h"
#include "TTreeReader.h"

#include "config/ParamContainerTable.hh"

enum EReactionTypes{carbonAlpha, protonScat, alphaScat, carbonScat, nReactionTypes};

typedef struct {
    std::unique_ptr<TTreeReader> reader;

    std::unique_ptr<TTreeReaderValue<int> > flag; 
    std::unique_ptr<TTreeReaderValue<float> > Ek;
    std::unique_ptr<TTreeReaderValue<float> > pxv, pyv, pzv;
    std::unique_ptr<TTreeReaderValue<float> > xv, yv, zv;
    std::unique_ptr<TTreeReaderValue<float> > theta;
    std::unique_ptr<TTreeReaderValue<float> > trkLen;
    std::unique_ptr<TTreeReaderValue<float> > Ebeam;
    std::unique_ptr<TTreeReaderValue<float> > Egm;
    std::unique_ptr<TTreeReaderValue<float> > thetaGm;
    std::unique_ptr<TTreeReaderValue<float> > phiGm;

    std::unique_ptr<TTreeReaderValue<std::vector<float> > > qdc;
    std::unique_ptr<TTreeReaderValue<std::vector<float> > > tSig;
    std::unique_ptr<TTreeReaderValue<std::vector<int> > > secFlags;
}DataReader;

typedef struct {
    std::array<int, nReactionTypes> reactionFlags;
    float Ek, Ebeam, Egm;
    float pxv, pyv, pzv;
    float xv, yv, zv;
    float theta;
    float trkLen;
    float thetaGm, phiGm;
}Output;

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
    void InitDataReader(DataReader &dataReader, TFile *file);

    G4bool CheckEventNum() const;
    G4bool CheckReaders();
    G4bool CheckReaderValues();
    void ResetDataReaders();
    void ResetDataReader(DataReader &dataReader);

    void OpenRootFiles(const std::string &evtFileName, const std::string &bgFileName);
    TFile *OpenRootFile(const std::string &fileName);

    void CloseRootFile(TFile *file);
    void CloseRootFiles();

    void ReadEvent();
    G4bool NextAll();

    void ReadAndMergeInput();
    void ReadTracks();

    void ProcessSecondaries(const vector<int> &secFlags);

    void WriteInputHeader(std::ofstream &stream);
    void WriteOutputHeader(std::ofstream &stream);

    void WriteInputData(std::ofstream &stream);
    void WriteOutputData(std::ofstream &stream);

    private:
    G4float chargeFSR, timeFSR, timeOffset;

    Long64_t nEvents;
    size_t inputSize;
    G4double padPlaneX, padPlaneY;
    size_t inputX, inputY;
    // [idx, [charge, time]]
    std::vector<std::pair<size_t, std::pair<G4float, G4float> > > input;

    TFile *evtRootFile, *bgRootFile;

    // buffer size of output output, the last are filled with EmptyTrack defined in "TrainingDataTypes.hh"
    size_t nSecondaryTrkMax;

    Output output;
    DataReader evtReaders, bgReaders;

    G4bool gammaIncluded;
    const ParamContainer *parContainer;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


