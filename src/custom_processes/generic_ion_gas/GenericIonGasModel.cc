#include "custom_processes/generic_ion_gas/GenericIonGasModel.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicsFreeVector.hh"
#include "G4IonStoppingData.hh"
#include "G4VIonDEDXTable.hh"
#include "G4VIonDEDXScalingAlgorithm.hh"
#include "G4IonDEDXScalingICRU73.hh"
#include "G4BraggIonModel.hh"
#include "G4BetheBlochModel.hh"
#include "G4ProductionCutsTable.hh"
#include "G4ParticleChangeForLoss.hh"
#include "G4LossTableManager.hh"
#include "G4EmParameters.hh"
#include "G4GenericIon.hh"
#include "G4Electron.hh"
#include "G4DeltaAngle.hh"
#include "Randomize.hh"
#include "G4Exp.hh"

using namespace std;

GenericIonGasModel::GenericIonGasModel(
    const G4ParticleDefinition*,
    const G4String& nam)
    : G4VEmModel(nam),
    braggIonModel(0),
    betheBlochModel(0),
    nmbBins(90),
    nmbSubBins(100),
    particleChangeLoss(0),
    corrFactor(1.0),
    energyLossLimit(0.01),
    cutEnergies(0),
    isInitialised(false)
{
    genericIon = G4GenericIon::Definition();
    genericIonPDGMass = genericIon->GetPDGMass();
    corrections = G4LossTableManager::Instance()->EmCorrections();

    // The Bragg ion and Bethe Bloch models are instantiated
    braggIonModel = new G4BraggIonModel();
    betheBlochModel = new G4BetheBlochModel();

    // The boundaries for the range tables are set
    lowerEnergyEdgeIntegr = 0.025 * MeV;
    upperEnergyEdgeIntegr = betheBlochModel->HighEnergyLimit();

    // Cache parameters are set
    cacheParticle = nullptr;
    cacheMass = 0;
    cacheElecMassRatio = 0;
    cacheCharge = 0;
    cacheChargeSquare = 0.;
    cacheStrippedRatio = 0.;
    cacheTrackId = -1;
    cacheEvtId = -1;
    cacheRunId = -1;

    // Cache parameters are set
    rangeCacheParticle = nullptr;
    rangeCacheMatCutsCouple = 0;
    rangeCacheEnergyRange = 0;
    rangeCacheRangeEnergy = 0;

    // Cache parameters are set
    dedxCacheParticle = nullptr;
    dedxCacheMaterial = 0;
    dedxCacheEnergyCut = 0;
    dedxCacheIter = lossTableList.end();
    dedxCacheTransitionEnergy = 0.0;
    dedxCacheTransitionFactor = 0.0;
    dedxCacheGenIonMassRatio = 0.0;

    // default generator
    SetAngularDistribution(new G4DeltaAngle());
}

GenericIonGasModel::~GenericIonGasModel()
{
    // dE/dx table objects are deleted and the container is cleared
    LossTableList::iterator iterTables = lossTableList.begin();
    LossTableList::iterator iterTables_end = lossTableList.end();

    for(;iterTables != iterTables_end; ++iterTables) { delete *iterTables; }
    lossTableList.clear();

    // range table
    RangeEnergyTable::iterator itr = r.begin();
    RangeEnergyTable::iterator itr_end = r.end();
    for(;itr != itr_end; ++itr) { delete itr->second; }
    r.clear();

    // inverse range
    EnergyRangeTable::iterator ite = E.begin();
    EnergyRangeTable::iterator ite_end = E.end();
    for(;ite != ite_end; ++ite) { delete ite->second; }
    E.clear();
}

G4double GenericIonGasModel::ChargeSquareRatio(const G4Track &track)
{
    if(CheckNewTrack(track))
        UpdateCache(&track);
    return GetChargeSquareRatio(track.GetDefinition(), track.GetMaterial(), track.GetKineticEnergy());
}

G4double GenericIonGasModel::GetChargeSquareRatio(
    const G4ParticleDefinition* particle,
    const G4Material* material,
    G4double kineticEnergy)
{
    G4double strippedSquareRatio = cacheStrippedRatio*cacheStrippedRatio;
    G4double chargeSquareRatio = strippedSquareRatio*corrections->EffectiveChargeSquareRatio(particle, material, kineticEnergy);
    G4double chargeSquareRatioCorr = chargeSquareRatio*corrections->EffectiveChargeCorrection(particle, material, kineticEnergy);
    if(chargeSquareRatioCorr > 1.0)
        return chargeSquareRatioCorr;
    else
        return 1.;
}

G4double GenericIonGasModel::GetParticleCharge(
    const G4ParticleDefinition* particle,
    const G4Material* material,
    G4double kineticEnergy)
{
    G4double particleCharge = cacheStrippedRatio*corrections->GetParticleCharge(particle, material, kineticEnergy);
    if(particleCharge > 1.)
        return particleCharge;
    else
        return 1.;
}

