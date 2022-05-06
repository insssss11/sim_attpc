#ifndef GenericIonGasModel_HH
#define GenericIonGasModel_HH

#include <iomanip>
#include <list>
#include <map>
#include <utility>
#include <CLHEP/Units/PhysicalConstants.h>

#include "G4VEmModel.hh"
#include "G4EmCorrections.hh"
#include "G4IonDEDXHandler.hh"

class G4BraggIonModel;
class G4BetheBlochModel;
class G4ParticleChangeForLoss;
class G4VIonDEDXTable;
class G4VIonDEDXScalingAlgorithm;
class G4PhysicsFreeVector;
class G4MaterialCutsCouple;

typedef std::list<G4IonDEDXHandler*> LossTableList;
typedef std::pair<const G4ParticleDefinition*,
    const G4MaterialCutsCouple*> IonMatCouple;

// Customized energy loss model by ionization
// 

class GenericIonGasModel : public G4VEmModel {

    public:
    explicit GenericIonGasModel(const G4ParticleDefinition* particle = nullptr,
        const G4String& name = "GenericIonGasModel");

    virtual ~GenericIonGasModel();

    // returns charge square ratio and if new track, update cachCharge
    G4double ChargeSquareRatio(const G4Track &) override;

    G4double GetChargeSquareRatio(
        const G4ParticleDefinition*, // Projectile
        const G4Material*,  // Target Material
        G4double) override; // Kinetic energy of projectile

    G4double GetParticleCharge(
        const G4ParticleDefinition*, // Projectile
        const G4Material*,  // Target Material
        G4double) override; // Kinetic energy of projectile 

    void Initialise(
        const G4ParticleDefinition*, // Projectile
        const G4DataVector&) override; // Cut energies

    G4double MinEnergyCut(
        const G4ParticleDefinition*,  // Projectile
        const G4MaterialCutsCouple*) override;

    G4double ComputeCrossSectionPerAtom(
        const G4ParticleDefinition*, // Projectile
        G4double,  // Kinetic energy of projectile
        G4double,  // Atomic number
        G4double,  // Mass number
        G4double,  // Energy cut for secondary prod.
        G4double) override; // Maximum energy of secondaries

    G4double CrossSectionPerVolume(
        const G4Material*,  // Target material
        const G4ParticleDefinition*, // Projectile
        G4double,  // Kinetic energy
        G4double,  // Energy cut for secondary prod.
        G4double) override; // Maximum energy of secondaries

    G4double ComputeDEDXPerVolume(
        const G4Material*, // Target material
        const G4ParticleDefinition*, // Projectile
        G4double,  // Kinetic energy of projectile
        G4double) override; // Energy cut for secondary prod.

    // Function, which computes the continuous energy loss (due to electronic
    // stopping) for a given pre-step energy and step length by using
    // range vs energy (and energy vs range) tables  
    G4double ComputeLossForStep(
        const G4MaterialCutsCouple*, // Mat-cuts couple
        const G4ParticleDefinition*, // Projectile
        G4double,  // Kinetic energy of projectile
        G4double); // Length of current step

    // Function, which computes the mean energy transfer rate to delta rays 
    inline G4double DeltaRayMeanEnergyTransferRate(
        const G4Material*, // Target Material
        const G4ParticleDefinition*, // Projectile
        G4double,  // Kinetic energy of projectile
        G4double); // Energy cut for secondary prod.


    void SampleSecondaries(std::vector<G4DynamicParticle*>*,
        const G4MaterialCutsCouple*,
        const G4DynamicParticle*,
        G4double,  // Energy cut for secondary prod.
        G4double) override; // Maximum energy of secondaries

    void CorrectionsAlongStep(
        const G4MaterialCutsCouple*,// Mat.-Cut couple
        const G4DynamicParticle*,   // Dyn. particle
        const G4double&,            // Length of current step
        G4double&) override;        // Energy loss in current step

    // Function which allows to add additional stopping power tables
    // in combination with a scaling algorithm, which may depend on dynamic
    // information like the current particle energy (the table and scaling 
    // algorithm are used via a handler class, which performs e.g.caching or
    // which applies the scaling of energy and dE/dx values)
    G4bool AddDEDXTable(const G4String& name,
        G4VIonDEDXTable* table,
        G4VIonDEDXScalingAlgorithm* algorithm = nullptr);

    G4bool RemoveDEDXTable(const G4String& name);

    // Function which allows to switch off scaling of stopping powers of heavy
    // ions from existing ICRU 73 data
    // void DeactivateICRU73Scaling();

    // Function checking the applicability of physics tables to ion-material
    // combinations (Note: the energy range of tables is not checked)
    inline LossTableList::iterator IsApplicable(
        const G4ParticleDefinition*,  // Projectile (ion) 
        const G4Material*);           // Target material

    // Function printing a dE/dx table for a given ion-material combination
    // and a specified energy grid 
    void PrintDEDXTable(
        const G4ParticleDefinition*,  // Projectile (ion) 
        const G4Material*, // Absorber material
        G4double,          // Minimum energy per nucleon
        G4double,          // Maximum energy per nucleon
        G4int,             // Number of bins
        G4bool);           // Logarithmic scaling of energy

    // Function printing a dE/dx table for a given ion-material combination
    // and a specified energy grid 
    void PrintDEDXTableHandlers(
        const G4ParticleDefinition*,  // Projectile (ion) 
        const G4Material*, // Absorber material
        G4double,          // Minimum energy per nucleon
        G4double,          // Maximum energy per nucleon
        G4int,             // Number of bins
        G4bool);           // Logarithmic scaling of energy

    // Function for setting energy loss limit for stopping power integration
    inline void SetEnergyLossLimit(G4double ionEnergyLossLimit);

