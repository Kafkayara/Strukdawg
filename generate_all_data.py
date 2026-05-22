import csv
import math
import random
import os

def load_unique_coordinates(csv_path, max_count=500):
    print(f"📖 Reading {csv_path} for unique locations...")
    coords = []
    seen = set()
    with open(csv_path, 'r', encoding='utf-8') as f:
        reader = csv.reader(f)
        header = next(reader)
        # Find indices for lng and lat
        lng_idx = header.index('lng')
        lat_idx = header.index('lat')
        
        for row in reader:
            if not row:
                continue
            lng = float(row[lng_idx])
            lat = float(row[lat_idx])
            coord = (lng, lat)
            if coord not in seen:
                seen.add(coord)
                coords.append(coord)
                if len(coords) >= max_count:
                    break
    print(f"✅ Found {len(coords)} unique locations.")
    return coords

def calculate_distance(c1, c2):
    # Euclidean distance scaled to km (approx 111km per degree)
    lng1, lat1 = c1
    lng2, lat2 = c2
    dist = math.sqrt((lng1 - lng2)**2 + (lat1 - lat2)**2) * 111.0
    return round(max(0.1, dist), 2)

def generate_scenario(folder, locations, num_routes):
    print(f"📁 Generating scenario in '{folder}' ({len(locations)} locations, {num_routes} routes)...")
    os.makedirs(folder, exist_ok=True)
    
    # 1. Write lokasi.txt
    lokasi_path = os.path.join(folder, "lokasi.txt")
    with open(lokasi_path, "w", encoding='utf-8', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["id_lokasi", "nama_lokasi", "tipe"])
        for idx, coord in enumerate(locations):
            id_loc = f"L{str(idx+1).zfill(3)}"
            name = f"Shanghai_{idx+1}"
            writer.writerow([id_loc, name, "Tujuan"])
            
    # 2. Write rute.txt
    rute_path = os.path.join(folder, "rute.txt")
    
    # Generate unique edges
    edges = set()
    num_nodes = len(locations)
    max_possible_edges = num_nodes * (num_nodes - 1)
    target_routes = min(num_routes, max_possible_edges)
    
    # Generate edges
    while len(edges) < target_routes:
        u = random.randint(0, num_nodes - 1)
        v = random.randint(0, num_nodes - 1)
        if u != v:
            edges.add((u, v))
            
    with open(rute_path, "w", encoding='utf-8', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["id_rute", "asal", "tujuan", "jarak"])
        for idx, edge in enumerate(edges):
            id_route = f"R{str(idx+1).zfill(4)}"
            asal = f"L{str(edge[0]+1).zfill(3)}"
            tujuan = f"L{str(edge[1]+1).zfill(3)}"
            dist = calculate_distance(locations[edge[0]], locations[edge[1]])
            writer.writerow([id_route, asal, tujuan, dist])
            
    print(f"   Done: {len(locations)} locations, {target_routes} routes written.")

if __name__ == "__main__":
    random.seed(42)  # For reproducibility
    csv_file = "delivery_sh.csv"
    
    if not os.path.exists(csv_file):
        print(f"❌ Error: {csv_file} not found!")
        exit(1)
        
    # Load 500 unique locations
    locations = load_unique_coordinates(csv_file, 500)
    
    # Define scenarios
    scenarios = [
        # Size based
        {"folder": "data_kecil", "size": 10, "routes": 30},
        {"folder": "data_sedang", "size": 50, "routes": 150},
        {"folder": "data_progress", "size": 100, "routes": 400},
        {"folder": "data_besar", "size": 500, "routes": 2000},
        
        # Density based (using 100 locations)
        {"folder": "density_sparse", "size": 100, "routes": 150},
        {"folder": "density_normal", "size": 100, "routes": 500},
        {"folder": "density_dense", "size": 100, "routes": 3000},
    ]
    
    for s in scenarios:
        sub_locs = locations[:s["size"]]
        generate_scenario(s["folder"], sub_locs, s["routes"])
        
    print("\n🎉 All scenarios generated successfully!")