#include "G4EventManager.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4Run.hh"
G4bool GenericIonGasModel::CheckNewTrack(const G4Track &track)
{ 
    const G4int runId = G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID();
    const G4int evtId = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    const G4int trkId = track.GetTrackID();
    // G4cout << runId << " " << evtId << " " << track.GetTrackID() << " " << cacheEvtId << " " << cacheTrackId << " " << track.GetParticleDefinition()->GetParticleName() << G4endl;
    // if a new run or a new event
    if(runId != cacheRunId || evtId != cacheEvtId || trkId != cacheTrackId)
    {
        cacheRunId = runId;
        cacheEvtId = evtId;
        cacheTrackId = trkId;
        return true;
    }
    return false;
}

G4double GenericIonGasModel::MinEnergyCut(
    const G4ParticleDefinition*,
    const G4MaterialCutsCouple* couple)
{
    return couple->GetMaterial()->GetIonisation()->GetMeanExcitationEnergy();
}


G4double GenericIonGasModel::MaxSecondaryEnergy(
    const G4ParticleDefinition *particle, G4double kineticEnergy)
{
    // If called to prepare physics table before a run, the cache must be updated using particle definition.
    if(!IsRunStarted() || !particle->IsGeneralIon())
        UpdateCache(particle);

    G4double tau = kineticEnergy/cacheMass;
    G4double tmax = 2.0 * electron_mass_c2 * tau * (tau + 2.) /
        (1. + 2.0 * (tau + 1.) * cacheElecMassRatio +
        cacheElecMassRatio * cacheElecMassRatio);
    /*
    G4cerr << "-------------------------------------------------------------" << G4endl;
    G4cerr << particle->GetParticleName() << " " << tau << " " << tmax  << " " << cacheElecMassRatio << " " << cacheParticle << G4endl;
    if(!IsRunStarted())
        G4cerr << "Before Run" << G4endl;
    else
        G4cerr << cacheParticle->GetParticleName() << G4endl;
    */
    return tmax;
}

G4bool GenericIonGasModel::IsRunStarted() const
{
    return G4EventManager::GetEventManager()->GetConstCurrentEvent() != nullptr;
}

// #########################################################################


// #########################################################################

void GenericIonGasModel::Initialise(
    const G4ParticleDefinition* particle,
    const G4DataVector& cuts) {
    // Cached parameters are reset
    cacheParticle = 0;
    cacheMass = 0;
    cacheElecMassRatio = 0;
    cacheCharge = 0;
    cacheChargeSquare = 0.;
    cacheStrippedRatio = 0.;
    cacheTrackId = -1;
    cacheEvtId = -1;

    // Cached parameters are reset
    rangeCacheParticle = 0;
    rangeCacheMatCutsCouple = 0;
    rangeCacheEnergyRange = 0;
    rangeCacheRangeEnergy = 0;

    // Cached parameters are reset
    dedxCacheParticle = 0;
    dedxCacheMaterial = 0;
    dedxCacheEnergyCut = 0;
    dedxCacheIter = lossTableList.end();
    dedxCacheTransitionEnergy = 0.0;
    dedxCacheTransitionFactor = 0.0;
    dedxCacheGenIonMassRatio = 0.0;

    // By default ICRU 73 stopping power tables are loaded:
    if(!isInitialised) {
        G4bool icru90 = G4EmParameters::Instance()->UseICRU90Data();
        isInitialised = true;
        AddDEDXTable("ICRU73",
            new G4IonStoppingData("ion_stopping_data/icru", icru90),
            new G4IonDEDXScalingICRU73());
    }
    // The cache of loss tables is cleared
    LossTableList::iterator iterTables = lossTableList.begin();
    LossTableList::iterator iterTables_end = lossTableList.end();

    for(;iterTables != iterTables_end; ++iterTables) {
        (*iterTables)->ClearCache();
    }

    // Range vs energy and energy vs range vectors from previous runs are
    // cleared
    RangeEnergyTable::iterator iterRange = r.begin();
    RangeEnergyTable::iterator iterRange_end = r.end();

    for(;iterRange != iterRange_end; ++iterRange) {
        delete iterRange->second;
    }
    r.clear();

    EnergyRangeTable::iterator iterEnergy = E.begin();
    EnergyRangeTable::iterator iterEnergy_end = E.end();

    for(;iterEnergy != iterEnergy_end; iterEnergy++) {
        delete iterEnergy->second;
    }
    E.clear();

    // The cut energies 
    cutEnergies = cuts;

    // All dE/dx vectors are built
    const G4ProductionCutsTable* coupleTable =
        G4ProductionCutsTable::GetProductionCutsTable();
    size_t nmbCouples = coupleTable->GetTableSize();

    for(size_t i = 0; i < nmbCouples; ++i)
    {
        const G4MaterialCutsCouple* couple = coupleTable->GetMaterialCutsCouple(i);
        const G4Material* material = couple->GetMaterial();
        for(G4int atomicNumberIon = 3; atomicNumberIon < 102; ++atomicNumberIon)
        {
            LossTableList::iterator iter = lossTableList.begin();
            LossTableList::iterator iter_end = lossTableList.end();
            for(;iter != iter_end; ++iter)
            {
                if(*iter == 0)
                {
                    G4cout << "GenericIonGasModel::Initialise(): Skipping illegal table." << G4endl;
                }
                if((*iter)->BuildDEDXTable(atomicNumberIon, material))
                {
                    break;
                }
            }
        }
    }
    // The particle change object
    if(!particleChangeLoss) {
        particleChangeLoss = GetParticleChangeForLoss();
        braggIonModel->SetParticleChange(particleChangeLoss, 0);
        betheBlochModel->SetParticleChange(particleChangeLoss, 0);
    }

    // The G4braggIonModel and G4betheBlochModel instances are initialised with
    // the same settings as the current model:
    braggIonModel->Initialise(particle, cuts);
    betheBlochModel->Initialise(particle, cuts);
}

