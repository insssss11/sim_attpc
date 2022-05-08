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
        Empty,
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
        0, // Electron
        1, // Proton
        2, // Alhpa
        6, // Carbon
        8, // Oxygen
        -1 // Unknown
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
        std::unique_ptr<TTreeReader> reader;
    }DataReader;

    typedef struct {
        std::unique_ptr<TTreeReaderValue<int> > flag;
        std::unique_ptr<TTreeReaderValue<float> > Ek;
        std::unique_ptr<TTreeReaderValue<float> > pxv, pyv, pzv;
        std::unique_ptr<TTreeReaderValue<float> > xv, yv, zv;
        std::unique_ptr<TTreeReaderValue<float> > theta;
        std::unique_ptr<TTreeReaderValue<float> > trkLen;
        std::unique_ptr<TTreeReaderValue<std::vector<float> > > qdc;
        std::unique_ptr<TTreeReaderValue<std::vector<float> > > tSig;
        std::unique_ptr<TTreeReaderValue<std::vector<int> > > secFlags;
    }DataReaderValue;
    

    typedef struct {
        int flag;
        float Ek;
        float pxv, pyv, pzv;
        float xv, yv, zv;
        float theta;
        float trkLen;
        std::vector<unsigned char> secFlags;
    }Output;
}
#endif


