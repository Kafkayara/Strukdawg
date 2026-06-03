#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <queue>   // Untuk priority_queue (Dijkstra)
#include <limits>  // Untuk numeric_limits
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif
using namespace std;
using namespace chrono;

size_t getMemoryUsage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.PagefileUsage; // Private Bytes
    }
#endif
    return 0;
}

string padZero(int num, int target_len) {
    string s = to_string(num);
    if (s.length() < target_len) {
        return string(target_len - s.length(), '0') + s;
    }
    return s;
}

struct Lokasi {
    string id;
    string nama;
    string tipe;
};

class MatrixManager {
private:
    vector<vector<float>> matriks;
    map<string, int> idKeIndex;
    map<int, string> indexKeId;
    map<string, Lokasi> daftarLokasi;
    int jumlahLokasi = 0;
    int jumlahRute = 0;
    int counterLokasi = 0;
    int counterRute = 0;
    const string FILE_LOKASI = "data/lokasi.txt";
    const string FILE_RUTE = "data/rute.txt";

public:
    MatrixManager() {
        loadData();
    }

    void loadData() {
        idKeIndex.clear();
        indexKeId.clear();
        daftarLokasi.clear();
        jumlahLokasi = 0;
        jumlahRute = 0;
        counterLokasi = 0;
        counterRute = 0;
        
        ifstream fileLokasi(FILE_LOKASI);
        if (fileLokasi.is_open()) {
            string line;
            getline(fileLokasi, line);
            
            int idx = 0;
            while (getline(fileLokasi, line)) {
                if (line.empty()) continue;
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                
                string id = line.substr(0, pos1);
                string nama = line.substr(pos1 + 1, pos2 - pos1 - 1);
                string tipe = line.substr(pos2 + 1);
                
                daftarLokasi[id] = {id, nama, tipe};
                idKeIndex[id] = idx;
                indexKeId[idx] = id;
                idx++;
                
                int num = stoi(id.substr(1));
                if (num > counterLokasi) counterLokasi = num;
            }
            fileLokasi.close();
            jumlahLokasi = idx;
            cout << "Load " << jumlahLokasi << " lokasi\n";
        } else {
            cout << "File " << FILE_LOKASI << " tidak ditemukan!\n";
        }
        
        matriks.assign(jumlahLokasi, vector<float>(jumlahLokasi, 0.0f));
        
        ifstream fileRute(FILE_RUTE);
        if (fileRute.is_open()) {
            string line;
            getline(fileRute, line);
            
            int ruteCount = 0;
            while (getline(fileRute, line)) {
                if (line.empty()) continue;
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                size_t pos3 = line.find(',', pos2 + 1);
                
                string id_rute = line.substr(0, pos1);
                string asal = line.substr(pos1 + 1, pos2 - pos1 - 1);
                string tujuan = line.substr(pos2 + 1, pos3 - pos2 - 1);
                float jarak = stof(line.substr(pos3 + 1));
                
                if (idKeIndex.find(asal) != idKeIndex.end() && 
                    idKeIndex.find(tujuan) != idKeIndex.end()) {
                    int i = idKeIndex[asal];
                    int j = idKeIndex[tujuan];
                    matriks[i][j] = jarak;
                    ruteCount++;
                }
                
                int num = stoi(id_rute.substr(1));
                if (num > counterRute) counterRute = num;
            }
            fileRute.close();
            jumlahRute = ruteCount;
            cout << "Load " << jumlahRute << " rute\n";
        } else {
            cout << "File " << FILE_RUTE << " tidak ditemukan!\n";
        }
        cout << "Memory Usage: " << getMemoryUsage() / 1024.0 << " KB" << endl;
    }

    void saveData() {
        auto start = high_resolution_clock::now();
        
        ofstream fileLokasi(FILE_LOKASI);
        fileLokasi << "id_lokasi,nama_lokasi,tipe\n";
        for (int i = 0; i < jumlahLokasi; i++) {
            string id = indexKeId[i];
            Lokasi& l = daftarLokasi[id];
            fileLokasi << l.id << "," << l.nama << "," << l.tipe << "\n";
        }
        fileLokasi.close();

        ofstream fileRute(FILE_RUTE);
        fileRute << "id_rute,asal,tujuan,jarak\n";
        int ruteCounter = 0;
        for (int i = 0; i < jumlahLokasi; i++) {
            for (int j = 0; j < jumlahLokasi; j++) {
                if (matriks[i][j] > 0) {
                    ruteCounter++;
                    fileRute << "R" << padZero(ruteCounter, 4)
                             << "," << indexKeId[i] << "," << indexKeId[j]
                             << "," << matriks[i][j] << "\n";
                }
            }
        }
        fileRute.close();
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Data berhasil disimpan! " << duration.count() / 1000.0 << " ms\n";
    }