// #########################################################################

G4double GenericIonGasModel::ComputeCrossSectionPerAtom(
    const G4ParticleDefinition* particle,
    G4double kineticEnergy, G4double atomicNumber,
    G4double,
    G4double cutEnergy, G4double maxKinEnergy)
{
    G4double crosssection = 0.0;
    G4double tmax = MaxSecondaryEnergy(particle, kineticEnergy);
    G4double maxEnergy = min(tmax, maxKinEnergy);
    if(cutEnergy < tmax) {
        G4double energy = kineticEnergy + cacheMass;
        G4double betaSquared = kineticEnergy*
            (energy + cacheMass)/(energy*energy);
        G4double strippedSquareRatio = cacheStrippedRatio*cacheStrippedRatio;
        crosssection = 1.0/cutEnergy - 1.0/maxEnergy -
            betaSquared*log(maxEnergy/cutEnergy)/tmax;
        crosssection *= twopi_mc2_rcl2*cacheChargeSquare*strippedSquareRatio/betaSquared;
    }

    /*
    G4cout << "########################################################"
        << G4endl
        << "# G4IonParameterizedLossModel::ComputeCrossSectionPerAtom"
        << G4endl
        << "# particle =" << particle->GetParticleName()
        <<  G4endl
        << "# cut(MeV) = " << cutEnergy/MeV
        << G4endl;

    G4cout << "#"
        << std::setw(13) << std::right << "E(MeV)"
        << std::setw(14) << "CS(um)"
        << std::setw(14) << "E_max_sec(MeV)"
        << G4endl
        << "# ------------------------------------------------------"
        << G4endl;

    G4cout << std::setw(14) << std::right << kineticEnergy / MeV
        << std::setw(14) << crosssection / (um * um)
        << std::setw(14) << tmax / MeV
        << G4endl;
    */
    crosssection *= atomicNumber;
    return crosssection;
}

// #########################################################################

G4double GenericIonGasModel::CrossSectionPerVolume(
    const G4Material* material, const G4ParticleDefinition* particle,
    G4double kineticEnergy, G4double cutEnergy, G4double maxEnergy)
{
    G4double nbElecPerVolume = material->GetTotNbOfElectPerVolume();
    G4double cross = ComputeCrossSectionPerAtom(particle,
        kineticEnergy,
        nbElecPerVolume, 0,
        cutEnergy,
        maxEnergy);
    return cross;
}

// #########################################################################

