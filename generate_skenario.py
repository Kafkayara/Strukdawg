import pandas as pd
import random
import os

print("="*60)
print("MEMBUAT DATA UNTUK 3 SKENARIO PENGUJIAN")
print("="*60)

# Baca data mentah (50 baris pertama)
df = pd.read_csv('delivery_sh_50data.csv')
print(f"\n📂 Data mentah: {len(df)} baris")

# ============================================
# FUNGSI MEMBUAT DATA LOKASI & RUTE
# ============================================
def buat_data(jumlah_lokasi, jumlah_rute, nama_folder):
    print(f"\n📌 Membuat skenario: {nama_folder} ({jumlah_lokasi} lokasi, {jumlah_rute} rute)")
    
    # Buat folder
    if not os.path.exists(nama_folder):
        os.makedirs(nama_folder)
    
    # 1. Ambil lokasi unik sebanyak yang diminta
    unique_coords = df[['lng', 'lat']].drop_duplicates().head(jumlah_lokasi).reset_index(drop=True)
    unique_coords['id_lokasi'] = ['L' + str(i+1).zfill(3) for i in range(len(unique_coords))]
    
    # Simpan lokasi.txt
    with open(f'{nama_folder}/lokasi.txt', 'w', encoding='utf-8') as f:
        f.write('id_lokasi,nama_lokasi,tipe\n')
        for idx, row in unique_coords.iterrows():
            f.write(f"{row['id_lokasi']},Lokasi_{idx+1},Tujuan\n")
    
    print(f"   ✅ {nama_folder}/lokasi.txt: {len(unique_coords)} lokasi")
    
    # 2. Buat mapping koordinat ke ID
    coord_to_id = {}
    for _, row in unique_coords.iterrows():
        key = f"{row['lng']:.5f}_{row['lat']:.5f}"
        coord_to_id[key] = row['id_lokasi']
    
    # 3. Buat rute
    with open(f'{nama_folder}/rute.txt', 'w', encoding='utf-8') as f:
        f.write('id_rute,asal,tujuan,jarak\n')
        
        rute_counter = 0
        rute_set = set()
        
        # Ambil dari data berurutan dulu
        semua_id = list(unique_coords['id_lokasi'])
        
        while rute_counter < jumlah_rute:
            asal = random.choice(semua_id)
            tujuan = random.choice(semua_id)
            
            if asal != tujuan:
                rute_key = f"{asal}_{tujuan}"
                if rute_key not in rute_set:
                    jarak = round(random.uniform(0.1, 5.0), 2)
                    rute_counter += 1
                    id_rute = f"R{str(rute_counter).zfill(4)}"
                    f.write(f"{id_rute},{asal},{tujuan},{jarak}\n")
                    rute_set.add(rute_key)
        
        print(f"   ✅ {nama_folder}/rute.txt: {rute_counter} rute")
    
    return len(unique_coords), rute_counter

# ============================================
# BUAT 3 SKENARIO
# ============================================

skenario = [
    {"folder": "data_kecil", "lokasi": 10, "rute": 30},
    {"folder": "data_sedang", "lokasi": 25, "rute": 66},
    {"folder": "data_progress", "lokasi": 30, "rute": 100},
]

for s in skenario:
    buat_data(s["lokasi"], s["rute"], s["folder"])

# ============================================
# RINGKASAN
# ============================================
print("\n" + "="*60)
print("RINGKASAN DATA YANG TELAH DIBUAT:")
print("="*60)
print("""
📁 data_kecil/    : 10 lokasi, 30 rute
📁 data_sedang/   : 25 lokasi, 66 rute
📁 data_progress/ : 30 lokasi, 100 rute
""")
print("✅ Selesai! Sekarang kamu punya 3 folder data untuk pengujian.")