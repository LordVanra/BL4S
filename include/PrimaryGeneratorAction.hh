#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event* event);

    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
    
    void SetMomentum(G4double momentum);

  private:
    G4ParticleGun* fParticleGun;
    G4double fBeamSigma;  // Gaussian spread in x and y (1 cm)
};

#endif