G4double GenericIonGasModel::ComputeDEDXPerVolume(
    const G4Material* material, const G4ParticleDefinition* particle,
    G4double kineticEnergy, G4double cutEnergy)
{
    G4double dEdx = 0.0;
    UpdateDEDXCache(particle, material, cutEnergy);

    LossTableList::iterator iter = dedxCacheIter;

    if(iter != lossTableList.end()) {
        G4double transitionEnergy = dedxCacheTransitionEnergy;
        if(transitionEnergy > kineticEnergy)
        {
            dEdx = (*iter)->GetDEDX(particle, material, kineticEnergy);
            G4double dEdxDeltaRays = DeltaRayMeanEnergyTransferRate(material,
                particle,
                kineticEnergy,
                cutEnergy);
            dEdx -= dEdxDeltaRays;
        }
        else {
            G4double massRatio = dedxCacheGenIonMassRatio;
            G4double chargeSquare = GetChargeSquareRatio(particle, material, kineticEnergy);

            G4double scaledKineticEnergy = kineticEnergy * massRatio;
            G4double scaledTransitionEnergy = transitionEnergy * massRatio;

            G4double lowEnergyLimit = betheBlochModel->LowEnergyLimit();
            if(scaledTransitionEnergy >= lowEnergyLimit) {

                dEdx = betheBlochModel->ComputeDEDXPerVolume(
                    material, genericIon,
                    scaledKineticEnergy, cutEnergy);
                dEdx *= chargeSquare;
                dEdx += corrections->ComputeIonCorrections(particle,
                    material, kineticEnergy);
                G4double factor = 1.0 + dedxCacheTransitionFactor /
                    kineticEnergy;
                dEdx *= factor;
            }
        }
    }
    else {
        G4double massRatio = 1.0;
        G4double chargeSquare = 1.0;
        if(particle != genericIon) {

            chargeSquare = GetChargeSquareRatio(particle, material, kineticEnergy);
            massRatio = genericIonPDGMass / particle->GetPDGMass();
        }
        G4double scaledKineticEnergy = kineticEnergy * massRatio;
        G4double lowEnergyLimit = betheBlochModel->LowEnergyLimit();
        if(scaledKineticEnergy < lowEnergyLimit)
        {
            dEdx = braggIonModel->ComputeDEDXPerVolume(
                material, genericIon,
                scaledKineticEnergy, cutEnergy);
            dEdx *= chargeSquare;
        }
        else {
            G4double dEdxLimitParam = braggIonModel->ComputeDEDXPerVolume(
                material, genericIon,
                lowEnergyLimit, cutEnergy);

            G4double dEdxLimitBetheBloch = betheBlochModel->ComputeDEDXPerVolume(
                material, genericIon,
                lowEnergyLimit, cutEnergy);

            if(particle != genericIon) {
                G4double chargeSquareLowEnergyLimit = GetChargeSquareRatio(particle, material, lowEnergyLimit / massRatio);

                dEdxLimitParam *= chargeSquareLowEnergyLimit;
                dEdxLimitBetheBloch *= chargeSquareLowEnergyLimit;

                dEdxLimitBetheBloch +=
                    corrections->ComputeIonCorrections(particle,
                    material, lowEnergyLimit / massRatio);
            }

            G4double factor = (1.0 + (dEdxLimitParam/dEdxLimitBetheBloch - 1.0)
                * lowEnergyLimit / scaledKineticEnergy);

            dEdx = betheBlochModel->ComputeDEDXPerVolume(
                material, genericIon,
                scaledKineticEnergy, cutEnergy);

            dEdx *= chargeSquare;

            if(particle != genericIon) {
                dEdx += corrections->ComputeIonCorrections(particle,
                    material, kineticEnergy);
            }
            dEdx *= factor;
        }
    }
    if(dEdx < 0.0) dEdx = 0.0;

    return dEdx;
}

// #########################################################################

void GenericIonGasModel::PrintDEDXTable(
    const G4ParticleDefinition* particle,  // Projectile (ion)
    const G4Material* material,  // Absorber material
    G4double lowerBoundary,      // Minimum energy per nucleon
    G4double upperBoundary,      // Maximum energy per nucleon
    G4int numBins,               // Number of bins
    G4bool logScaleEnergy) {     // Logarithmic scaling of energy

    G4double atomicMassNumber = particle->GetAtomicMass();
    G4double materialDensity = material->GetDensity();

    G4cout << "# dE/dx table for " << particle->GetParticleName()
        << " in material " << material->GetName()
        << " of density " << materialDensity / g * cm3
        << " g/cm3"
        << G4endl
        << "# Projectile mass number A1 = " << atomicMassNumber
        << G4endl
        << "# ------------------------------------------------------"
        << G4endl;
    G4cout << "#"
        << setw(13) << right << "E"
        << setw(14) << "E/A1"
        << setw(14) << "dE/dx"
        << setw(14) << "1/rho*dE/dx"
        << G4endl;
    G4cout << "#"
        << setw(13) << right << "(MeV)"
        << setw(14) << "(MeV)"
        << setw(14) << "(MeV/cm)"
        << setw(14) << "(MeV*cm2/mg)"
        << G4endl
        << "# ------------------------------------------------------"
        << G4endl;

    G4double energyLowerBoundary = lowerBoundary * atomicMassNumber;
    G4double energyUpperBoundary = upperBoundary * atomicMassNumber;

    if(logScaleEnergy) {

        energyLowerBoundary = log(energyLowerBoundary);
        energyUpperBoundary = log(energyUpperBoundary);
    }

    G4double deltaEnergy = (energyUpperBoundary - energyLowerBoundary) /
        G4double(nmbBins);

    for(int i = 0; i < numBins + 1; i++) {

        G4double energy = energyLowerBoundary + i * deltaEnergy;
        if(logScaleEnergy) energy = G4Exp(energy);

        G4double dedx = ComputeDEDXPerVolume(material, particle, energy, DBL_MAX);
        G4cout.precision(6);
        G4cout << setw(14) << right << energy / MeV
            << setw(14) << energy / atomicMassNumber / MeV
            << setw(14) << dedx / MeV * cm
            << setw(14) << dedx / materialDensity / (MeV*cm2/(0.001*g))
            << G4endl;
    }
}

// #########################################################################

