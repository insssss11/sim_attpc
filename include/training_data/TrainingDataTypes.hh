/// \file TrainingDataTypes.hh
/// \brief Definition of the TrainingDataTypes class

#ifndef TrainingDataTypes_h
#define TrainingDataTypes_h 1

#include "globals.hh"
#include <memory>
#include <vector>
#include <array>
#include <string>

#include "TTreeReader.h"
#include "G4ParticleDefinition.hh"
#include "G4GenericIon.hh"

namespace TrainingDataTypes {

    typedef enum {
        Nothing,
        Gamma,
        Electron,
        Proton,
        Alpha,
        Carbon,
        Oxygen,
        nParticles,
        Unknown,
    }EParticle;

    const std::array<std::string, nParticles> paricleNames = {"Nothing", "Gamma", "Electron", "Proton", "Alpha", "Carbon", "Oxygen"};


    static constexpr std::array<int, nParticles> kAtomicNum = {
        -1, // Nothing
        0, // Gamma
        1, // Proton
        2, // Alhpa
        6, // Carbon
        8  // Oxygen
    };

    static EParticle FromAtomNum(int atomicNum)
    {
        switch(atomicNum)
        {
            case 0:
                return Gamma;
            case 1:
                return Proton;
            case 2:
                return Alpha;
            case 6:
                return Carbon;
            case 8:
                return Oxygen;
            default:
                return Unknown;
        }
    }

    static EParticle FromParticleDef(const G4ParticleDefinition *parDef)
    {
        if(parDef->GetParticleName() == "gamma")
            return Gamma;
        else if(parDef->GetParticleName() == "e-")
            return Electron;
        else if(parDef->GetParticleName() == "proton")
            return Proton;
        else if(parDef->GetParticleName() == "alpha")
            return Alpha;
        else if(parDef->IsGeneralIon() && parDef->GetAtomicNumber() == 6)
            return Carbon;
        else if(parDef->IsGeneralIon() && parDef->GetAtomicNumber() == 8)
            return Oxygen;
        else
            return Unknown;
    }

    typedef struct {
        std::unique_ptr<TTreeReader> reader1, reader2, reader3;
    }DataReader;

    typedef struct {
        // tree1
        std::unique_ptr<TTreeReaderValue<int> > nTrk;
        // tree2
        std::unique_ptr<TTreeReaderValue<int> > evtId, atomNum, nStep;
        std::unique_ptr<TTreeReaderValue<float> > trkLen;
        std::unique_ptr<TTreeReaderValue<std::vector<float> > >
            x, y, z,
            px, py, pz,
            time;
        // tree3
        std::unique_ptr<TTreeReaderValue<std::vector<float> > > qdc;
    }DataReaderValue;

    typedef struct {
        EParticle particle;
        G4float trkLen;
        G4float time;
        G4float px, py, pz;
        G4float x, y, z;
    }Track;

    typedef struct {
        // the number of tracks of background carbon tracks
        int nBgCarbons;
        int nSecondaries;
        std::vector<Track> tracks;
    }Label;

    static constexpr Track emptyTrack = {
        EParticle::Nothing,
        -1., -1.,
        -1., -1., -1.,
        -1., -1., -1.};
}
#endif


