# Proton Transmission Simulation

A GEANT4 simulation to measure proton transmission through magnetic fields and material slabs, testing whether the combined effect equals the product of individual effects.

## Project Overview

This simulation fires protons at a target and measures transmission under three conditions:
1. **Material only**: Protons pass through polyethylene (magnet off)
2. **Magnet only**: Protons are deflected by 1T magnetic field (no material)
3. **Hybrid**: Both magnetic field and material present

The goal is to compute: **Δ = T_hybrid / (T_magnet × T_material) - 1**

**Build**
```cmd
build.bat
```

## Running the Simulation

### Interactive Mode (with visualization)

```bash
cd build
./protonTransmission
```

This will:
- Open a 3D visualization window
- Show the geometry (magnet, slab, scoring plane)
- Allow you to fire particles and see their trajectories

### Batch Mode (single run)

```bash
cd build
./protonTransmission run.mac
```

### Full Automated Analysis

```bash
cd build
python3 ../run_analysis.py
```

This will:
- Run all 15 configurations (3 setups × 5 momenta)
- Generate 100,000 events per configuration
- Calculate transmission coefficients
- Compute deviations
- Generate analysis plots

**Note:** The full analysis takes several hours to complete.

## Simulation Details

### Geometry

The simulation consists of:

| Component | Position | Size | Details |
|-----------|----------|------|---------|
| Proton Gun | z = 0 | - | Fires along +z axis |
| Magnetic Field | z = 0.5m to 1.5m | 30cm × 30cm × 1m | 1.0 Tesla in y-direction |
| Material Slab | z = 2.0m to 2.1m | 1m × 1m × 10cm | HDPE (0.94 g/cm³) |
| Scoring Plane | z = 3.5m | 2m × 2m × 2mm | Counts transmitted protons |

### Beam Parameters

- **Particle**: Proton
- **Spatial distribution**: Gaussian (σ = 1 cm in x and y)
- **Angular distribution**: Parallel beam (zero divergence)
- **Momenta**: 0.5, 1.0, 2.0, 3.0, 5.0 GeV/c
- **Events per run**: 100,000

### Physics List

**FTFP_BERT** - CERN's recommended physics list for GeV-range hadron interactions.

## Output and Analysis

### What Gets Measured

At the scoring plane (z = 3.5m), for each proton that arrives:
- Count (for transmission calculation)
- X position (cm)
- Y position (cm)
- Kinetic energy (GeV)

### Expected Results

**T_material** (increases with momentum):
- ~0.30 at 0.5 GeV/c → ~0.60 at 5 GeV/c
- Higher energy protons interact less with nuclear forces

**T_magnet** (decreases with momentum):
- ~0.20 at 0.5 GeV/c → ~0.90 at 5 GeV/c
- Lower momentum particles are deflected more

**Δ** (deviation from independence):
- Should be small and positive (a few percent)
- Positive due to oblique path length effect

### Output Files

After running the analysis, you'll get:
- `transmission_analysis.png` - Two plots showing:
  1. Transmission curves vs momentum
  2. Deviation (Δ) vs momentum with error bars

## Modifying the Simulation

### Change Magnetic Field Strength

Edit `src/DetectorConstruction.cc`, line ~90:
```cpp
fMagneticField = new G4UniformMagField(G4ThreeVector(0., 2.0*tesla, 0.));
```

### Change Material Slab Thickness

Edit `src/DetectorConstruction.cc`, line ~115:
```cpp
G4Box* solidSlab = new G4Box("Slab", 0.5*m, 0.5*m, 0.20*m);  // 40 cm thick
```

### Change Beam Spread

Edit `src/PrimaryGeneratorAction.cc`, line ~17:
```cpp
fBeamSigma(2.0*cm)  // Change from 1 cm to 2 cm
```

### Change Number of Events

Edit `run_analysis.py`, line ~13:
```python
N_EVENTS = 200000  # Double the statistics
```

## Configuration Management

To easily switch between the three run configurations, you can:

1. **Manually modify DetectorConstruction.cc** before building
2. **Use macro commands** (if implemented)
3. **Use the Python script** which automatically handles configurations

The Python script (`run_analysis.py`) is designed to orchestrate all runs, but you'll need to implement configuration switching in the GEANT4 code or via messenger commands.