void GenericIonGasModel::PrintDEDXTableHandlers(
    const G4ParticleDefinition* particle,  // Projectile (ion)
    const G4Material* material,  // Absorber material
    G4double lowerBoundary,      // Minimum energy per nucleon
    G4double upperBoundary,      // Maximum energy per nucleon
    G4int numBins,               // Number of bins
    G4bool logScaleEnergy) {     // Logarithmic scaling of energy

    LossTableList::iterator iter = lossTableList.begin();
    LossTableList::iterator iter_end = lossTableList.end();

    for(;iter != iter_end; iter++) {
        G4bool isApplicable = (*iter)->IsApplicable(particle, material);
        if(isApplicable) {
            (*iter)->PrintDEDXTable(particle, material,
                lowerBoundary, upperBoundary,
                numBins, logScaleEnergy);
            break;
        }
    }
}

// #########################################################################

void GenericIonGasModel::SampleSecondaries(
    vector<G4DynamicParticle*>*secondaries,
    const G4MaterialCutsCouple* couple, const G4DynamicParticle* particle,
    G4double cutKinEnergySec, G4double userMaxKinEnergySec)
{
    G4double rossiMaxKinEnergySec = MaxSecondaryKinEnergy(particle);
    G4double maxKinEnergySec = min(rossiMaxKinEnergySec, userMaxKinEnergySec);

    if(cutKinEnergySec >= maxKinEnergySec)
        return;
    G4double kineticEnergy = particle->GetKineticEnergy();
    G4double energy = kineticEnergy + cacheMass;
    G4double betaSquared = kineticEnergy * (energy + cacheMass)
        / (energy * energy);
    G4double kinEnergySec;
    G4double grej;
    do {
        // Sampling kinetic energy from f(T) (using F(T)):
        G4double xi = G4UniformRand();
        kinEnergySec = cutKinEnergySec * maxKinEnergySec /
            (maxKinEnergySec * (1.0 - xi) + cutKinEnergySec * xi);
        // Deriving the value of the rejection function at the obtained kinetic
        // energy:
        grej = 1.0 - betaSquared * kinEnergySec / rossiMaxKinEnergySec;
        if(grej > 1.0) {
            G4cout << "GenericIonGasModel::SampleSecondary Warning: "
                << "Majorant 1.0 < "
                << grej << " for e= " << kinEnergySec
                << G4endl;
        }

    } while(G4UniformRand() >= grej);

    const G4Material* mat = couple->GetMaterial();
    G4int Z = SelectRandomAtomNumber(mat);

    const G4ParticleDefinition* electron = G4Electron::Electron();
    G4DynamicParticle* delta = new G4DynamicParticle(electron,
        GetAngularDistribution()->SampleDirection(particle, kinEnergySec,
        Z, mat),
        kinEnergySec);

    secondaries->push_back(delta);

    // Change kinematics of primary particle
    G4ThreeVector direction = particle->GetMomentumDirection();
    G4double totalMomentum = sqrt(kineticEnergy*(energy + cacheMass));

    G4ThreeVector finalP = totalMomentum*direction - delta->GetMomentum();
    finalP = finalP.unit();

    kineticEnergy -= kinEnergySec;

    particleChangeLoss->SetProposedKineticEnergy(kineticEnergy);
    particleChangeLoss->SetProposedMomentumDirection(finalP);
}

// #########################################################################

void GenericIonGasModel::UpdateRangeCache(
    const G4ParticleDefinition* particle,
    const G4MaterialCutsCouple* matCutsCouple) {

    // ############## Caching ##################################################
    // If the ion-material-cut combination is covered by any native ion data
    // parameterisation (for low energies), range vectors are computed

    if(particle == rangeCacheParticle &&
        matCutsCouple == rangeCacheMatCutsCouple) {
    }
    else {
        rangeCacheParticle = particle;
        rangeCacheMatCutsCouple = matCutsCouple;

        const G4Material* material = matCutsCouple->GetMaterial();
        LossTableList::iterator iter = IsApplicable(particle, material);

        // If any table is applicable, the transition factor is computed:
        if(iter != lossTableList.end()) {

            // Build range-energy and energy-range vectors if they don't exist
            IonMatCouple ionMatCouple = make_pair(particle, matCutsCouple);
            RangeEnergyTable::iterator iterRange = r.find(ionMatCouple);

            if(iterRange == r.end()) BuildRangeVector(particle, matCutsCouple);

            rangeCacheEnergyRange = E[ionMatCouple];
            rangeCacheRangeEnergy = r[ionMatCouple];
        }
        else {
            rangeCacheEnergyRange = 0;
            rangeCacheRangeEnergy = 0;
        }
    }
}

// #########################################################################

