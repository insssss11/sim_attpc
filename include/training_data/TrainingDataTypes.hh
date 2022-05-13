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

    inline EParticle FromParticleName(const std::string &parName)
    {
        if(parName == "gamma")
            return Gamma;
        else if(parName == "e-" || parName == "eletron")
            return Electron;
        else if(parName == "proton")
            return Proton;
        else if(parName == "alpha" || parName == "He" || parName == "alpha")
            return Alpha;
        else if(parName == "C12" || parName == "c12" || parName == "carbon")
            return Carbon;
        else if(parName == "O16" || parName == "o16" || parName == "oxygen")
            return Oxygen;
        else
            return Unknown;
    }
}
#endif


