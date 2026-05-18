import subprocess
import time
import os

print("="*60)
print("PERBANDINGAN PERFORMANCE: LIST vs MATRIX")
print("="*60)

# Pastikan data siap
os.system("copy data\\lokasi.txt data\\")
os.system("copy data\\rute.txt data\\")

def uji_program(nama_program, perintah_cari, perintah_insert):
    print(f"\n🔍 Menguji {nama_program}...")
    
    # UJI SEARCH (Cari rute L001 → L002)
    start = time.time()
    proses = subprocess.Popen([perintah_cari], 
                              stdin=subprocess.PIPE, 
                              stdout=subprocess.PIPE,
                              text=True)
    # Kirim perintah: 1 (Cari Rute) -> L001 -> L002 -> 4 (Keluar)
    commands = "1\nL001\nL002\n4\n"
    stdout, stderr = proses.communicate(input=commands)
    end = time.time()
    waktu_search = (end - start) * 1000
    
    # UJI INSERT (Tambah rute baru) - hanya untuk yang support insert
    if nama_program != "Matrix (Versi Dasar)":
        start = time.time()
        proses = subprocess.Popen([perintah_insert], 
                                  stdin=subprocess.PIPE, 
                                  stdout=subprocess.PIPE,
                                  text=True)
        # 2 (Insert Rute) -> L001 -> L999 -> 1.5 -> 4 (Keluar)
        commands = "2\nL001\nL999\n1.5\n4\n"
        stdout, stderr = proses.communicate(input=commands)
        end = time.time()
        waktu_insert = (end - start) * 1000
    else:
        waktu_insert = None
    
    return {"search": waktu_search, "insert": waktu_insert}

# ============================================
# UJI LIST (rute_manager.exe)
# ============================================
print("\n" + "-"*40)
print("📊 UJI ADJACENCY LIST")
print("-"*40)

hasil_list = uji_program("List", ".\\rute_manager.exe", ".\\rute_manager.exe")

# ============================================
# UJI MATRIX (rute_manager_matrix.exe)
# ============================================
print("\n" + "-"*40)
print("📊 UJI ADJACENCY MATRIX")
print("-"*40)

hasil_matrix = uji_program("Matrix", ".\\rute_manager_matrix.exe", None)

# ============================================
# TAMPILKAN HASIL
# ============================================
print("\n" + "="*60)
print("📈 HASIL PERBANDINGAN PERFORMANCE")
print("="*60)

print("\n┌──────────────┬─────────────┬─────────────┐")
print("│ Struktur     │ Search (ms) │ Insert (ms) │")
print("├──────────────┼─────────────┼─────────────┤")
print(f"│ Adjacency List │ {hasil_list['search']:11.3f} │ {hasil_list['insert']:11.3f} │")
print(f"│ Adjacency Matrix│ {hasil_matrix['search']:11.3f} │ {'N/A':11} │")
print("└──────────────┴─────────────┴─────────────┘")

print("\n📊 ANALISIS:")
print(f"   - Search Matrix {'LEBIH CEPAT' if hasil_matrix['search'] < hasil_list['search'] else 'LEBIH LAMBAT'}")
if hasil_list['search'] > 0:
    persen = (hasil_matrix['search'] / hasil_list['search']) * 100
    print(f"   - Matrix {persen:.1f}% dari waktu List")

print("\n💾 PERKIRAAN MEMORI (30 lokasi):")
print(f"   - Adjacency List : ~256 KB (hanya simpan 100 rute)")
print(f"   - Adjacency Matrix: ~900 KB (30x30 = 900 slot, walau kosong)")