void GenericIonGasModel::UpdateDEDXCache(
    const G4ParticleDefinition* particle,
    const G4Material* material,
    G4double cutEnergy) {

    if(particle == dedxCacheParticle &&
        material == dedxCacheMaterial &&
        cutEnergy == dedxCacheEnergyCut) {
    }
    else {

        dedxCacheParticle = particle;
        dedxCacheMaterial = material;
        dedxCacheEnergyCut = cutEnergy;

        G4double massRatio = genericIonPDGMass / particle->GetPDGMass();
        dedxCacheGenIonMassRatio = massRatio;

        LossTableList::iterator iter = IsApplicable(particle, material);
        dedxCacheIter = iter;

        // If any table is applicable, the transition factor is computed:
        if(iter != lossTableList.end()) {

            // Retrieving the transition energy from the parameterisation table
            G4double transitionEnergy =
                (*iter)->GetUpperEnergyEdge(particle, material);
            dedxCacheTransitionEnergy = transitionEnergy;

            // Computing dE/dx from low-energy parameterisation at
            // transition energy
            G4double dEdxParam = (*iter)->GetDEDX(particle, material,
                transitionEnergy);
            G4double dEdxDeltaRays = DeltaRayMeanEnergyTransferRate(material,
                particle,
                transitionEnergy,
                cutEnergy);
            dEdxParam -= dEdxDeltaRays;

            // Computing dE/dx from Bethe-Bloch formula at transition
            // energy
            G4double transitionChargeSquare = GetChargeSquareRatio(particle, material, transitionEnergy);

            G4double scaledTransitionEnergy = transitionEnergy * massRatio;

            G4double dEdxBetheBloch =
                betheBlochModel->ComputeDEDXPerVolume(
                material, genericIon,
                scaledTransitionEnergy, cutEnergy);
            dEdxBetheBloch *= transitionChargeSquare;

            // Additionally, high order corrections are added
            dEdxBetheBloch +=
                corrections->ComputeIonCorrections(particle,
                material, transitionEnergy);

            // Computing transition factor from both dE/dx values
            dedxCacheTransitionFactor =
                (dEdxParam - dEdxBetheBloch)/dEdxBetheBloch
                * transitionEnergy;
        }
        else {
            dedxCacheParticle = particle;
            dedxCacheMaterial = material;
            dedxCacheEnergyCut = cutEnergy;

            dedxCacheGenIonMassRatio =
                genericIonPDGMass / particle->GetPDGMass();

            dedxCacheTransitionEnergy = 0.0;
            dedxCacheTransitionFactor = 0.0;
        }
    }
}

void GenericIonGasModel::CorrectionsAlongStep(
    const G4MaterialCutsCouple* couple, const G4DynamicParticle* dynamicParticle,
    const G4double& length, G4double& eloss)
{


    const G4ParticleDefinition* particle = dynamicParticle->GetDefinition();
    const G4Material* material = couple->GetMaterial();
    G4double kineticEnergy = dynamicParticle->GetKineticEnergy();

    if(kineticEnergy == eloss) { return; }

    G4double cutEnergy = DBL_MAX;
    size_t cutIndex = couple->GetIndex();
    cutEnergy = cutEnergies[cutIndex];

    UpdateDEDXCache(particle, material, cutEnergy);

    LossTableList::iterator iter = dedxCacheIter;

    // If parameterization for ions is available the electronic energy loss is overwritten
    if(iter != lossTableList.end())
    {
        // The energy loss is calculated using the ComputeDEDXPerVolume function
        // and the step length (it is assumed that dE/dx does not change
        // considerably along the step)
        eloss = length * ComputeDEDXPerVolume(material, particle, kineticEnergy, cutEnergy);
        if(eloss > energyLossLimit * kineticEnergy)
        {
            eloss = ComputeLossForStep(couple, particle, kineticEnergy, length);
        }
    }
    // For all corrections below a kinetic energy between the Pre- and
    // Post-step energy values is used
    G4double energy = kineticEnergy - eloss * 0.5;
    if(energy < 0.0)
        energy = kineticEnergy * 0.5;

    G4double chargeSquareRatio
        = corrections->EffectiveChargeSquareRatio(particle, material, energy)*cacheStrippedRatio*cacheStrippedRatio;

    GetModelOfFluctuations()->SetParticleAndCharge(particle, chargeSquareRatio);

    G4double transitionEnergy = dedxCacheTransitionEnergy;
    if(iter != lossTableList.end() && transitionEnergy < kineticEnergy)
    {
        chargeSquareRatio *= corrections->EffectiveChargeCorrection(particle, material, energy);
        G4double chargeSquareRatioCorr = chargeSquareRatio/corrFactor;
        eloss *= chargeSquareRatioCorr;
    }
    else if(iter == lossTableList.end())
    {
        chargeSquareRatio *= corrections->EffectiveChargeCorrection(particle, material, energy);
        G4double chargeSquareRatioCorr = chargeSquareRatio/corrFactor;
        eloss *= chargeSquareRatioCorr;
    }

    // Ion high order corrections are applied if the current model does not
    // overwrite the energy loss (i.e. when the effective charge approach is
    // used)
    if(iter == lossTableList.end())
    {
        G4double scaledKineticEnergy = kineticEnergy * dedxCacheGenIonMassRatio;
        G4double lowEnergyLimit = betheBlochModel->LowEnergyLimit();
        // Corrections are only applied in the Bethe-Bloch energy region
        if(scaledKineticEnergy > lowEnergyLimit)
            eloss += length*corrections->IonHighOrderCorrections(particle, couple, energy);
    }
}

