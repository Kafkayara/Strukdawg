import subprocess
import time
import os

print("="*70)
print(" PENGUJIAN OPERASI SPESIFIK")
print("="*70)

skenario = [
    {"nama": "Kecil", "folder": "data_kecil", "lokasi": 10, "rute": 30},
    {"nama": "Sedang", "folder": "data_sedang", "lokasi": 25, "rute": 66},
    {"nama": "Progress", "folder": "data_progress", "lokasi": 30, "rute": 100},
]

def uji_insert_search(nama, folder, lokasi, rute):
    print(f"\n🔍 UJI {nama} ({lokasi} lokasi, {rute} rute)")
    
    # Copy data
    os.system(f"copy {folder}\\* data\\")
    
    # Jalankan program dengan perintah khusus
    # Kita akan gunakan pendekatan manual karena agak kompleks
    # Untuk sementara, kita akan ukur manual dulu
    
    print("   Silakan jalankan program manual dan catat:")
    print("   - Waktu insert 1 rute")
    print("   - Waktu search berhasil")
    print("   - Waktu search gagal")
    print()

# Jalankan
for s in skenario:
    uji_insert_search(s["nama"], s["folder"], s["lokasi"], s["rute"])

print("\n✅ Catat hasilnya di tabel berikut:")
print("""
| Skenario | Insert 1 rute | Search berhasil | Search gagal |
|----------|---------------|-----------------|--------------|
| Kecil    | _____ ms      | _____ ms        | _____ ms     |
| Sedang   | _____ ms      | _____ ms        | _____ ms     |
| Progress | _____ ms      | _____ ms        | _____ ms     |
""")