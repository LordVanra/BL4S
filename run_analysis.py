#!/usr/bin/env python3
"""
Run all configurations of the proton transmission simulation
and analyze the results. (40 runs: 5 momenta * 4 obstacle states * 2 materials)
"""

import subprocess
import os
import sys
import re
import numpy as np
import matplotlib.pyplot as plt

# Configuration
MOMENTA = [0.5, 1.0, 2.0, 3.0, 5.0]  # GeV/c
N_EVENTS = 100000
MATERIALS = ["Aluminum6061", "Polycarbonate"]
OBSTACLES = [
    ("ff", False, False),
    ("ft", False, True),
    ("tf", True, False),
    ("tt", True, True)
]

# Results storage
results = {mat: {obs: {'T': [], 'T_err': []} for obs, _, _ in OBSTACLES} for mat in MATERIALS}

def create_macro(momentum, config_name, magnet_on, slab_on, material):
    """Create a macro file for a specific configuration"""
    # Initialize the setup, then configure detector parameters
    macro_content = f"""# Generated macro for {config_name}_mat{material} at {momentum} GeV/c
/detector/setMagneticField {"true" if magnet_on else "false"}
/detector/setMaterialSlab {"true" if slab_on else "false"}
/detector/setSlabMaterial {material}
/run/initialize
/gun/momentum {momentum} GeV
/run/beamOn {N_EVENTS}
"""
    
    filename = f"run_{config_name}_mat{material}_{momentum}GeV.mac"
    with open(filename, 'w') as f:
        f.write(macro_content)
    
    return filename

def run_simulation(macro_file):
    """Run GEANT4 simulation with given macro"""
    print(f"  -> Running simulation with {macro_file}...")
    try:
        # Determine executable path depending on OS / build directory
        exe = './protonTransmission'
        if os.path.exists('./build/Release/protonTransmission.exe'):
            exe = './build/Release/protonTransmission.exe'
        elif os.path.exists('./build/protonTransmission.exe'):
            exe = './build/protonTransmission.exe'
        elif os.path.exists('protonTransmission.exe'):
            exe = 'protonTransmission.exe'
            
        result = subprocess.run(
            [exe, macro_file],
            capture_output=True,
            text=True,
            timeout=1200  # 20 min timeout
        )
        
        hits = parse_hits_from_output(result.stdout)
        if hits == -1:
            print("     WARNING: Could not parse hits, assuming 0.")
            return 0
        return hits
    except Exception as e:
        print(f"     ERROR running simulation: {e}")
        return 0

def parse_hits_from_output(output):
    """Parse the number of hits from GEANT4 output"""
    match = re.search(r'Number of protons at scoring plane\s*:\s*(\d+)', output)
    if match:
        return int(match.group(1))
    return -1

def calculate_transmission(hits, n_events):
    """Calculate transmission and error"""
    T = hits / n_events
    T_err = np.sqrt(hits) / n_events
    return T, T_err

def main():
    print(f"Proton Transmission Simulation - 40 Configurations Run Script")
    print(f"Events per run: {N_EVENTS}")
    
    # Check if executable exists
    exe_found = any(os.path.exists(p) for p in [
        './protonTransmission', './build/Release/protonTransmission.exe', 
        './build/protonTransmission.exe', 'protonTransmission.exe'
    ])
    if not exe_found:
        print("ERROR: protonTransmission executable not found! Make sure you've built it first.")
        sys.exit(1)
    
    # Run all configurations
    for material in MATERIALS:
        print(f"\\n=============================================")
        print(f"MATERIAL: {material}")
        print(f"=============================================")
        for obs_name, magnet_on, slab_on in OBSTACLES:
            print(f"\\n--- Obstacle Config: {obs_name.upper()} (Magnet:{magnet_on}, Slab:{slab_on}) ---")
            for momentum in MOMENTA:
                macro_file = create_macro(momentum, obs_name, magnet_on, slab_on, material)
                hits = run_simulation(macro_file)
                T, T_err = calculate_transmission(hits, N_EVENTS)
                
                results[material][obs_name]['T'].append(T)
                results[material][obs_name]['T_err'].append(T_err)
                print(f"     Momentum {momentum:>4} GeV/c -> Hits: {hits:>6} | T = {T:.4f} \u00B1 {T_err:.4f}")

    # Generate plots
    generate_plots(results)
    print("\\nAnalysis complete! Check transmission_40runs_analysis.png and transmission_results_table.txt")

import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