    bool insertLokasi(string nama, string tipe) {
        auto start = high_resolution_clock::now();
        
        counterLokasi++;
        string id = "L" + padZero(counterLokasi, 3);
        daftarLokasi[id] = {id, nama, tipe};
        
        idKeIndex[id] = jumlahLokasi;
        indexKeId[jumlahLokasi] = id;
        
        jumlahLokasi++;
        matriks.resize(jumlahLokasi);
        for (int i = 0; i < jumlahLokasi; i++) {
            matriks[i].resize(jumlahLokasi, 0.0f);
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Lokasi ditambahkan: " << id << " - " << nama << endl;
        cout << "Waktu eksekusi: " << duration.count() / 1000.0 << " ms" << endl;
        return true;
    }

    bool insertRute(string asal, string tujuan, float jarak) {
        auto start = high_resolution_clock::now();
        
        if (idKeIndex.find(asal) == idKeIndex.end()) {
            cout << "Lokasi asal " << asal << " tidak ditemukan!\n";
            return false;
        }
        if (idKeIndex.find(tujuan) == idKeIndex.end()) {
            cout << "Lokasi tujuan " << tujuan << " tidak ditemukan!\n";
            return false;
        }
        
        int i = idKeIndex[asal];
        int j = idKeIndex[tujuan];
        
        if (matriks[i][j] > 0) {
            cout << "⚠️  Rute sudah ada! (Lama: " << matriks[i][j] << " km, Baru: " << jarak << " km)\n";
            return false;
        }
        
        matriks[i][j] = jarak;
        jumlahRute++;
        counterRute++;
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Rute ditambahkan: " << asal << " ke " << tujuan << " = " << jarak << " km\n";
        cout << "Waktu eksekusi: " << duration.count() / 1000.0 << " ms" << endl;
        return true;
    }

    bool updateLokasi(string id, string namaBaru, string tipeBaru) {
        auto start = high_resolution_clock::now();
        if (daftarLokasi.find(id) == daftarLokasi.end()) {
            cout << "Error: Lokasi " << id << " tidak ditemukan!\n";
            return false;
        }
        daftarLokasi[id].nama = namaBaru;
        daftarLokasi[id].tipe = tipeBaru;
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Lokasi " << id << " berhasil diperbarui!\n";
        cout << "Waktu eksekusi: " << duration.count() / 1000.0 << " ms" << endl;
        return true;
    }

    bool updateRute(string asal, string tujuan, float jarakBaru) {
        auto start = high_resolution_clock::now();
        if (idKeIndex.find(asal) == idKeIndex.end() || idKeIndex.find(tujuan) == idKeIndex.end()) {
            cout << "Error: Lokasi asal/tujuan tidak ditemukan!\n";
            return false;
        }
        int i = idKeIndex[asal];
        int j = idKeIndex[tujuan];
        if (matriks[i][j] == 0.0f) {
            cout << "Rute " << asal << " ke " << tujuan << " tidak ditemukan!\n";
            return false;
        }
        matriks[i][j] = jarakBaru;
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Rute " << asal << " ke " << tujuan << " diperbarui menjadi " << jarakBaru << " km!\n";
        cout << "Waktu eksekusi: " << duration.count() / 1000.0 << " ms" << endl;
        return true;
    }

    bool deleteLokasi(string id) {
        auto start = high_resolution_clock::now();
        if (idKeIndex.find(id) == idKeIndex.end()) {
            cout << "Error: Lokasi " << id << " tidak ditemukan!\n";
            return false;
        }
        int delIdx = idKeIndex[id];

        daftarLokasi.erase(id);
        
        matriks.erase(matriks.begin() + delIdx);
        for (auto& row : matriks) {
            row.erase(row.begin() + delIdx);
        }
        
        vector<string> remaining;
        for (int k = 0; k < jumlahLokasi; k++) {
            if (k != delIdx) {
                remaining.push_back(indexKeId[k]);
            }
        }
        
        idKeIndex.clear();
        indexKeId.clear();
        jumlahLokasi = remaining.size();
        for (int k = 0; k < jumlahLokasi; k++) {
            idKeIndex[remaining[k]] = k;
            indexKeId[k] = remaining[k];
        }
        
        // Hitung ulang jumlah rute aktif
        int ruteCount = 0;
        for (int i = 0; i < jumlahLokasi; i++) {
            for (int j = 0; j < jumlahLokasi; j++) {
                if (matriks[i][j] > 0) {
                    ruteCount++;
                }
            }
        }
        jumlahRute = ruteCount;
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Lokasi " << id << " beserta rute terkait berhasil dihapus!\n";
        cout << "Waktu eksekusi: " << duration.count() / 1000.0 << " ms" << endl;
        return true;
    }

    bool deleteRute(string asal, string tujuan) {
        auto start = high_resolution_clock::now();
        if (idKeIndex.find(asal) == idKeIndex.end() || idKeIndex.find(tujuan) == idKeIndex.end()) {
            cout << "Error: Rute tidak ditemukan!\n";
            return false;
        }
        int i = idKeIndex[asal];
        int j = idKeIndex[tujuan];
        if (matriks[i][j] == 0.0f) {
            cout << "Rute " << asal << " ke " << tujuan << " tidak ditemukan!\n";
            return false;
        }
        matriks[i][j] = 0.0f;
        jumlahRute--;
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Rute " << asal << " ke " << tujuan << " berhasil dihapus!\n";
        cout << "Waktu eksekusi: " << duration.count() / 1000.0 << " ms" << endl;
        return true;
    }

    void searchRute(string asal, string tujuan) {
        auto start = high_resolution_clock::now();
        
        if (idKeIndex.find(asal) == idKeIndex.end() || 
            idKeIndex.find(tujuan) == idKeIndex.end()) {
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start);
            cout << "Lokasi tidak ditemukan!\n";
            cout << "Waktu: " << duration.count() / 1000.0 << " ms\n";
            return;
        }
        
        int i = idKeIndex[asal];
        int j = idKeIndex[tujuan];
        float jarak = matriks[i][j];
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        
        if (jarak > 0) {
            cout << "RUTE DITEMUKAN! Jarak: " << jarak << " km\n";
        } else {
            cout << "Rute tidak ditemukan!\n";
        }
        cout << "Waktu: " << duration.count() / 1000.0 << " ms\n";
    }

