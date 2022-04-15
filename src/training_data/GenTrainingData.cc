#include "training_data/GenTrainingData.hh"
#include "training_data/GenTrainingDataException.hh"
#include "config/ParamContainerTable.hh"

#include "TTree.h"

#include <memory>

using namespace std;
using namespace GenTrainingDataErrorNum;
using namespace TrainingDataTypes;

GenTrainingData::GenTrainingData(const string &evtFileName, const string &bgFileName)
    : evtRootFile(nullptr), bgRootFile(nullptr),
    gammaIncluded(false)
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
    auto parContainer = ParamContainerTable::Instance()->GetContainer("gas_chamber");
    inputSize = parContainer->GetParamI("nPadX")*parContainer->GetParamI("nPadY");
    input.reserve(inputSize);
    fullScaleRange = parContainer->GetParamD("fullScaleRange");
    SetNtrkSecondaryMax(6);
}

void GenTrainingData::InitDataReaders()
{
    InitDataReader(evtReaders, evtRootFile);
    InitDataReader(bgReaders, bgRootFile);
    if(!CheckEventNum())
        throw GenTrainingDataException("InitDataReaders()", INPUT_SIZE_INCONSISTENT);
    else
        nEvents = evtReaders.reader1->GetEntries();
}

void GenTrainingData::InitDataReader(DataReader &dataReader, TFile *file)
{
    dataReader.reader1 = make_unique<TTreeReader>("tree_gc1", file);
    dataReader.reader2 = make_unique<TTreeReader>("tree_gc2", file);
    dataReader.reader3 = make_unique<TTreeReader>("tree_gc3", file);
}

void GenTrainingData::InitDataReaderValues()
{
    InitDataReaderValue(evtReaderValues, evtReaders);
    InitDataReaderValue(bgReaderValues, bgReaders);
}

void GenTrainingData::InitDataReaderValue(DataReaderValue &dataReaderValue, DataReader &dataReader)
{
    dataReaderValue.nTrk = make_unique<TTreeReaderValue<int> >(*dataReader.reader1, "Ntrk");

    dataReaderValue.evtId = make_unique<TTreeReaderValue<int> >(*dataReader.reader2, "evtId");
    dataReaderValue.particleEnum = make_unique<TTreeReaderValue<int> >(*dataReader.reader2, "pEnum");
    dataReaderValue.nStep = make_unique<TTreeReaderValue<int> >(*dataReader.reader2, "Nstep");

    dataReaderValue.x = make_unique<TTreeReaderValue<std::vector<float> > >(*dataReader.reader2, "x");
    dataReaderValue.y = make_unique<TTreeReaderValue<std::vector<float> > >(*dataReader.reader2, "y");
    dataReaderValue.z = make_unique<TTreeReaderValue<std::vector<float> > >(*dataReader.reader2, "z");
    dataReaderValue.px = make_unique<TTreeReaderValue<std::vector<float> > >(*dataReader.reader2, "px");
    dataReaderValue.py = make_unique<TTreeReaderValue<std::vector<float> > >(*dataReader.reader2, "py");
    dataReaderValue.pz = make_unique<TTreeReaderValue<std::vector<float> > >(*dataReader.reader2, "pz");
    dataReaderValue.time = make_unique<TTreeReaderValue<std::vector<float> > >(*dataReader.reader2, "t");
    dataReaderValue.trkLen = make_unique<TTreeReaderValue<float> >(*dataReader.reader2, "trkLen");

    dataReaderValue.qdc = make_unique<TTreeReaderValue<std::vector<float> > >(*dataReader.reader3, "qdc");
}

G4bool GenTrainingData::CheckEventNum() const
{
    return evtReaders.reader1->GetEntries() == bgReaders.reader1->GetEntries();
}

void GenTrainingData::SetNtrkSecondaryMax(size_t bufferSize)
{
    nSecondaryTrkMax = bufferSize;
    output.tracks.resize(nSecondaryTrkMax);
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
    dataReader.reader1->Restart();
    dataReader.reader2->Restart();
    dataReader.reader3->Restart();
}

void GenTrainingData::ReadEvent()
{
    try {
        ReadAndMergeInput();
        ResetOutputs();
        ReadTracks(evtReaderValues, evtReaders);
        ReadTracks(bgReaderValues, bgReaders);
        SortOutputLabel();
    }
    catch(Exception const &e){
        e.WarnGeantKernel(JustWarning);
    }
}

G4bool GenTrainingData::NextAll()
{
    return evtReaders.reader1->Next() && bgReaders.reader1->Next() && evtReaders.reader3->Next() && bgReaders.reader3->Next();
}

void GenTrainingData::ReadAndMergeInput()
{
    input.clear();
    vector<float> qdcEvt = **evtReaderValues.qdc;
    vector<float> qdcBg = **bgReaderValues.qdc;
    if(qdcEvt.size() != inputSize || qdcEvt.size() != qdcBg.size())
        throw GenTrainingDataException("ReadAndMergeInput()", PAD_SIZES_MISMATCH);
    for(size_t i = 0;i < inputSize;++i)
    {
        G4float qdc = (qdcEvt.at(i) + qdcBg.at(i)) > fullScaleRange ? 1. : (qdcEvt.at(i) + qdcBg.at(i))/fullScaleRange;
        if(qdc > 0)
            input.emplace_back(i, qdc);
    }
}