def generate_plots(results):
    """Generate analysis plots and save all to a single PDF"""
    
    colors = {'ff': 'black', 'ft': 'blue', 'tf': 'red', 'tt': 'purple'}
    markers = {'ff': 'o', 'ft': 's', 'tf': '^', 'tt': 'D'}
    labels_map = {
        'ff': 'FF (No Magnet, No Slab)', 
        'ft': 'FT (Slab Only)', 
        'tf': 'TF (Magnet Only)', 
        'tt': 'TT (Magnet + Slab)'
    }

    print("\\nGenerating PDF report...")
    with PdfPages('transmission_analysis_report.pdf') as pdf:
        
        # Plot 1: Standard transmission comparison per material (Side-by-side)
        fig, axes = plt.subplots(1, 2, figsize=(16, 6))
        for idx, material in enumerate(MATERIALS):
            ax = axes[idx]
            for obs_name, _, _ in OBSTACLES:
                t_vals = results[material][obs_name]['T']
                t_errs = results[material][obs_name]['T_err']
                ax.errorbar(MOMENTA, t_vals, yerr=t_errs,
                            marker=markers[obs_name], color=colors[obs_name], 
                            label=labels_map[obs_name], capsize=5, linestyle='-')
                
            ax.set_xlabel('Momentum (GeV/c)', fontsize=12)
            ax.set_ylabel('Transmission', fontsize=12)
            ax.set_title(f'Transmission vs Momentum ({material})', fontsize=14, fontweight='bold')
            ax.legend()
            ax.grid(True, alpha=0.3)
            ax.set_ylim(-0.05, 1.05)
        plt.tight_layout()
        pdf.savefig(fig)
        plt.close()

        # Plot 2: Material comparison (Aluminum vs Polycarbonate) for TT configuration
        fig, ax = plt.subplots(figsize=(10, 6))
        
        ax.errorbar(MOMENTA, results['Aluminum6061']['tt']['T'], yerr=results['Aluminum6061']['tt']['T_err'],
                    marker='o', color='blue', label='Aluminum 6061 (TT)', capsize=5, linestyle='-')
        ax.errorbar(MOMENTA, results['Polycarbonate']['tt']['T'], yerr=results['Polycarbonate']['tt']['T_err'],
                    marker='s', color='orange', label='Polycarbonate (TT)', capsize=5, linestyle='-')
        
        ax.set_xlabel('Momentum (GeV/c)', fontsize=12)
        ax.set_ylabel('Transmission', fontsize=12)
        ax.set_title('Material Comparison: Full Setup (Magnet + Slab)', fontsize=14, fontweight='bold')
        ax.legend()
        ax.grid(True, alpha=0.3)
        ax.set_ylim(-0.05, 1.05)
        plt.tight_layout()
        pdf.savefig(fig)
        plt.close()

        # Plot 3: Fractional Loss Comparison (How much is lost relative to FF Baseline)
        fig, axes = plt.subplots(1, 2, figsize=(16, 6))
        for idx, material in enumerate(MATERIALS):
            ax = axes[idx]
            ff_vals = np.array(results[material]['ff']['T'])
            
            for obs_name in ['ft', 'tf', 'tt']:
                t_vals = np.array(results[material][obs_name]['T'])
                loss = (ff_vals - t_vals) / ff_vals * 100
                
                # Simple error propagation for loss
                ff_err = np.array(results[material]['ff']['T_err'])
                t_err = np.array(results[material][obs_name]['T_err'])
                loss_err = np.abs(loss) * np.sqrt((t_err/t_vals)**2 + (ff_err/ff_vals)**2)
                
                ax.errorbar(MOMENTA, loss, yerr=loss_err,
                            marker=markers[obs_name], color=colors[obs_name], 
                            label=f'Loss due to {labels_map[obs_name][:2]}', capsize=5, linestyle='--')
                
            ax.set_xlabel('Momentum (GeV/c)', fontsize=12)
            ax.set_ylabel('Relative Loss w.r.t Beam (%)', fontsize=12)
            ax.set_title(f'Fractional Beam Loss ({material})', fontsize=14, fontweight='bold')
            ax.legend()
            ax.grid(True, alpha=0.3)
        plt.tight_layout()
        pdf.savefig(fig)
        plt.close()

    # Save a summary table to file
    with open('transmission_results_table.txt', 'w') as f:
        f.write("Momentum | Material       | ff      | ft      | tf      | tt      \\n")
        f.write("-" * 75 + "\\n")
        for momentum_idx, momentum in enumerate(MOMENTA):
            for material in MATERIALS:
                ff_t = results[material]['ff']['T'][momentum_idx]
                ft_t = results[material]['ft']['T'][momentum_idx]
                tf_t = results[material]['tf']['T'][momentum_idx]
                tt_t = results[material]['tt']['T'][momentum_idx]
                f.write(f"{momentum:>8.2f} | {material:<14} | {ff_t:.4f} | {ft_t:.4f} | {tf_t:.4f} | {tt_t:.4f} \\n")

if __name__ == '__main__':
    main()
