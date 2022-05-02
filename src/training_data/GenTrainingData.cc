#include "training_data/GenTrainingData.hh"
#include "training_data/GenTrainingDataException.hh"
#include "config/ParamContainerTable.hh"

#include "TTree.h"

#include "G4SystemOfUnits.hh"

#include <memory>

using namespace std;
using namespace GenTrainingDataErrorNum;
using namespace TrainingDataTypes;

GenTrainingData::GenTrainingData(const string &evtFileName, const string &bgFileName)
    : evtRootFile(nullptr), bgRootFile(nullptr),
    gammaIncluded(false), parContainer(ParamContainerTable::Instance()->GetContainer("gas_chamber"))
{
    try {
        OpenRootFiles(evtFileName, bgFileName);
        Init();
    }
    catch(GenTrainingDataException const &e)
    {
        cerr << e.what() << endl;
        CloseRootFiles();
        throw e;
    }
}

GenTrainingData::~GenTrainingData()
{
    CloseRootFiles();
}

void GenTrainingData::Init()
{
    try {
        InitDataInfo();
        InitDataReaders();
        InitDataReaderValues();
    }
    catch(ParamContainerException const &e)
    {
        cerr << e.what() << endl;
        throw GenTrainingDataException("Init()", PARAMETER_LOAD_FAILURE);
    }
}

void GenTrainingData::InitDataInfo()
{
    inputX = parContainer->GetParamI("nPadX");
    inputY = parContainer->GetParamI("nPadY");
    inputSize = inputX*inputY;
    input.reserve(inputSize);
    chargeFSR = parContainer->GetParamD("chargeFSR");
    timeFSR = parContainer->GetParamD("timeFSR");
    timeOffset = parContainer->GetParamD("timeOffset");

    SetNtrkSecondaryMax(6);
}

void GenTrainingData::InitDataReaders()
{
    InitDataReader(evtReaders, evtRootFile);
    InitDataReader(bgReaders, bgRootFile);
    if(!CheckEventNum())
        throw GenTrainingDataException("InitDataReaders()", INPUT_SIZE_INCONSISTENT);
    else
        nEvents = evtReaders.reader->GetEntries();
}

void GenTrainingData::InitDataReader(DataReader &dataReader, TFile *file)
{
    dataReader.reader = make_unique<TTreeReader>("tree_gc3", file);
}

void GenTrainingData::InitDataReaderValues()
{
    InitDataReaderValue(evtReaderValues, evtReaders);
    InitDataReaderValue(bgReaderValues, bgReaders);
}

void GenTrainingData::InitDataReaderValue(DataReaderValue &dataReaderValue, DataReader &dataReader)
{
    dataReaderValue.flag = make_unique<TTreeReaderValue<int> >(*dataReader.reader, "flag");
    dataReaderValue.Ek = make_unique<TTreeReaderValue<float> >(*dataReader.reader, "Ek");
    dataReaderValue.pxv = make_unique<TTreeReaderValue<float> >(*dataReader.reader, "pxv");
    dataReaderValue.pyv = make_unique<TTreeReaderValue<float> >(*dataReader.reader, "pyv");
    dataReaderValue.pzv = make_unique<TTreeReaderValue<float> >(*dataReader.reader, "pzv");
    dataReaderValue.xv = make_unique<TTreeReaderValue<float> >(*dataReader.reader, "xv");
    dataReaderValue.yv = make_unique<TTreeReaderValue<float> >(*dataReader.reader, "yv");
    dataReaderValue.zv = make_unique<TTreeReaderValue<float> >(*dataReader.reader, "zv");
    dataReaderValue.theta = make_unique<TTreeReaderValue<float> >(*dataReader.reader, "theta");
    dataReaderValue.trkLen = make_unique<TTreeReaderValue<float> >(*dataReader.reader, "trkLen");
    dataReaderValue.qdc = make_unique<TTreeReaderValue<std::vector<float> > >(*dataReader.reader, "qdc");
    dataReaderValue.tSig = make_unique<TTreeReaderValue<std::vector<float> > >(*dataReader.reader, "tSig");
}

G4bool GenTrainingData::CheckEventNum() const
{
    return evtReaders.reader->GetEntries() == bgReaders.reader->GetEntries();
}

void GenTrainingData::SetNtrkSecondaryMax(size_t bufferSize)
{
    nSecondaryTrkMax = bufferSize;
}

void GenTrainingData::CloseRootFiles()
{
    CloseRootFile(evtRootFile);
    CloseRootFile(bgRootFile);
    evtRootFile = nullptr;
    bgRootFile = nullptr;
}

void GenTrainingData::CloseRootFile(TFile *file)
{
    if(file == nullptr)
        return;
    else if(file->IsOpen())
        file->Close();
}

void GenTrainingData::OpenRootFiles(const string &evtFileName, const string &bgFileName)
{
    CloseRootFiles();
    evtRootFile = OpenRootFile(evtFileName);
    bgRootFile = OpenRootFile(bgFileName);
}