    void displayAllRute() {
        auto start = high_resolution_clock::now();
        
        cout << "\n=== DAFTAR RUTE ===\n";
        int total = 0;
        for (int i = 0; i < jumlahLokasi; i++) {
            for (int j = 0; j < jumlahLokasi; j++) {
                if (matriks[i][j] > 0) {
                    cout << indexKeId[i] << " ke " << indexKeId[j]
                         << " : " << matriks[i][j] << " km\n";
                    total++;
                }
            }
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        
        cout << "Total: " << total << " rute\n";
        cout << "Waktu tampil: " << duration.count() / 1000.0 << " ms\n";
    }

    void displayAllLokasi() {
        auto start = high_resolution_clock::now();
        
        cout << "\n=== DAFTAR LOKASI ===\n";
        for (int i = 0; i < jumlahLokasi; i++) {
            string id = indexKeId[i];
            cout << id << " | " << daftarLokasi[id].nama << " | Tipe: " << daftarLokasi[id].tipe << "\n";
        }
        cout << "Total: " << jumlahLokasi << " lokasi\n";
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Waktu tampil: " << duration.count() / 1000.0 << " ms\n";
    }

    void cariJalurTerpendek(string asal, string tujuan) {
        auto start = high_resolution_clock::now();

        if (idKeIndex.find(asal) == idKeIndex.end()) {
            cout << "Lokasi asal " << asal << " tidak ditemukan!" << endl;
            auto end = high_resolution_clock::now();
            cout << "Waktu eksekusi: " << duration_cast<microseconds>(end - start).count() / 1000.0 << " ms" << endl;
            return;
        }
        if (idKeIndex.find(tujuan) == idKeIndex.end()) {
            cout << "Lokasi tujuan " << tujuan << " tidak ditemukan!" << endl;
            auto end = high_resolution_clock::now();
            cout << "Waktu eksekusi: " << duration_cast<microseconds>(end - start).count() / 1000.0 << " ms" << endl;
            return;
        }

        int src = idKeIndex[asal];
        int dst = idKeIndex[tujuan];

        // Dijkstra's Algorithm using adjacency matrix
        vector<float> dist(jumlahLokasi, numeric_limits<float>::infinity());
        vector<int> prev(jumlahLokasi, -1);
        dist[src] = 0;

        // Min-heap: (jarak, index)
        priority_queue<pair<float, int>, vector<pair<float, int>>, greater<pair<float, int>>> pq;
        pq.push({0, src});

        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();

            if (d > dist[u]) continue; // skip stale entry
            if (u == dst) break;       // found shortest path

            for (int v = 0; v < jumlahLokasi; v++) {
                if (matriks[u][v] > 0) {
                    float newDist = dist[u] + matriks[u][v];
                    if (newDist < dist[v]) {
                        dist[v] = newDist;
                        prev[v] = u;
                        pq.push({newDist, v});
                    }
                }
            }
        }

        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);

