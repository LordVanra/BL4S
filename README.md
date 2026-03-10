# Proton Transmission Simulation

This repository contains a full Monte Carlo particle physics simulation built with **Geant4**.

## Installation Prerequisites

To run this code, you need two separate environments: a **C++ environment for Geant4** and a **Python environment for data analysis**.

### 1. C++ & Geant4 Installation (Windows)

This simulation is written in modern C++ and relies heavily on the **Geant4 Toolkit**.

1. **Install CMake**: Download and install CMake (ensure it's added to your system PATH).
2. **Install Visual Studio 2022**: Install the desktop development with C++ workload.
3. **Install Geant4**: 
   - Download the pre-compiled Geant4 libraries for Windows from the [CERN Geant4 website](https://geant4.web.cern.ch/download).
   - Alternatively, build Geant4 from source using CMake (ensure you enable the dataset installation and OpenGL Qt/Win32 UI components during CMake generation).
4. **Environment Variables**: You must source the Geant4 environment variables before compiling or running the code. Usually, this means running the provided `geant4.bat` script in your terminal (e.g., `call "C:\path\to\geant4-install\bin\geant4.bat"`).

### 2. Python Packages Installation

The automated `run_analysis.py` script requires Python 3 and a few standard scientific libraries. It is highly recommended to use a virtual environment (`venv`).

```powershell
# Create a virtual environment
python -m venv venv

# Activate the virtual environment (Windows)
.\venv\Scripts\Activate.ps1

# Install required numerical and plotting libraries
pip install numpy matplotlib
```

---

## How to Run

### Interactive Visualization Mode
To view the 3D OpenGL visualization of the geometry and watch a few particles curve through the magnetic field and shower in the target:

1. Ensure your Geant4 environment is activated (`call geant4.bat`).
2. Run the build script to compile the project:
   ```powershell
   .\build.bat
   ```
3. The build script automatically launches `protonTransmission.exe`, which executes `vis.mac` and opens the interactive viewer. 

### Automated 40-Run Analysis Mode
To perform the full statistical study (100,000 protons per configuration) across all 40 setups and generate the PDF plots:

1. Ensure the project is already compiled (you should have run `build.bat` at least once).
2. Ensure you are in your Python virtual environment (so `numpy` and `matplotlib` are available).
3. Run the analysis script:
   ```powershell
   python run_analysis.py
   ```
4. The script will take roughly 10-20 minutes to complete. Once finished, it will output a raw data table (`transmission_results_table.txt`) and a multi-page PDF with graphs comparing the beam transmission and loss (`transmission_analysis_report.pdf`).
