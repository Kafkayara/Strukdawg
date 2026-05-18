import pandas as pd

print("="*50)
print("MEMBUAT DATA UNTUK 3 TAHAP PENGUJIAN")
print("="*50)

# ============================================
# 1. BACA DATA PROGRESS YANG SUDAH ADA
# ============================================
df_lokasi = pd.read_csv('data/lokasi.txt')
df_rute = pd.read_csv('data/rute.txt')

print(f"\n📂 Data progress yang ada:")
print(f"   - Lokasi: {len(df_lokasi)} baris")
print(f"   - Rute: {len(df_rute)} baris")

# ============================================
# 2. BUAT FOLDER UNTUK MASING-MASING TAHAP
# ============================================
import os
os.makedirs('data_kecil', exist_ok=True)
os.makedirs('data_sedang', exist_ok=True)

print("\n📁 Folder dibuat: data_kecil, data_sedang")

# ============================================
# 3. BUAT DATA KECIL (10 LOKASI, 30 RUTE)
# ============================================
print("\n" + "-"*40)
print("MEMBUAT DATA KECIL (10 lokasi, 30 rute)")
print("-"*40)

# Ambil 10 lokasi pertama
df_lokasi_kecil = df_lokasi.head(10).copy()
df_lokasi_kecil.to_csv('data_kecil/lokasi.txt', index=False)
print(f"✅ Lokasi: {len(df_lokasi_kecil)} baris")

# Ambil 30 rute pertama
df_rute_kecil = df_rute.head(30).copy()
df_rute_kecil.to_csv('data_kecil/rute.txt', index=False)
print(f"✅ Rute: {len(df_rute_kecil)} baris")

# ============================================
# 4. BUAT DATA SEDANG (20 LOKASI, 60 RUTE)
# ============================================
print("\n" + "-"*40)
print("MEMBUAT DATA SEDANG (20 lokasi, 60 rute)")
print("-"*40)

# Ambil 20 lokasi pertama
df_lokasi_sedang = df_lokasi.head(20).copy()
df_lokasi_sedang.to_csv('data_sedang/lokasi.txt', index=False)
print(f"✅ Lokasi: {len(df_lokasi_sedang)} baris")

# Ambil 60 rute pertama
df_rute_sedang = df_rute.head(60).copy()
df_rute_sedang.to_csv('data_sedang/rute.txt', index=False)
print(f"✅ Rute: {len(df_rute_sedang)} baris")

# ============================================
# 5. RINGKASAN
# ============================================
print("\n" + "="*50)
print("RINGKASAN DATA YANG TERSEDIA")
print("="*50)
print(f"""
📁 data_kecil/   : {len(df_lokasi_kecil)} lokasi, {len(df_rute_kecil)} rute
📁 data_sedang/  : {len(df_lokasi_sedang)} lokasi, {len(df_rute_sedang)} rute
📁 data/         : {len(df_lokasi)} lokasi, {len(df_rute)} rute (progress)

✅ Semua data siap digunakan!
""")