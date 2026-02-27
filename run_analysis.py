#!/usr/bin/env python3
"""
Run all configurations of the proton transmission simulation
and analyze the results.
"""

import subprocess
import os
import sys
import numpy as np
import matplotlib.pyplot as plt

# Configuration
MOMENTA = [0.5, 1.0, 2.0, 3.0, 5.0]  # GeV/c
N_EVENTS = 100000

# Results storage
results = {
    'momentum': [],
    'T_material': [],
    'T_magnet': [],
    'T_hybrid': [],
    'T_material_err': [],
    'T_magnet_err': [],
    'T_hybrid_err': []
}

def create_macro(momentum, config_name, magnet_on, slab_on):
    """Create a macro file for a specific configuration"""
    macro_content = f"""# Generated macro for {config_name} at {momentum} GeV/c
/run/initialize
/gun/momentum {momentum} GeV
/run/beamOn {N_EVENTS}
"""
    
    filename = f"run_{config_name}_{momentum}GeV.mac"
    with open(filename, 'w') as f:
        f.write(macro_content)
    
    return filename

def run_simulation(macro_file):
    """Run GEANT4 simulation with given macro"""
    print(f"Running simulation with {macro_file}...")
    
    try:
        result = subprocess.run(
            ['./protonTransmission', macro_file],
            capture_output=True,
            text=True,
            timeout=600  # 10 minute timeout
        )
        
        # Parse output to get hit count
        # This is a placeholder - you'll need to implement proper output parsing
        # or modify the GEANT4 code to write results to a file
        
        hits = parse_hits_from_output(result.stdout)
        return hits
        
    except subprocess.TimeoutExpired:
        print(f"ERROR: Simulation timed out for {macro_file}")
        return 0
    except Exception as e:
        print(f"ERROR running simulation: {e}")
        return 0

def parse_hits_from_output(output):
    """Parse the number of hits from GEANT4 output"""
    # This is a placeholder - implement based on your actual output format
    # For now, return a dummy value
    # In reality, you should grep for scoring information
    return 50000  # Placeholder

def calculate_transmission(hits, n_events):
    """Calculate transmission and error"""
    T = hits / n_events
    T_err = np.sqrt(hits) / n_events
    return T, T_err

def main():
    print("=" * 70)
    print("Proton Transmission Simulation - Automated Run Script")
    print("=" * 70)
    print()
    
    # Check if executable exists
    if not os.path.exists('./protonTransmission'):
        print("ERROR: protonTransmission executable not found!")
        print("Please build the project first using build.sh or build.bat")
        sys.exit(1)
    
    # Run all configurations
    for momentum in MOMENTA:
        print(f"\n{'=' * 70}")
        print(f"Running momentum: {momentum} GeV/c")
        print('=' * 70)
        
        # Run A: Material only
        print(f"\n  Configuration A: Material only (B=0, slab=ON)")
        macro_A = create_macro(momentum, "material", False, True)
        hits_material = run_simulation(macro_A)
        T_material, T_material_err = calculate_transmission(hits_material, N_EVENTS)
        
        # Run B: Magnet only
        print(f"\n  Configuration B: Magnet only (B=1T, slab=OFF)")
        macro_B = create_macro(momentum, "magnet", True, False)
        hits_magnet = run_simulation(macro_B)
        T_magnet, T_magnet_err = calculate_transmission(hits_magnet, N_EVENTS)
        
        # Run C: Hybrid
        print(f"\n  Configuration C: Hybrid (B=1T, slab=ON)")
        macro_C = create_macro(momentum, "hybrid", True, True)
        hits_hybrid = run_simulation(macro_C)
        T_hybrid, T_hybrid_err = calculate_transmission(hits_hybrid, N_EVENTS)
        
        # Store results
        results['momentum'].append(momentum)
        results['T_material'].append(T_material)
        results['T_magnet'].append(T_magnet)
        results['T_hybrid'].append(T_hybrid)
        results['T_material_err'].append(T_material_err)
        results['T_magnet_err'].append(T_magnet_err)
        results['T_hybrid_err'].append(T_hybrid_err)
        
        print(f"\n  Results for {momentum} GeV/c:")
        print(f"    T_material = {T_material:.4f} ± {T_material_err:.4f}")
        print(f"    T_magnet   = {T_magnet:.4f} ± {T_magnet_err:.4f}")
        print(f"    T_hybrid   = {T_hybrid:.4f} ± {T_hybrid_err:.4f}")
    
    # Calculate deviations
    T_product = np.array(results['T_material']) * np.array(results['T_magnet'])
    delta = (np.array(results['T_hybrid']) / T_product) - 1
    delta_percent = delta * 100
    
    # Calculate error on delta (simplified)
    delta_err = np.array(results['T_hybrid_err']) / T_product * 100
    
    # Print summary table
    print("\n" + "=" * 70)
    print("SUMMARY RESULTS")
    print("=" * 70)
    print(f"{'Momentum':>10} {'T_material':>12} {'T_magnet':>12} {'T_hybrid':>12} {'T_product':>12} {'Δ (%)':>10}")
    print("-" * 70)
    
    for i, p in enumerate(results['momentum']):
        print(f"{p:>10.1f} {results['T_material'][i]:>12.4f} "
              f"{results['T_magnet'][i]:>12.4f} {results['T_hybrid'][i]:>12.4f} "
              f"{T_product[i]:>12.4f} {delta_percent[i]:>10.2f}")
    
    # Generate plots
    generate_plots(results, T_product, delta_percent, delta_err)
    
    print("\n" + "=" * 70)
    print("Analysis complete! Check the output plots.")
    print("=" * 70)

def generate_plots(results, T_product, delta_percent, delta_err):
    """Generate analysis plots"""
    
    # Plot 1: Transmission curves
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))
    
    # Transmission vs momentum
    ax1.errorbar(results['momentum'], results['T_material'], 
                 yerr=results['T_material_err'],
                 marker='o', label='T_material', capsize=5)
    ax1.errorbar(results['momentum'], results['T_magnet'], 
                 yerr=results['T_magnet_err'],
                 marker='s', label='T_magnet', capsize=5)
    ax1.errorbar(results['momentum'], results['T_hybrid'], 
                 yerr=results['T_hybrid_err'],
                 marker='^', label='T_hybrid', capsize=5)
    ax1.plot(results['momentum'], T_product, 'k--', label='T_magnet × T_material')
    
    ax1.set_xlabel('Momentum (GeV/c)', fontsize=12)
    ax1.set_ylabel('Transmission', fontsize=12)
    ax1.set_title('Transmission vs Momentum', fontsize=14, fontweight='bold')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # Deviation plot
    ax2.errorbar(results['momentum'], delta_percent, yerr=delta_err,
                 marker='o', color='red', capsize=5, linewidth=2)
    ax2.axhline(y=0, color='k', linestyle='--', alpha=0.5)
    ax2.set_xlabel('Momentum (GeV/c)', fontsize=12)
    ax2.set_ylabel('Δ (%)', fontsize=12)
    ax2.set_title('Deviation from Independence', fontsize=14, fontweight='bold')
    ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('transmission_analysis.png', dpi=300, bbox_inches='tight')
    print("\nPlot saved as: transmission_analysis.png")

if __name__ == '__main__':
    main()