TFile *GenTrainingData::OpenRootFile(const string &fileName)
{
    TFile *file = TFile::Open(fileName.c_str(), "READ");
    if(file == nullptr)
        throw GenTrainingDataException("OpenRootFile(const string &)", ROOT_FILE_OPEN_FAILURE, fileName);
    else if(!file->IsOpen())
        throw GenTrainingDataException("OpenRootFile(const string &)", ROOT_FILE_OPEN_FAILURE, fileName);
    return file;
}

#include <iomanip>
void GenTrainingData::WriteTrainingData(const string &inputDataName, const string &outputDataName)
{
    std::vector<G4float> inputEvt(inputSize), inputBg(inputSize);
    ofstream inputFile, outputFile;
    inputFile.open(inputDataName);
    outputFile.open(outputDataName);
    WriteInputHeader(inputFile);
    WriteOutputHeader(outputFile);
    ResetDataReaders();
    while(NextAll())
    {
        ReadEvent();
        WriteInputData(inputFile);
        WriteOutputData(outputFile);
    }
    inputFile.close();
    outputFile.close();
}

void GenTrainingData::ResetDataReaders()
{
    ResetDataReader(evtReaders);
    ResetDataReader(bgReaders);
}

void GenTrainingData::ResetDataReader(DataReader &dataReader)
{
    dataReader.reader->Restart();
}

void GenTrainingData::ReadEvent()
{
    try {
        ReadAndMergeInput();
        ReadTracks();
    }
    catch(Exception const &e){
        e.WarnGeantKernel(JustWarning);
    }
}

G4bool GenTrainingData::NextAll()
{
    return evtReaders.reader->Next() && bgReaders.reader->Next();
}

void GenTrainingData::ReadAndMergeInput()
{
    input.clear();
    vector<float> qdcEvt = **evtReaderValues.qdc;
    vector<float> qdcBg = **bgReaderValues.qdc;
    vector<float> tSigEvt = **evtReaderValues.tSig;
    vector<float> tSigBg = **bgReaderValues.tSig;
    if(qdcEvt.size() != inputSize || qdcEvt.size() != qdcBg.size())
        throw GenTrainingDataException("ReadAndMergeInput()", PAD_SIZES_MISMATCH);
    for(size_t i = 0;i < inputSize;++i)
    {
        G4float qdc = (qdcEvt.at(i) + qdcBg.at(i)) > chargeFSR ? 1. : (qdcEvt.at(i) + qdcBg.at(i))/chargeFSR;
        if(qdc > 1e-4)
        {
            G4float time = (qdcEvt.at(i)*tSigEvt.at(i) + qdcBg.at(i)*tSigBg.at(i))/(qdcEvt.at(i) + qdcBg.at(i));
            if(time < timeOffset)
                time = 0.;
            else if(time > timeOffset + timeFSR)
                time = 1.;
            else
                time = (time - timeOffset)/timeFSR;
            input.emplace_back(i, make_pair(qdc, time));
        }
    }
}

void GenTrainingData::ReadTracks()
{
    output.flag = **evtReaderValues.flag;
    output.Ek = **evtReaderValues.Ek;
    output.pxv = **evtReaderValues.pxv;
    output.pyv = **evtReaderValues.pyv;
    output.pzv = **evtReaderValues.pzv;
    output.xv = **evtReaderValues.xv;
    output.yv = **evtReaderValues.yv;
    output.zv = **evtReaderValues.zv;
    output.theta = **evtReaderValues.theta;
    output.trkLen = **evtReaderValues.trkLen;
}

void GenTrainingData::WriteInputHeader(std::ofstream &stream)
{
    stream << "# charge is in the unit of pC, time is in the unit of ns" << endl;
    stream << "# A row of data consists of pairs of none-zero indice(starting index = 1) and their values." << endl;
    stream << setw(10) << "nEvents" << setw(15) << "inputSize"  << setw(10)
        << "inputX" << setw(10) << "inputY"  << setw(20) << "chargeFSR"  << setw(10) << "timeFSR" << setw(15) << "timeOffset" <<  endl;
    stream << setw(10) << GetEventNum() << setw(15) << inputSize << setw(10) 
        << inputX << setw(10) << inputY  << setw(20) << chargeFSR*1e12 << setw(10) << timeFSR << setw(15) << timeOffset <<  endl;
}

void GenTrainingData::WriteOutputHeader(std::ofstream &stream)
{
    stream << "# [Reaction Flag] [Ek] [pxv] [pyv] [pzv] [xv] [yv] [zv] [theta] [trkLen]" << endl;
    stream << setw(10) << "nEvents" << G4endl;
    stream << setw(10) << GetEventNum() << G4endl;;
}

void GenTrainingData::WriteInputData(std::ofstream &stream)
{
    for(const auto &p : input)
        stream << "(" << p.first << "," << p.second.first << "," << p.second.second << ")";
    stream << endl;
}

void GenTrainingData::WriteOutputData(std::ofstream &stream)
{
    stream << output.flag << " "
    << output.Ek << " "
    << output.pxv << " "
    << output.pyv << " "
    << output.pzv << " "
    << output.xv << " "
    << output.yv << " "
    << output.zv << " "
    << output.theta*180./3.141592 << " "
    << output.trkLen << " " << endl;
}

Long64_t GenTrainingData::GetEventNum() const
{
    return nEvents;
}