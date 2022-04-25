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
        Emtpy,
        Gamma,
        Electron,
        Proton,
        Alpha,
        Carbon,
        Oxygen,
        Unknown,
        nParticles,
    }EParticle;
    const std::array<std::string, nParticles> paricleNames = {"Empty", "Gamma", "Electron", "Proton", "Alpha", "Carbon", "Oxygen", "Unknown"};

    static constexpr std::array<int, nParticles> kAtomicNum = {
        -1, // Empty
        0, // Gamma
        1, // Proton
        2, // Alhpa
        6, // Carbon
        8  // Oxygen
    };

    inline EParticle FromAtomNum(int atomicNum)
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

    inline EParticle FromParticleDef(const G4ParticleDefinition *parDef)
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
        std::unique_ptr<TTreeReaderValue<int> > evtId, particleEnum, nStep;
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
        int reactionFlag;
        G4float px, py, pz;
        G4float x1, y1, z1;        
        G4float x2, y2, z2;
        G4float trkLen;
    }Output;

    static constexpr Track emptyTrack = {
        EParticle::Emtpy,
        -1., -1.,
        -1., -1., -1.,
        -1., -1., -1.};
}
#endif