    protected:
    G4double MaxSecondaryEnergy(const G4ParticleDefinition*,
        G4double) override;   // Kinetic energy of projectile

    private:
    G4bool CheckNewTrack(const G4Track &track);
    G4bool IsRunStarted() const;
    // Function which updates parameters concerning the dE/dx calculation
    // (the parameters are only updated if the particle, the material or 
    // the associated energy cut has changed)
    void UpdateDEDXCache(
        const G4ParticleDefinition*,   // Projectile (ion) 
        const G4Material*,             // Target material
        G4double cutEnergy);           // Energy cut

    // Function which updates parameters concerning the range calculation
    // (the parameters are only updated if the particle, the material or 
    // the associated energy cut has changed)
    void UpdateRangeCache(
        const G4ParticleDefinition*,   // Projectile (ion) 
        const G4MaterialCutsCouple*);  // Target material

    // Function, which updates parameters concering particle properties
    inline void UpdateCache(const G4Track*);    // update cache with dynamic particle(can be partially stripped)
    inline void UpdateCache(const G4ParticleDefinition*);  // update cache with particle definition(only fully stripped)

    // Function, which builds range vs energy (and energy vs range) vectors
    // for a given particle, material and energy cut   
    void BuildRangeVector(
        const G4ParticleDefinition*,   // Projectile (ion) 
        const G4MaterialCutsCouple*);  // Material cuts couple

    // Assignment operator and copy constructor are hidden:
    GenericIonGasModel & operator=(
        const GenericIonGasModel &right);
    GenericIonGasModel(const GenericIonGasModel &);

    // ######################################################################
    // # Models and dE/dx tables for computing the energy loss 
    // # 
    // ######################################################################

    // G4BraggModel and G4BetheBlochModel are used for ion-target
    // combinations and/or projectile energies not covered by parametrisations
    // adopted by this model:
    G4BraggIonModel* braggIonModel;
    G4BetheBlochModel* betheBlochModel;

    // List of dE/dx tables plugged into the model
    LossTableList lossTableList;

    // ######################################################################
    // # Maps of Range vs Energy and Energy vs Range vectors
    // # 
    // ######################################################################

    typedef std::map<IonMatCouple, G4PhysicsFreeVector*> RangeEnergyTable;
    RangeEnergyTable r;

    typedef std::map<IonMatCouple, G4PhysicsFreeVector*> EnergyRangeTable;
    EnergyRangeTable E;

    // ######################################################################
    // # Energy grid definitions (e.g. used for computing range-energy 
    // # tables)
    // ######################################################################

    G4double lowerEnergyEdgeIntegr;
    G4double upperEnergyEdgeIntegr;

    size_t nmbBins;
    size_t nmbSubBins;

    // ######################################################################
    // # Particle change for loss
    // # 
    // ######################################################################

    // Pointer to particle change object, which is used to set e.g. the
    // energy loss and secondary delta-electron
    // used indicating if model is initialized  
    G4ParticleChangeForLoss* particleChangeLoss;

    // ######################################################################
    // # Corrections and energy loss limit
    // # 
    // ######################################################################

    // Pointer to an G4EmCorrections object, which is used to compute the
    // effective ion charge, and other corrections (like high order corrections
    // to stopping powers) 
    G4EmCorrections* corrections;

    // Corrections factor for effective charge, computed for each particle
    // step
    G4double corrFactor;

    // Parameter indicating the maximal fraction of kinetic energy, which
    // a particle may loose along a step, in order that the simple relation
    // (dE/dx)*l can still be applied to compute the energy loss (l = step 
    // length)
    G4double energyLossLimit;

    // ######################################################################
    // # Cut energies and properties of generic ion
    // # 
    // ######################################################################

    // Vector containing the current cut energies (the vector index matches
    // the material-cuts couple index):
    G4DataVector cutEnergies;

    // Pointer to generic ion and mass of generic ion
    const G4ParticleDefinition* genericIon;
    G4double genericIonPDGMass;

    // ######################################################################
    // # "Most-recently-used" cache parameters
    // #
    // ######################################################################

    // Cached key (particle) and value information for a faster 
    // access of particle-related information
    const G4ParticleDefinition* cacheParticle; // Key: Current projectile
    G4double cacheMass;                        // Projectile mass
    G4double cacheElecMassRatio;               // Electron-mass ratio
    // actual charge : cacheStrippedRatio*cacheCharge
    G4double cacheStrippedRatio;               // Charge/FullCharge
    G4double cacheCharge;                      // Charge of the fully scripped
    G4double cacheChargeSquare;                // Charge squared
    G4int cacheTrackId;                          
    G4int cacheEvtId;

    // Cached parameters needed during range computations:
    const G4ParticleDefinition* rangeCacheParticle; // Key: 1) Current ion,
    const G4MaterialCutsCouple* rangeCacheMatCutsCouple; // 2) Mat-cuts-couple
    G4PhysicsVector* rangeCacheEnergyRange;         // Energy vs range vector
    G4PhysicsVector* rangeCacheRangeEnergy;         // Range vs energy vector

    // Cached parameters needed during dE/dx computations:
    const G4ParticleDefinition* dedxCacheParticle; // Key: 1) Current ion,
    const G4Material* dedxCacheMaterial;           //      2) material and
    G4double dedxCacheEnergyCut;                   //      3) cut energy 
    LossTableList::iterator dedxCacheIter;         // Responsible dE/dx table
    G4double dedxCacheTransitionEnergy;      // Transition energy between
    // parameterization and 
    // Bethe-Bloch model
    G4double dedxCacheTransitionFactor;      // Factor for smoothing the dE/dx
    // values in the transition region
    G4double dedxCacheGenIonMassRatio;       // Ratio of generic ion mass       
    // and current particle mass
    G4bool isInitialised;
};

#endif