        if (dist[dst] == numeric_limits<float>::infinity()) {
            cout << "Rute dari " << asal << " ke " << tujuan << " tidak ditemukan" << endl;
        } else {
            // Reconstruct path
            vector<int> path;
            int current = dst;
            while (current != src) {
                path.push_back(current);
                current = prev[current];
            }
            path.push_back(src);
            reverse(path.begin(), path.end());

            cout << "RUTE DITEMUKAN!" << endl;
            cout << "   Total Jarak : " << dist[dst] << " km" << endl;
            cout << "   Jumlah Hop  : " << path.size() - 1 << endl;
            cout << "   Jalur       : ";
            for (size_t i = 0; i < path.size(); i++) {
                string id = indexKeId[path[i]];
                if (daftarLokasi.count(id))
                    cout << id << " (" << daftarLokasi[id].nama << ")";
                else
                    cout << id;
                if (i < path.size() - 1) cout << " ke ";
            }
            cout << endl;

            // Detail per-hop
            cout << "\n   Detail per segmen:" << endl;
            for (size_t i = 0; i < path.size() - 1; i++) {
                string fromId = indexKeId[path[i]];
                string toId = indexKeId[path[i+1]];
                float segDist = matriks[path[i]][path[i+1]];
                cout << "   " << (i+1) << ". " << fromId << " ke " << toId << " = " << segDist << " km" << endl;
            }
        }
        cout << "Waktu eksekusi: " << duration.count() / 1000.0 << " ms" << endl;
        cout << "Memory Usage: " << getMemoryUsage() / 1024.0 << " KB" << endl;
    }
};

int main() {
    cout << "\n========================================\n";
    cout << "SISTEM MANAJEMEN RUTE (ADJACENCY MATRIX)\n";
    cout << "========================================\n";
    
    MatrixManager manager;
    
    int pilihan;
    do {
        cout << "\n================ MENU ================\n";
        cout << "1. Insert Lokasi Baru\n";
        cout << "2. Insert Rute Baru\n";
        cout << "3. Update Lokasi\n";
        cout << "4. Update Rute\n";
        cout << "5. Delete Lokasi\n";
        cout << "6. Delete Rute\n";
        cout << "7. Cari Rute (Dijkstra)\n";
        cout << "8. Tampil Semua Rute\n";
        cout << "9. Tampil Semua Lokasi\n";
        cout << "10. Simpan & Keluar\n";
        cout << "========================================\n";
        cout << "Pilihan [1-10]: ";
        cin >> pilihan;
        
        switch(pilihan) {
            case 1: {
                string nama, tipe;
                cout << "Nama Lokasi: ";
                cin.ignore();
                getline(cin, nama);
                cout << "Tipe (Gudang/Tujuan): ";
                getline(cin, tipe);
                manager.insertLokasi(nama, tipe);
                break;
            }
            case 2: {
                string asal, tujuan;
                float jarak;
                cout << "Asal: ";
                cin >> asal;
                cout << "Tujuan: ";
                cin >> tujuan;
                cout << "Jarak (km): ";
                cin >> jarak;
                manager.insertRute(asal, tujuan, jarak);
                break;
            }
            case 3: {
                string id, namaBaru, tipeBaru;
                cout << "ID Lokasi yang di-update: ";
                cin >> id;
                cout << "Nama Baru: ";
                cin.ignore();
                getline(cin, namaBaru);
                cout << "Tipe Baru: ";
                getline(cin, tipeBaru);
                manager.updateLokasi(id, namaBaru, tipeBaru);
                break;
            }
            case 4: {
                string asal, tujuan;
                float jarakBaru;
                cout << "Asal: ";
                cin >> asal;
                cout << "Tujuan: ";
                cin >> tujuan;
                cout << "Jarak Baru (km): ";
                cin >> jarakBaru;
                manager.updateRute(asal, tujuan, jarakBaru);
                break;
            }
            case 5: {
                string id;
                cout << "ID Lokasi yang dihapus: ";
                cin >> id;
                manager.deleteLokasi(id);
                break;
            }
            case 6: {
                string asal, tujuan;
                cout << "Asal: ";
                cin >> asal;
                cout << "Tujuan: ";
                cin >> tujuan;
                manager.deleteRute(asal, tujuan);
                break;
            }
            case 7: {
                string asal, tujuan;
                cout << "Asal: ";
                cin >> asal;
                cout << "Tujuan: ";
                cin >> tujuan;
                manager.cariJalurTerpendek(asal, tujuan);
                break;
            }
            case 8:
                manager.displayAllRute();
                break;
            case 9:
                manager.displayAllLokasi();
                break;
            case 10:
                manager.saveData();
                cout << "Terima kasih!\n";
                break;
            default:
                cout << "Pilihan tidak valid!\n";
        }
    } while(pilihan != 10);
    
    return 0;
}