#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(nullptr),
  fBeamSigma(1.0*cm)
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);

  // Default particle kinematic
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle = particleTable->FindParticle(particleName="proton");
  fParticleGun->SetParticleDefinition(particle);
  
  // Default momentum (will be set via macro)
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
  fParticleGun->SetParticleMomentum(1.0*GeV);
  
  // Position will be randomized around (0, 0, 0)
  fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., 0.));
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // Generate Gaussian-distributed x and y positions
  G4double x0 = G4RandGauss::shoot(0., fBeamSigma);
  G4double y0 = G4RandGauss::shoot(0., fBeamSigma);
  G4double z0 = 0.;
  
  fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
  
  // Parallel beam (no divergence)
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
  
  fParticleGun->GeneratePrimaryVertex(anEvent);
}

void PrimaryGeneratorAction::SetMomentum(G4double momentum)
{
  fParticleGun->SetParticleMomentum(momentum);
}