void GenTrainingData::ResetOutputs()
{
    output.nBgCarbons = 0;
    output.nSecondaries = 0;
    output.tracks.assign(nSecondaryTrkMax, emptyTrack);
}

void GenTrainingData::ReadTracks(DataReaderValue &dataReaderValue, DataReader &dataReader)
{
    int nTrk = **dataReaderValue.nTrk;
    int evtId;
    float trkLen;
    vector<float> x, y, z, px, py, pz, time;
    int curEvtId = -1;
    try {
        int nOxygen = 0;
        for(int trkId = 0;trkId < nTrk;++trkId)
        {
            dataReader.reader2->Next();
            evtId = **dataReaderValue.evtId;
            EParticle parEnum = static_cast<EParticle>(**dataReaderValue.particleEnum);
            if(curEvtId == -1)
                curEvtId = evtId;
            else if(curEvtId != evtId)
                throw GenTrainingDataException("ReadTracks(DataReaderValue &)", EVENT_ID_MISMATCH, to_string(curEvtId), to_string(evtId));
            if(parEnum == EParticle::Carbon)
                ++output.nBgCarbons;
            else if(parEnum == Gamma && !gammaIncluded)
                continue; // if gammaIncluded flag not set, ignore gamma track.
            else
            {
                if(parEnum == EParticle::Oxygen)
                    ++nOxygen;
                x = **dataReaderValue.x;
                y = **dataReaderValue.y;
                z = **dataReaderValue.z;
                px = **dataReaderValue.px;
                py = **dataReaderValue.py;
                pz = **dataReaderValue.pz;
                time = **dataReaderValue.time;
                trkLen = **dataReaderValue.trkLen;
                output.tracks.at(output.nSecondaries) = {
                    parEnum,
                    trkLen, time.at(0),
                    px.at(0), py.at(0), pz.at(0),
                    x.at(0), y.at(0), z.at(0)};
                ++output.nSecondaries;
            }
        }
        // The number of oxygen tracks must be subtracted to that of background Carbon tracks
        output.nBgCarbons -= nOxygen;
    }
    catch(out_of_range const &e)
    {
        cerr << "The number of track : " << output.tracks.size() << endl;
        throw GenTrainingDataException("ReadTracks(DataReaderValue &, DataReader &)", OUTPUT_OUT_OF_RANGE);
    }
}

bool compareTrack(const Track &trk1, const Track &trk2)
{
    return trk1.time < trk2.time;
}

#include <algorithm>

void GenTrainingData::SortOutputLabel()
{
    sort(output.tracks.begin(), output.tracks.begin() + output.nSecondaries, compareTrack);
}

void GenTrainingData::ListParticleLabels(std::ofstream &stream)
{
    stream << "##########################################################################################################" << endl;
    stream << "# Particle Labels" << endl;
    for(int i = 0;i < nParticles;++i)
    {
        stream << "# " << setw(10) << paricleNames.at(i) << " : " << setw(10) << i << endl;
    }
    stream << "##########################################################################################################" << endl;
}

void GenTrainingData::WriteInputHeader(std::ofstream &stream)
{
    ListParticleLabels(stream);
    stream << "# fullScaleRange is in the unit of pC" << endl;
    stream << "# A row of data consists of pairs of none-zero indice(starting index = 1) and their values." << endl;
    stream << setw(10) << "nEvents" << setw(15) << "inputSize"  << setw(20) << "fullScaleRange" << endl;
    stream << setw(10) << GetEventNum() << setw(15) << inputSize << setw(20) << fullScaleRange*1e12 << endl;
}

void GenTrainingData::WriteOutputHeader(std::ofstream &stream)
{
    ListParticleLabels(stream);
    stream << "# [# of background C12 ions] [paticle labels 1...nSecondaryTrkMax] [x positions(mm)] [y, z positions(mm)] [px(MeV/c)] [py, pz(MeV/c)] [trkLen(mm)]" << endl;
    stream << setw(10) << "nEvents" << setw(20) << "nSecondaryTrkMax" << setw(20) << "nParticleLabels" << endl;
    stream << setw(10) << GetEventNum() << setw(20) << nSecondaryTrkMax << setw(20) << nParticles << endl;
}

void GenTrainingData::WriteInputData(std::ofstream &stream)
{
    for(const auto &p : input)
        stream << "(" << p.first << "," << p.second << ")";
    stream << endl;
}

void GenTrainingData::WriteOutputData(std::ofstream &stream)
{
    stream << output.nBgCarbons << " ";
    for(const auto &trk: output.tracks)
        stream << trk.particle << " ";
    for(const auto &trk: output.tracks)
        stream << trk.x << " ";
    for(const auto &trk: output.tracks)
        stream << trk.y << " ";
    for(const auto &trk: output.tracks)
        stream << trk.z << " ";
    for(const auto &trk: output.tracks)
        stream << trk.px << " ";
    for(const auto &trk: output.tracks)
        stream << trk.py << " ";
    for(const auto &trk: output.tracks)
        stream << trk.pz << " ";
    for(const auto &trk: output.tracks)
        stream << trk.trkLen << " ";
    stream << endl;
}

Long64_t GenTrainingData::GetEventNum() const
{
    return nEvents;
}