void GenericIonGasModel::BuildRangeVector(
    const G4ParticleDefinition* particle,
    const G4MaterialCutsCouple* matCutsCouple)
{
    G4double cutEnergy = DBL_MAX;
    size_t cutIndex = matCutsCouple->GetIndex();
    cutEnergy = cutEnergies[cutIndex];

    const G4Material* material = matCutsCouple->GetMaterial();

    G4double massRatio = genericIonPDGMass / particle->GetPDGMass();

    G4double lowerEnergy = lowerEnergyEdgeIntegr / massRatio;
    G4double upperEnergy = upperEnergyEdgeIntegr / massRatio;

    G4double logLowerEnergyEdge = log(lowerEnergy);
    G4double logUpperEnergyEdge = log(upperEnergy);

    G4double logDeltaEnergy = (logUpperEnergyEdge - logLowerEnergyEdge) /
        G4double(nmbBins);

    G4double logDeltaIntegr = logDeltaEnergy / G4double(nmbSubBins);

    G4PhysicsFreeVector* energyRangeVector =
        new G4PhysicsFreeVector(nmbBins+1,
        lowerEnergy,
        upperEnergy,
        /*spline=*/true);

    G4double dedxLow = ComputeDEDXPerVolume(material,
        particle,
        lowerEnergy,
        cutEnergy);

    G4double range = 2.0 * lowerEnergy / dedxLow;

    energyRangeVector->PutValues(0, lowerEnergy, range);

    G4double logEnergy = log(lowerEnergy);
    for(size_t i = 1; i < nmbBins+1; i++)
    {
        G4double logEnergyIntegr = logEnergy;
        for(size_t j = 0; j < nmbSubBins; j++)
        {
            G4double binLowerBoundary = G4Exp(logEnergyIntegr);
            logEnergyIntegr += logDeltaIntegr;
            G4double binUpperBoundary = G4Exp(logEnergyIntegr);
            G4double deltaIntegr = binUpperBoundary - binLowerBoundary;
            G4double energyIntegr = binLowerBoundary + 0.5 * deltaIntegr;
            G4double dedxValue = ComputeDEDXPerVolume(material,
                particle,
                energyIntegr,
                cutEnergy);
            if(dedxValue > 0.0)
                range += deltaIntegr / dedxValue;
        }
        logEnergy += logDeltaEnergy;
        G4double energy = G4Exp(logEnergy);
        energyRangeVector->PutValues(i, energy, range);
    }
    //vector is filled: activate spline
    energyRangeVector->FillSecondDerivatives();

    G4double lowerRangeEdge =
        energyRangeVector->Value(lowerEnergy);
    G4double upperRangeEdge =
        energyRangeVector->Value(upperEnergy);

    G4PhysicsFreeVector* rangeEnergyVector
        = new G4PhysicsFreeVector(nmbBins+1,
        lowerRangeEdge,
        upperRangeEdge,
        /*spline=*/true);

    for(size_t i = 0; i < nmbBins+1; i++)
    {
        G4double energy = energyRangeVector->Energy(i);
        rangeEnergyVector ->
            PutValues(i, energyRangeVector->Value(energy), energy);
    }

    rangeEnergyVector->FillSecondDerivatives();

    IonMatCouple ionMatCouple = make_pair(particle, matCutsCouple);

    E[ionMatCouple] = energyRangeVector;
    r[ionMatCouple] = rangeEnergyVector;
}

// #########################################################################

G4double GenericIonGasModel::ComputeLossForStep(
    const G4MaterialCutsCouple* matCutsCouple, const G4ParticleDefinition* particle,
    G4double kineticEnergy, G4double stepLength)
{

    G4double loss = 0.0;

    UpdateRangeCache(particle, matCutsCouple);

    G4PhysicsVector* energyRange = rangeCacheEnergyRange;
    G4PhysicsVector* rangeEnergy = rangeCacheRangeEnergy;

    if(energyRange != 0 && rangeEnergy != 0)
    {
        G4double lowerEnEdge = energyRange->Energy(0);
        G4double lowerRangeEdge = rangeEnergy->Energy(0);

        // Computing range for pre-step kinetic energy:
        G4double range = energyRange->Value(kineticEnergy);

        // Energy below vector boundary:
        if(kineticEnergy < lowerEnEdge)
        {
            range = energyRange->Value(lowerEnEdge);
            range *= sqrt(kineticEnergy / lowerEnEdge);
        }
        G4double remRange = range - stepLength;
        if(remRange < 0.0)
            loss = kineticEnergy;
        else if(remRange < lowerRangeEdge)
        {
            G4double ratio = remRange / lowerRangeEdge;
            loss = kineticEnergy - ratio * ratio * lowerEnEdge;
        }
        else
        {
            G4double energy = rangeEnergy->Value(range - stepLength);
            loss = kineticEnergy - energy;
        }
    }
    if(loss < 0.0) loss = 0.0;
    return loss;
}

// #########################################################################

