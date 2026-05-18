import pandas as pd

print("="*50)
print("MEMBACA 50 DATA PERTAMA DARI delivery_sh.csv")
print("="*50)

# Baca file CSV (ambil 50 baris pertama saja)
df = pd.read_csv('delivery_sh.csv', nrows=50)

print(f"\n✅ Berhasil membaca {len(df)} baris data")
print(f"✅ Kolom yang tersedia: {list(df.columns)}")

# Tampilkan 5 data pertama untuk preview (PAKAI order_id, BUKAN package_id)
print("\n📋 PREVIEW 5 DATA PERTAMA:")
print(df[['order_id', 'city', 'lng', 'lat']].head())

# Simpan ke file baru
df.to_csv('delivery_sh_50data.csv', index=False)
print("\n💾 Data 50 baris pertama disimpan ke 'delivery_sh_50data.csv'")