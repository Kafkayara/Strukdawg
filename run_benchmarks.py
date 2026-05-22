import subprocess
import os
import shutil
import re
import json

scenarios = [
    # Size based
    {"name": "Kecil (V=10, E=30)", "folder": "data_kecil", "size": 10},
    {"name": "Sedang (V=50, E=150)", "folder": "data_sedang", "size": 50},
    {"name": "Progress (V=100, E=400)", "folder": "data_progress", "size": 100},
    {"name": "Besar (V=500, E=2000)", "folder": "data_besar", "size": 500},
    
    # Density based
    {"name": "Density Sparse (V=100, E=150)", "folder": "density_sparse", "size": 100},
    {"name": "Density Normal (V=100, E=500)", "folder": "density_normal", "size": 100},
    {"name": "Density Dense (V=100, E=3000)", "folder": "density_dense", "size": 100},
]

def parse_output(stdout_str):
    metrics = {}
    lines = stdout_str.split('\n')
    
    # Extract memory and basic info
    for line in lines:
        if "Memory Usage:" in line:
            parts = line.split("Memory Usage:")
            if len(parts) > 1:
                try:
                    mem_str = parts[1].strip().split()[0]
                    metrics["initial_mem_kb"] = float(mem_str)
                except ValueError:
                    pass
        elif "Load " in line and " lokasi" in line:
            parts = line.split("Load ")
            if len(parts) > 1:
                try:
                    metrics["num_locations"] = int(parts[1].split()[0])
                except ValueError:
                    pass
        elif "Load " in line and " rute" in line:
            parts = line.split("Load ")
            if len(parts) > 1:
                try:
                    metrics["num_routes"] = int(parts[1].split()[0])
                except ValueError:
                    pass
                    
    # Parse execution times
    current_op = None
    for line in lines:
        if "Lokasi ditambahkan:" in line:
            current_op = "insert_lokasi"
        elif "Rute ditambahkan:" in line:
            current_op = "insert_rute"
        elif "berhasil diperbarui!" in line and "Lokasi" in line:
            current_op = "update_lokasi"
        elif "diperbarui menjadi" in line and "Rute" in line:
            current_op = "update_rute"
        elif "RUTE DITEMUKAN!" in line or ("Rute" in line and "tidak ditemukan" in line) or ("Lokasi tidak ditemukan" in line) or ("Rute tidak ditemukan" in line):
            current_op = "search_rute"
        elif "berhasil dihapus!" in line and "Rute" in line:
            current_op = "delete_rute"
        elif "beserta rute terkait berhasil dihapus!" in line:
            current_op = "delete_lokasi"
            
        if current_op and ("Waktu eksekusi:" in line or "Waktu:" in line):
            parts = line.split("Waktu")
            if len(parts) > 1:
                try:
                    time_part = parts[1].replace("eksekusi:", "").replace(":", "").strip()
                    time_val = float(time_part.split()[0])
                    metrics[current_op] = time_val
                except ValueError:
                    pass
                current_op = None
                
    return metrics

def run_program(executable, size):
    new_id = f"L{str(size + 1).zfill(3)}"
    
    # Sequence of CRUD commands:
    # 1. Insert Lokasi (1) -> NewLoc, Tujuan
    # 2. Insert Rute (2) -> L001, new_id, 12.5
    # 3. Update Lokasi (3) -> L001, L001_Updated, Gudang
    # 4. Update Rute (4) -> L001, new_id, 15.5
    # 5. Search Rute (7) -> L001, new_id
    # 6. Delete Rute (6) -> L001, new_id
    # 7. Delete Lokasi (5) -> L001
    # 8. Save & Exit (10)
    
    commands = [
        "1", "NewLoc", "Tujuan",
        "2", "L001", new_id, "12.5",
        "3", "L001", "L001_Updated", "Gudang",
        "4", "L001", new_id, "15.5",
        "7", "L001", new_id,
        "6", "L001", new_id,
        "5", "L001",
        "10"
    ]
    
    input_str = "\n".join(commands) + "\n"
    
    p = subprocess.Popen(
        [executable],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        encoding='utf-8',
        errors='ignore'
    )
    
    stdout, stderr = p.communicate(input=input_str)
    return parse_output(stdout), stdout, stderr

def main():
    os.makedirs("data", exist_ok=True)
    results = {}
    
    for s in scenarios:
        name = s["name"]
        folder = s["folder"]
        size = s["size"]
        
        print(f"\n🚀 Running benchmark for Scenario: {name} ...")
        
        # 1. Copy scenario data to data/ for List Manager
        shutil.copy(os.path.join(folder, "lokasi.txt"), os.path.join("data", "lokasi.txt"))
        shutil.copy(os.path.join(folder, "rute.txt"), os.path.join("data", "rute.txt"))
        
        # 2. Run Adjacency List (rute_manager.exe)
        list_metrics, list_out, list_err = run_program(".\\rute_manager.exe", size)
        
        # 3. Re-copy scenario data to data/ for Matrix Manager to ensure identical initial state
        shutil.copy(os.path.join(folder, "lokasi.txt"), os.path.join("data", "lokasi.txt"))
        shutil.copy(os.path.join(folder, "rute.txt"), os.path.join("data", "rute.txt"))
        
        # 4. Run Adjacency Matrix (rute_manager_matrix.exe)
        matrix_metrics, matrix_out, matrix_err = run_program(".\\rute_manager_matrix.exe", size)
        
        results[name] = {
            "list": list_metrics,
            "matrix": matrix_metrics
        }
        
        # Save output logs for inspection if needed
        os.makedirs("benchmark_logs", exist_ok=True)
        with open(f"benchmark_logs/{folder}_list.log", "w", encoding="utf-8") as f:
            f.write(list_out)
        with open(f"benchmark_logs/{folder}_matrix.log", "w", encoding="utf-8") as f:
            f.write(matrix_out)
            
    # Write full JSON results
    with open("benchmark_results.json", "w") as f:
        json.dump(results, f, indent=4)
        
    print("\n🎉 Benchmarks completed! Printing quick summary table:")
    print("=" * 115)
    print(f"{'Scenario':<30} | {'Metric':<18} | {'Adjacency List (ms)':<25} | {'Adjacency Matrix (ms)':<25}")
    print("=" * 115)
    
    ops = [
        ("initial_mem_kb", "RAM (KB)"),
        ("insert_lokasi", "Insert Location"),
        ("insert_rute", "Insert Route"),
        ("update_lokasi", "Update Location"),
        ("update_rute", "Update Route"),
        ("search_rute", "Search Route"),
        ("delete_rute", "Delete Route"),
        ("delete_lokasi", "Delete Location"),
    ]
    
    for name in results:
        print(f"\n--- {name} ---")
        for key, display in ops:
            v_list = results[name]["list"].get(key, "N/A")
            v_matrix = results[name]["matrix"].get(key, "N/A")
            
            # Formatting
            if isinstance(v_list, float):
                v_list_str = f"{v_list:.3f}" if "mem" not in key else f"{v_list:.1f}"
            else:
                v_list_str = str(v_list)
                
            if isinstance(v_matrix, float):
                v_matrix_str = f"{v_matrix:.3f}" if "mem" not in key else f"{v_matrix:.1f}"
            else:
                v_matrix_str = str(v_matrix)
                
            print(f"{name:<30} | {display:<18} | {v_list_str:<25} | {v_matrix_str:<25}")
            
    print("=" * 115)

if __name__ == "__main__":
    main()