G4bool GenericIonGasModel::AddDEDXTable(const G4String& nam, G4VIonDEDXTable* table, G4VIonDEDXScalingAlgorithm* algorithm)
{
    if(table == 0) {
        G4cout << "GenericIonGasModel::AddDEDXTable() Cannot "
            << " add table: Invalid pointer."
            << G4endl;

        return false;
    }
    // Checking uniqueness of name
    LossTableList::iterator iter = lossTableList.begin();
    LossTableList::iterator iter_end = lossTableList.end();
    for(;iter != iter_end; ++iter) {
        const G4String& tableName = (*iter)->GetName();
        if(tableName == nam) {
            G4cout << "GenericIonGasModel::AddDEDXTable() Cannot "
                << " add table: Name already exists."
                << G4endl;
            return false;
        }
    }

    G4VIonDEDXScalingAlgorithm* scalingAlgorithm = algorithm;
    if(scalingAlgorithm == 0)
        scalingAlgorithm = new G4VIonDEDXScalingAlgorithm;

    G4IonDEDXHandler* handler = new G4IonDEDXHandler(table, scalingAlgorithm, nam);
    lossTableList.push_front(handler);
    return true;
}

G4bool GenericIonGasModel::RemoveDEDXTable(const G4String& nam)
{
    LossTableList::iterator iter = lossTableList.begin();
    LossTableList::iterator iter_end = lossTableList.end();
    for(;iter != iter_end; iter++)
    {
        G4String tableName = (*iter)->GetName();
        if(tableName == nam)
        {
            delete (*iter);
            // Remove from table list
            lossTableList.erase(iter);
            // Range vs energy and energy vs range vectors are cleared
            RangeEnergyTable::iterator iterRange = r.begin();
            RangeEnergyTable::iterator iterRange_end = r.end();
            for(;iterRange != iterRange_end; iterRange++)
                delete iterRange->second;
            r.clear();
            EnergyRangeTable::iterator iterEnergy = E.begin();
            EnergyRangeTable::iterator iterEnergy_end = E.end();
            for(;iterEnergy != iterEnergy_end; iterEnergy++)
                delete iterEnergy->second;
            E.clear();
            return true;
        }
    }

    return false;
}

inline G4double GenericIonGasModel::DeltaRayMeanEnergyTransferRate(
    const G4Material* material, const G4ParticleDefinition* particle,
    G4double kineticEnergy, G4double cutEnergy)
{
    G4double meanRate = 0.0;
    G4double maxKinEnergy = MaxSecondaryEnergy(particle, kineticEnergy);

    if(cutEnergy < maxKinEnergy)
    {
        G4double totalEnergy = kineticEnergy + cacheMass;
        G4double betaSquared = kineticEnergy*(totalEnergy + cacheMass)/(totalEnergy*totalEnergy);
        G4double cutMaxEnergyRatio = cutEnergy / maxKinEnergy;
        meanRate =
            (-log(cutMaxEnergyRatio) - (1.0 - cutMaxEnergyRatio) * betaSquared) *
            CLHEP::twopi_mc2_rcl2 *
            (material->GetTotNbOfElectPerVolume()) / betaSquared;
        meanRate *= GetChargeSquareRatio(particle, material, kineticEnergy);
    }
    return meanRate;
}

void GenericIonGasModel::UpdateCache(const G4Track *track)
{
    const auto dParticle = track->GetDynamicParticle();
    const auto particle = track->GetParticleDefinition();
    cacheTrackId = track->GetTrackID();
    cacheParticle = track->GetDynamicParticle()->GetParticleDefinition();

    cacheMass = cacheParticle->GetPDGMass();

    cacheElecMassRatio = CLHEP::electron_mass_c2/cacheMass;

    cacheStrippedRatio = dParticle->GetCharge()/particle->GetPDGCharge();
    cacheCharge = particle->GetPDGCharge();
    cacheChargeSquare = cacheCharge*cacheCharge;
}

void GenericIonGasModel::UpdateCache(const G4ParticleDefinition* particle)
{
    cacheParticle = particle;
    cacheMass = cacheParticle->GetPDGMass();
    cacheElecMassRatio = CLHEP::electron_mass_c2/cacheMass;
    cacheStrippedRatio = 1.;
    cacheCharge = particle->GetPDGCharge();
    cacheChargeSquare = cacheCharge*cacheCharge;
}

LossTableList::iterator GenericIonGasModel::IsApplicable(
    const G4ParticleDefinition* particle,   // Projectile (ion) 
    const G4Material* material)             // Target material
{
    LossTableList::iterator iter = lossTableList.end();
    LossTableList::iterator iterTables = lossTableList.begin();
    LossTableList::iterator iterTables_end = lossTableList.end();

    for(;iterTables != iterTables_end; iterTables++)
    {
        G4bool isApplicable = (*iterTables)->IsApplicable(particle, material);
        if(isApplicable)
        {
            iter = iterTables;
            break;
        }
    }
    return iter;
}

void GenericIonGasModel::SetEnergyLossLimit(G4double ionEnergyLossLimit)
{
    if(ionEnergyLossLimit > 0 && ionEnergyLossLimit <= 1)
        energyLossLimit = ionEnergyLossLimit;
}