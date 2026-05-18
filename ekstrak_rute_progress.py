import pandas as pd
import math
import random

print("="*60)
print("EKSTRAKSI DATA - PROGRESS (30 LOKASI, 100 RUTE)")
print("="*60)

# Baca 50 data
df = pd.read_csv('delivery_sh_50data.csv')
print(f"\n📂 Membaca {len(df)} baris data")

# ============================================
# 1. EKSTRAK LOKASI (AMBIL 30 SAJA)
# ============================================
print("\n" + "-"*40)
print("TAHAP 1: EKSTRAK 30 LOKASI")
print("-"*40)

# Ambil koordinat unik, lalu ambil 30 pertama
unique_coords = df[['lng', 'lat']].drop_duplicates().head(30).reset_index(drop=True)
print(f"📍 Ditemukan {len(unique_coords)} lokasi unik")

# Buat ID lokasi (L001, L002, ...)
unique_coords['id_lokasi'] = ['L' + str(i+1).zfill(3) for i in range(len(unique_coords))]

# Buat folder data
import os
if not os.path.exists('data'):
    os.makedirs('data')

# Simpan lokasi.txt
with open('data/lokasi.txt', 'w', encoding='utf-8') as f:
    f.write('id_lokasi,nama_lokasi,tipe\n')
    for idx, row in unique_coords.iterrows():
        nama = f"Shanghai_{idx+1}"
        f.write(f"{row['id_lokasi']},{nama},Tujuan\n")

print(f"💾 File data/lokasi.txt: {len(unique_coords)} lokasi")

# ============================================
# 2. EKSTRAK RUTE (BUAT 100 RUTE)
# ============================================
print("\n" + "-"*40)
print("TAHAP 2: EKSTRAK 100 RUTE")
print("-"*40)

# Buat dictionary mapping koordinat -> id_lokasi
coord_to_id = {}
for _, row in unique_coords.iterrows():
    key = f"{row['lng']:.5f}_{row['lat']:.5f}"
    coord_to_id[key] = row['id_lokasi']

# Buat rute.txt
with open('data/rute.txt', 'w', encoding='utf-8') as f:
    f.write('id_rute,asal,tujuan,jarak\n')
    
    rute_counter = 0
    rute_set = set()  # Untuk cegah duplikasi
    
    # BAGIAN 1: Rute dari data berurutan
    print("\n📌 Membuat rute dari data berurutan...")
    for i in range(len(df) - 1):
        if rute_counter >= 100:
            break
            
        row_now = df.iloc[i]
        row_next = df.iloc[i + 1]
        
        asal_key = f"{row_now['lng']:.5f}_{row_now['lat']:.5f}"
        tujuan_key = f"{row_next['lng']:.5f}_{row_next['lat']:.5f}"
        
        if asal_key in coord_to_id and tujuan_key in coord_to_id:
            asal_id = coord_to_id[asal_key]
            tujuan_id = coord_to_id[tujuan_key]
            
            if asal_id != tujuan_id:
                # Cek duplikasi
                rute_key = f"{asal_id}_{tujuan_id}"
                if rute_key not in rute_set:
                    jarak = math.sqrt(
                        (row_next['lng'] - row_now['lng'])**2 + 
                        (row_next['lat'] - row_now['lat'])**2
                    ) * 111
                    jarak = round(jarak, 2)
                    
                    rute_counter += 1
                    id_rute = f"R{str(rute_counter).zfill(4)}"
                    
                    f.write(f"{id_rute},{asal_id},{tujuan_id},{jarak}\n")
                    rute_set.add(rute_key)
                    print(f"   ✓ {id_rute}: {asal_id} → {tujuan_id}")
    
    # BAGIAN 2: Tambah rute acak sampai 100
    print(f"\n📌 Menambahkan rute acak (saat ini {rute_counter} rute)...")
    
    semua_id = list(unique_coords['id_lokasi'])
    max_iter = 1000
    iterasi = 0
    
    while rute_counter < 100 and iterasi < max_iter:
        asal = random.choice(semua_id)
        tujuan = random.choice(semua_id)
        
        if asal != tujuan:
            rute_key = f"{asal}_{tujuan}"
            if rute_key not in rute_set:
                # Hitung jarak acak antara 0.1 - 5 km
                jarak = round(random.uniform(0.1, 5.0), 2)
                
                rute_counter += 1
                id_rute = f"R{str(rute_counter).zfill(4)}"
                
                f.write(f"{id_rute},{asal},{tujuan},{jarak}\n")
                rute_set.add(rute_key)
                print(f"   ✓ {id_rute} (ACAK): {asal} → {tujuan} = {jarak} km")
        
        iterasi += 1

print(f"\n✅ TOTAL RUTE: {rute_counter}")

# ============================================
# 3. STATISTIK
# ============================================
print("\n" + "="*60)
print("HASIL EKSTRAKSI - PROGRESS")
print("="*60)
print(f"""
📊 STATISTIK DATA:
   - Jumlah lokasi     : {len(unique_coords)} node
   - Jumlah rute       : {rute_counter} edge
   - Rasio rute/lokasi : {rute_counter/len(unique_coords):.1f} : 1

📁 FILE OUTPUT:
   - data/lokasi.txt : {len(unique_coords)} lokasi
   - data/rute.txt   : {rute_counter} rute

✅ Data progress siap!
""")