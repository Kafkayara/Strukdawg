import subprocess
import time
import os
import re

print("="*70)
print(" PENGUJIAN OPERASI SPESIFIK (INSERT & SEARCH)")
print("="*70)

# Daftar skenario
skenario = [
    {"nama": "Kecil", "folder": "data_kecil", "lokasi": 10, "rute": 30},
    {"nama": "Sedang", "folder": "data_sedang", "lokasi": 25, "rute": 66},
    {"nama": "Progress", "folder": "data_progress", "lokasi": 30, "rute": 100},
]

# File hasil
hasil_file = "hasil_operasi.txt"
with open(hasil_file, "w") as f:
    f.write("="*70 + "\n")
    f.write("HASIL PENGUJIAN OPERASI INSERT & SEARCH\n")
    f.write("="*70 + "\n\n")

# ============================================
# FUNGSI UJI 1 SKENARIO
# ============================================
def uji_skenario(nama, folder, lokasi, rute):
    print(f"\n🔍 MENGUJI SKENARIO {nama} ({lokasi} lokasi, {rute} rute)")
    print("-"*60)
    
    # 1. Copy data
    os.system(f"copy {folder}\\* data\\ > nul")
    print("✅ Data siap")
    
    # 2. UJI INSERT 1 RUTE
    print("\n⏱️  Mengukur INSERT 1 rute...")
    
    start = time.time()
    
    # Jalankan program dengan perintah insert rute
    proses = subprocess.Popen([".\\rute_manager.exe"], 
                              stdin=subprocess.PIPE, 
                              stdout=subprocess.PIPE,
                              stderr=subprocess.PIPE,
                              text=True)
    
    # Kirim perintah: 
    # 2 (Insert Rute) + Enter
    # L001 + Enter (asal)
    # L002 + Enter (tujuan)
    # 1.5 + Enter (jarak)
    # 6 + Enter (keluar)
    commands = "2\nL001\nL002\n1.5\n6\n"
    stdout, stderr = proses.communicate(input=commands)
    
    end = time.time()
    waktu_insert = (end - start) * 1000  # dalam ms
    print(f"   ✅ Insert 1 rute: {waktu_insert:.3f} ms")
    
    # 3. UJI SEARCH BERHASIL
    print("\n⏱️  Mengukur SEARCH BERHASIL...")
    
    start = time.time()
    
    proses = subprocess.Popen([".\\rute_manager.exe"], 
                              stdin=subprocess.PIPE, 
                              stdout=subprocess.PIPE,
                              text=True)
    
    # Cari rute yang pasti ada (L001 → L002)
    commands = "3\nL001\nL002\n6\n"
    stdout, stderr = proses.communicate(input=commands)
    
    end = time.time()
    waktu_search_berhasil = (end - start) * 1000
    print(f"   ✅ Search berhasil: {waktu_search_berhasil:.3f} ms")
    
    # 4. UJI SEARCH GAGAL
    print("\n⏱️  Mengukur SEARCH GAGAL...")
    
    start = time.time()
    
    proses = subprocess.Popen([".\\rute_manager.exe"], 
                              stdin=subprocess.PIPE, 
                              stdout=subprocess.PIPE,
                              text=True)
    
    # Cari rute yang TIDAK ada (L999 → L999)
    commands = "3\nL999\nL999\n6\n"
    stdout, stderr = proses.communicate(input=commands)
    
    end = time.time()
    waktu_search_gagal = (end - start) * 1000
    print(f"   ✅ Search gagal: {waktu_search_gagal:.3f} ms")
    
    # 5. Simpan hasil
    with open(hasil_file, "a") as f:
        f.write(f"\nSKENARIO {nama}\n")
        f.write(f"Jumlah lokasi: {lokasi}\n")
        f.write(f"Jumlah rute  : {rute}\n")
        f.write(f"Insert 1 rute    : {waktu_insert:.3f} ms\n")
        f.write(f"Search berhasil  : {waktu_search_berhasil:.3f} ms\n")
        f.write(f"Search gagal     : {waktu_search_gagal:.3f} ms\n")
        f.write("-"*40 + "\n")
    
    return {
        "insert": waktu_insert,
        "berhasil": waktu_search_berhasil,
        "gagal": waktu_search_gagal
    }

# ============================================
# JALANKAN SEMUA SKENARIO
# ============================================
hasil_list = []

for s in skenario:
    hasil = uji_skenario(s["nama"], s["folder"], s["lokasi"], s["rute"])
    hasil_list.append({
        "nama": s["nama"],
        "lokasi": s["lokasi"],
        "rute": s["rute"],
        "insert": hasil["insert"],
        "berhasil": hasil["berhasil"],
        "gagal": hasil["gagal"]
    })
    print("\n" + "="*60)

# ============================================
# TAMPILKAN TABEL RINGKASAN
# ============================================
print("\n" + "="*70)
print(" RINGKASAN HASIL PENGUJIAN")
print("="*70)

print("\n📊 TABEL PERBANDINGAN PERFORMANCE:")
print("-"*80)
print(f"{'Skenario':<10} {'Lokasi':<8} {'Rute':<8} {'Insert (ms)':<12} {'Search Berhasil':<15} {'Search Gagal':<15}")
print("-"*80)

for h in hasil_list:
    print(f"{h['nama']:<10} {h['lokasi']:<8} {h['rute']:<8} {h['insert']:<12.3f} {h['berhasil']:<15.3f} {h['gagal']:<15.3f}")

print("-"*80)

# ============================================
# ANALISIS TREN
# ============================================
print("\n📈 ANALISIS TREN PERFORMANCE:")
print("-"*40)

if len(hasil_list) >= 2:
    for i in range(1, len(hasil_list)):
        prev = hasil_list[i-1]
        curr = hasil_list[i]
        
        kenaikan_insert = ((curr['insert'] - prev['insert']) / prev['insert']) * 100
        kenaikan_berhasil = ((curr['berhasil'] - prev['berhasil']) / prev['berhasil']) * 100
        
        print(f"\n{prev['nama']} → {curr['nama']}:")
        print(f"   Insert : +{kenaikan_insert:.1f}%")
        print(f"   Search : +{kenaikan_berhasil:.1f}%")

print("\n" + "="*70)
print(f"✅ Hasil lengkap disimpan di: {hasil_file}")
print("="*70)