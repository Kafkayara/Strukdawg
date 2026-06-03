import subprocess
import time
import os

print("="*70)
print(" PENGUJIAN OTOMATIS 3 SKENARIO")
print("="*70)

# Daftar skenario yang akan diuji
skenario = [
    {"nama": "Kecil", "folder": "data_kecil", "lokasi": 10, "rute": 30},
    {"nama": "Sedang", "folder": "data_sedang", "lokasi": 25, "rute": 66},
    {"nama": "Progress", "folder": "data_progress", "lokasi": 30, "rute": 100},
]

# File untuk menyimpan hasil
hasil_file = "hasil_pengujian.txt"
with open(hasil_file, "w") as f:
    f.write("="*60 + "\n")
    f.write("HASIL PENGUJIAN PERFORMA PROGRAM\n")
    f.write("="*60 + "\n\n")

print("\n" + "-"*70)

# ============================================
# FUNGSI UNTUK MENGUJI 1 SKENARIO
# ============================================
def uji_skenario(nama, folder, lokasi, rute):
    print(f"\n🔍 MENGUJI SKENARIO {nama} ({lokasi} lokasi, {rute} rute)")
    print("-"*50)
    
    # 1. Copy file data ke folder data/
    print(f"📋 Copy data dari folder {folder}/ ke data/...")
    
    # Hapus file lama di folder data/ (kalau ada)
    if os.path.exists("data/lokasi.txt"):
        os.remove("data/lokasi.txt")
    if os.path.exists("data/rute.txt"):
        os.remove("data/rute.txt")
    
    # Copy file baru
    os.system(f"copy {folder}\\lokasi.txt data\\")
    os.system(f"copy {folder}\\rute.txt data\\")
    
    # 2. Jalankan program dan hitung waktu TOTAL
    print("⏱️  Menjalankan program...")
    start_total = time.time()
    
    # Jalankan program (akan tampil menu)
    proses = subprocess.Popen([".\\rute_manager.exe"], 
                              stdin=subprocess.PIPE, 
                              stdout=subprocess.PIPE,
                              stderr=subprocess.PIPE,
                              text=True,
                              encoding='utf-8')
    
    # Kirim perintah ke program:
    # 8 (Tampilkan Semua Rute) + Enter
    # 10 (Simpan & Keluar) + Enter
    commands = "8\n10\n"
    stdout, stderr = proses.communicate(input=commands)
    
    end_total = time.time()
    waktu_total = (end_total - start_total) * 1000  # dalam ms
    
    print(f"✅ Selesai! Waktu total: {waktu_total:.2f} ms")
    
    # 3. Simpan hasil ke file
    with open(hasil_file, "a") as f:
        f.write(f"\nSKENARIO {nama}\n")
        f.write(f"-"*30 + "\n")
        f.write(f"Jumlah lokasi: {lokasi}\n")
        f.write(f"Jumlah rute  : {rute}\n")
        f.write(f"Waktu total   : {waktu_total:.2f} ms\n")
    
    return waktu_total

# ============================================
# JALANKAN PENGUJIAN UNTUK SEMUA SKENARIO
# ============================================
hasil = []

for s in skenario:
    waktu = uji_skenario(s["nama"], s["folder"], s["lokasi"], s["rute"])
    hasil.append({
        "nama": s["nama"],
        "lokasi": s["lokasi"],
        "rute": s["rute"],
        "waktu": waktu
    })
    print("-"*50)

# ============================================
# TAMPILKAN RINGKASAN
# ============================================
print("\n" + "="*70)
print(" RINGKASAN HASIL PENGUJIAN")
print("="*70)

print("\n📊 TABEL HASIL PENGUJIAN:")
print("-"*60)
print(f"{'Skenario':<10} {'Lokasi':<10} {'Rute':<10} {'Waktu Total (ms)':<20}")
print("-"*60)

for h in hasil:
    print(f"{h['nama']:<10} {h['lokasi']:<10} {h['rute']:<10} {h['waktu']:<20.2f}")

print("-"*60)

# Hitung kenaikan
if len(hasil) >= 2:
    kenaikan_kecil_ke_sedang = ((hasil[1]['waktu'] - hasil[0]['waktu']) / hasil[0]['waktu']) * 100
    kenaikan_sedang_ke_progress = ((hasil[2]['waktu'] - hasil[1]['waktu']) / hasil[1]['waktu']) * 100
    
    print(f"\n📈 Analisis Kenaikan:")
    print(f"   Kecil → Sedang   : +{kenaikan_kecil_ke_sedang:.1f}%")
    print(f"   Sedang → Progress : +{kenaikan_sedang_ke_progress:.1f}%")

print("\n" + "="*70)
print(f"✅ Hasil lengkap disimpan di: {hasil_file}")
print("="*70)