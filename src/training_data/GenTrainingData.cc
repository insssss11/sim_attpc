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

    dataReader.flag = make_unique<TTreeReaderValue<int> >(*dataReader.reader, "flag");
    dataReader.Ek = make_unique<TTreeReaderValue<float> >(*dataReader.reader, "Ek");
    dataReader.pxv = make_unique<TTreeReaderValue<float> >(*dataReader.reader, "pxv");
    dataReader.pyv = make_unique<TTreeReaderValue<float> >(*dataReader.reader, "pyv");
    dataReader.pzv = make_unique<TTreeReaderValue<float> >(*dataReader.reader, "pzv");
    dataReader.xv = make_unique<TTreeReaderValue<float> >(*dataReader.reader, "xv");
    dataReader.yv = make_unique<TTreeReaderValue<float> >(*dataReader.reader, "yv");
    dataReader.zv = make_unique<TTreeReaderValue<float> >(*dataReader.reader, "zv");
    dataReader.theta = make_unique<TTreeReaderValue<float> >(*dataReader.reader, "theta");
    dataReader.trkLen = make_unique<TTreeReaderValue<float> >(*dataReader.reader, "trkLen");
    dataReader.qdc = make_unique<TTreeReaderValue<std::vector<float> > >(*dataReader.reader, "qdc");
    dataReader.tSig = make_unique<TTreeReaderValue<std::vector<float> > >(*dataReader.reader, "tSig");
    dataReader.secFlags = make_unique<TTreeReaderValue<std::vector<int> > >(*dataReader.reader, "secFlags");
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
    vector<float> qdcEvt = **evtReaders.qdc;
    vector<float> qdcBg = **bgReaders.qdc;
    vector<float> tSigEvt = **evtReaders.tSig;
    vector<float> tSigBg = **bgReaders.tSig;

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
    output.Ek = **evtReaders.Ek;
    output.pxv = **evtReaders.pxv;
    output.pyv = **evtReaders.pyv;
    output.pzv = **evtReaders.pzv;
    output.xv = **evtReaders.xv;
    output.yv = **evtReaders.yv;
    output.zv = **evtReaders.zv;
    output.theta = **evtReaders.theta;
    output.trkLen = **evtReaders.trkLen;

    vector<int> secFlagsEvt = **evtReaders.secFlags;
    vector<int> secFlagsBg = **bgReaders.secFlags;
    output.reactionFlags.fill(0);
    ProcessSecondaries(secFlagsEvt);
    ProcessSecondaries(secFlagsBg);
}

void GenTrainingData::ProcessSecondaries(const vector<int> &secFlags)
{
    if(**evtReaders.flag == 1)
            output.reactionFlags.at(0) = 1;

    // reactionCondis[parEnum] = a particle required to set reaction flag on
    constexpr int reactionCondis[nReactionTypes] = 
    {
        Oxygen, // carbonAlpha
        Proton, // protonScat
        Alpha, // alphaScat
        Carbon, // carbonScat
    };
    for(int i = 0;i <  static_cast<int>(nReactionTypes);++i)
    {
        if(secFlags.at(reactionCondis[i]) == 1)
            output.reactionFlags.at(i) = 1;
    }
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
    stream << "# [Reaction Flags] [Ek] [pxv] [pyv] [pzv] [xv] [yv] [zv] [theta] [trkLen]" << endl;
    stream << setw(10) << "nEvents"  << setw(15) << "nReactionTypes" << G4endl;
    stream << setw(10) << GetEventNum()  << setw(15) << nReactionTypes << G4endl;
}

void GenTrainingData::WriteInputData(std::ofstream &stream)
{
    for(const auto &p : input)
        stream << "(" << p.first << "," << p.second.first << "," << p.second.second << ")";
    stream << endl;
}

void GenTrainingData::WriteOutputData(std::ofstream &stream)
{
    for(const auto &reactionFlag : output.reactionFlags)
        stream << reactionFlag << " ";
    stream << output.Ek << " "
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