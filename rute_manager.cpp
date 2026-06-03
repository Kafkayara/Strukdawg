#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>  // Untuk stopwatch
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

struct Rute {
    string id_rute;
    string tujuan;
    float jarak;
    Rute(string id, string tuj, float j) : id_rute(id), tujuan(tuj), jarak(j) {}
};

struct Lokasi {
    string id;
    string nama;
    string tipe;
};

class RuteManager {
private:
    map<string, vector<Rute>> daftarRute;
    map<string, Lokasi> daftarLokasi;
    int counterLokasi = 0;
    int counterRute = 0;
    const string FILE_LOKASI = "data/lokasi.txt";
    const string FILE_RUTE = "data/rute.txt";

public:
    RuteManager() {
        loadData();
    }

    void loadData() {
        daftarLokasi.clear();
        daftarRute.clear();
        counterLokasi = 0;
        counterRute = 0;

        ifstream fileLokasi(FILE_LOKASI);
        if (fileLokasi.is_open()) {
            string line;
            getline(fileLokasi, line);
            
            while (getline(fileLokasi, line)) {
                if (line.empty()) continue;
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                
                string id = line.substr(0, pos1);
                string nama = line.substr(pos1 + 1, pos2 - pos1 - 1);
                string tipe = line.substr(pos2 + 1);
                
                daftarLokasi[id] = {id, nama, tipe};
                daftarRute[id] = vector<Rute>();
                
                int num = stoi(id.substr(1));
                if (num > counterLokasi) counterLokasi = num;
            }
            fileLokasi.close();
            cout << "Load " << daftarLokasi.size() << " lokasi" << endl;
        } else {
            cout << "File " << FILE_LOKASI << " tidak ditemukan!" << endl;
        }

        ifstream fileRute(FILE_RUTE);
        if (fileRute.is_open()) {
            string line;
            getline(fileRute, line);
            
            int sukses = 0;
            
            while (getline(fileRute, line)) {
                if (line.empty()) continue;
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                size_t pos3 = line.find(',', pos2 + 1);
                
                string id_rute = line.substr(0, pos1);
                string asal = line.substr(pos1 + 1, pos2 - pos1 - 1);
                string tujuan = line.substr(pos2 + 1, pos3 - pos2 - 1);
                float jarak = stof(line.substr(pos3 + 1));
                
                if (daftarLokasi.find(asal) != daftarLokasi.end() && 
                    daftarLokasi.find(tujuan) != daftarLokasi.end()) {
                    daftarRute[asal].push_back(Rute(id_rute, tujuan, jarak));
                    sukses++;
                }
                
                int num = stoi(id_rute.substr(1));
                if (num > counterRute) counterRute = num;
            }
            fileRute.close();
            cout << "Load " << sukses << " rute" << endl;
        } else {
            cout << "File " << FILE_RUTE << " tidak ditemukan!" << endl;
        }
        cout << "Memory Usage: " << getMemoryUsage() / 1024.0 << " KB" << endl;
    }

    void saveData() {
        ofstream fileLokasi(FILE_LOKASI);
        fileLokasi << "id_lokasi,nama_lokasi,tipe\n";
        for (auto& pair : daftarLokasi) {
            Lokasi& l = pair.second;
            fileLokasi << l.id << "," << l.nama << "," << l.tipe << endl;
        }
        fileLokasi.close();

        ofstream fileRute(FILE_RUTE);
        fileRute << "id_rute,asal,tujuan,jarak\n";
        for (auto& pair : daftarRute) {
            string asal = pair.first;
            for (auto& rute : pair.second) {
                fileRute << rute.id_rute << "," << asal << "," << rute.tujuan << "," << rute.jarak << endl;
            }
        }
        fileRute.close();
        cout << "Data berhasil disimpan!\n";
    }

    bool insertLokasi(string nama, string tipe) {
        auto start = high_resolution_clock::now();
        
        counterLokasi++;
        string id = "L" + padZero(counterLokasi, 3);
        daftarLokasi[id] = {id, nama, tipe};
        daftarRute[id] = vector<Rute>();
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        
        cout << "Lokasi ditambahkan: " << id << " - " << nama << endl;
        cout << "Waktu eksekusi: " << duration.count() / 1000.0 << " ms" << endl;
        return true;
    }

    bool insertRute(string asal, string tujuan, float jarak) {
        auto start = high_resolution_clock::now();
        
        if (daftarLokasi.find(asal) == daftarLokasi.end()) {
            cout << "Error: Lokasi asal " << asal << " tidak ditemukan!\n";
            return false;
        }
        if (daftarLokasi.find(tujuan) == daftarLokasi.end()) {
            cout << "Error: Lokasi tujuan " << tujuan << " tidak ditemukan!\n";
            return false;
        }
        
        for (auto& rute : daftarRute[asal]) {
            if (rute.tujuan == tujuan) {
                cout << "Rute sudah ada! (Lama: " << rute.jarak << " km, Baru: " << jarak << " km)\n";
                return false;
            }
        }
        
        counterRute++;
        string id_rute = "R" + padZero(counterRute, 4);
        daftarRute[asal].push_back(Rute(id_rute, tujuan, jarak));
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        
        cout << "Rute ditambahkan: " << id_rute << " (" << asal << " ke " << tujuan << " = " << jarak << " km)" << endl;
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
        if (daftarLokasi.find(asal) == daftarLokasi.end() || daftarLokasi.find(tujuan) == daftarLokasi.end()) {
            cout << "Error: Lokasi asal/tujuan tidak ditemukan!\n";
            return false;
        }
        bool ditemukan = false;
        for (auto& rute : daftarRute[asal]) {
            if (rute.tujuan == tujuan) {
                rute.jarak = jarakBaru;
                ditemukan = true;
                break;
            }
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        if (ditemukan) {
            cout << "Rute " << asal << " ke " << tujuan << " diperbarui menjadi " << jarakBaru << " km!\n";
        } else {
            cout << "Rute " << asal << " ke " << tujuan << " tidak ditemukan!\n";
        }
        cout << "Waktu eksekusi: " << duration.count() / 1000.0 << " ms" << endl;
        return ditemukan;
    }

    bool deleteLokasi(string id) {
        auto start = high_resolution_clock::now();
        if (daftarLokasi.find(id) == daftarLokasi.end()) {
            cout << "Error: Lokasi " << id << " tidak ditemukan!\n";
            return false;
        }
        daftarLokasi.erase(id);
        daftarRute.erase(id);
        for (auto& pair : daftarRute) {
            auto& v = pair.second;
            v.erase(remove_if(v.begin(), v.end(), [&](const Rute& r) {
                return r.tujuan == id;
            }), v.end());
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Lokasi " << id << " beserta rute terkait berhasil dihapus!\n";
        cout << "Waktu eksekusi: " << duration.count() / 1000.0 << " ms" << endl;
        return true;
    }

    bool deleteRute(string asal, string tujuan) {
        auto start = high_resolution_clock::now();
        if (daftarRute.find(asal) == daftarRute.end()) {
            cout << "Error: Rute tidak ditemukan!\n";
            return false;
        }
        auto& v = daftarRute[asal];
        auto it = remove_if(v.begin(), v.end(), [&](const Rute& r) {
            return r.tujuan == tujuan;
        });
        bool ditemukan = (it != v.end());
        if (ditemukan) {
            v.erase(it, v.end());
            cout << "Rute " << asal << " ke " << tujuan << " berhasil dihapus!\n";
        } else {
            cout << "Rute " << asal << " ke " << tujuan << " tidak ditemukan!\n";
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Waktu eksekusi: " << duration.count() / 1000.0 << " ms" << endl;
        return ditemukan;
    }

    void searchRute(string asal, string tujuan) {
        auto start = high_resolution_clock::now();
        
        if (daftarRute.find(asal) == daftarRute.end()) {
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start);
            cout << "Lokasi asal " << asal << " tidak memiliki rute" << endl;
            cout << "Waktu eksekusi: " << duration.count() / 1000.0 << " ms" << endl;
            return;
        }
        
        bool ditemukan = false;
        for (auto& rute : daftarRute[asal]) {
            if (rute.tujuan == tujuan) {
                ditemukan = true;
                auto end = high_resolution_clock::now();
                auto duration = duration_cast<microseconds>(end - start);
                
                cout << "RUTE DITEMUKAN!" << endl;
                cout << "   ID Rute : " << rute.id_rute << endl;
                cout << "   Asal    : " << asal << endl;
                cout << "   Tujuan  : " << tujuan << endl;
                cout << "   Jarak   : " << rute.jarak << " km" << endl;
                cout << "Waktu eksekusi: " << duration.count() / 1000.0 << " ms" << endl;
                return;
            }
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        
        cout << "Rute dari " << asal << " ke " << tujuan << " tidak ditemukan" << endl;
        cout << "Waktu eksekusi: " << duration.count() / 1000.0 << " ms" << endl;
    }

    void displayAllRute() {
        cout << "\n" << string(50, '=') << endl;
        cout << "           DAFTAR SEMUA RUTE" << endl;
        cout << string(50, '=') << endl;
        
        int total = 0;
        for (auto& pair : daftarRute) {
            string asal = pair.first;
            for (auto& rute : pair.second) {
                cout << asal << " ke " << rute.tujuan << " : " << rute.jarak << " km (ID: " << rute.id_rute << ")\n";
                total++;
            }
        }
        cout << string(50, '-') << endl;
        cout << "Total: " << total << " rute\n";
    }

    void displayAllLokasi() {
        cout << "\n" << string(50, '=') << endl;
        cout << "           DAFTAR SEMUA LOKASI" << endl;
        cout << string(50, '=') << endl;
        
        for (auto& pair : daftarLokasi) {
            Lokasi& l = pair.second;
            cout << l.id << " | " << l.nama << " | Tipe: " << l.tipe << endl;
        }
        cout << string(50, '-') << endl;
        cout << "Total: " << daftarLokasi.size() << " lokasi\n";
    }

    int totalRute() {
        int total = 0;
        for (auto& pair : daftarRute) {
            total += pair.second.size();
        }
        return total;
    }

    void cariJalurTerpendek(string asal, string tujuan) {
        auto start = high_resolution_clock::now();

        if (daftarLokasi.find(asal) == daftarLokasi.end()) {
            cout << "Lokasi asal " << asal << " tidak ditemukan!" << endl;
            auto end = high_resolution_clock::now();
            cout << "Waktu eksekusi: " << duration_cast<microseconds>(end - start).count() / 1000.0 << " ms" << endl;
            return;
        }
        if (daftarLokasi.find(tujuan) == daftarLokasi.end()) {
            cout << "Lokasi tujuan " << tujuan << " tidak ditemukan!" << endl;
            auto end = high_resolution_clock::now();
            cout << "Waktu eksekusi: " << duration_cast<microseconds>(end - start).count() / 1000.0 << " ms" << endl;
            return;
        }

        // Dijkstra's Algorithm
        map<string, float> dist;
        map<string, string> prev;
        for (auto& p : daftarLokasi) {
            dist[p.first] = numeric_limits<float>::infinity();
        }
        dist[asal] = 0;

        // Min-heap: (jarak, id_lokasi)
        priority_queue<pair<float, string>, vector<pair<float, string>>, greater<pair<float, string>>> pq;
        pq.push({0, asal});

        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();

            if (d > dist[u]) continue; // skip stale entry
            if (u == tujuan) break;    // found shortest path

            for (auto& rute : daftarRute[u]) {
                float newDist = dist[u] + rute.jarak;
                if (newDist < dist[rute.tujuan]) {
                    dist[rute.tujuan] = newDist;
                    prev[rute.tujuan] = u;
                    pq.push({newDist, rute.tujuan});
                }
            }
        }

        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);

        if (dist[tujuan] == numeric_limits<float>::infinity()) {
            cout << "Rute dari " << asal << " ke " << tujuan << " tidak ditemukan" << endl;
        } else {
            // Reconstruct path
            vector<string> path;
            string current = tujuan;
            while (current != asal) {
                path.push_back(current);
                current = prev[current];
            }
            path.push_back(asal);
            reverse(path.begin(), path.end());

            cout << "RUTE DITEMUKAN!" << endl;
            cout << "   Total Jarak : " << dist[tujuan] << " km" << endl;
            cout << "   Jumlah Hop  : " << path.size() - 1 << endl;
            cout << "   Jalur       : ";
            for (size_t i = 0; i < path.size(); i++) {
                if (daftarLokasi.count(path[i]))
                    cout << path[i] << " (" << daftarLokasi[path[i]].nama << ")";
                else
                    cout << path[i];
                if (i < path.size() - 1) cout << " ke ";
            }
            cout << endl;

            // Detail per-hop
            cout << "\n   Detail per segmen:" << endl;
            for (size_t i = 0; i < path.size() - 1; i++) {
                float segDist = 0;
                for (auto& r : daftarRute[path[i]]) {
                    if (r.tujuan == path[i+1]) { segDist = r.jarak; break; }
                }
                cout << "   " << (i+1) << ". " << path[i] << " ke " << path[i+1] << " = " << segDist << " km" << endl;
            }
        }
        cout << "Waktu eksekusi: " << duration.count() / 1000.0 << " ms" << endl;
        cout << "Memory Usage: " << getMemoryUsage() / 1024.0 << " KB" << endl;
    }
};

// =========================================================
// FUNGSI MAIN - MENU UTAMA
// =========================================================
int main() {
    cout << "\n" << string(60, '=') << endl;
    cout << "   SISTEM MANAJEMEN RUTE DISTRIBUSI (ADJACENCY LIST)" << endl;
    cout << string(60, '=') << endl;
    
    RuteManager manager;
    
    int pilihan;
    do {
        cout << "\n" << string(40, '-') << endl;
        cout << "MENU UTAMA:\n";
        cout << "1. Insert Lokasi Baru\n";
        cout << "2. Insert Rute Baru\n";
        cout << "3. Update Lokasi\n";
        cout << "4. Update Rute\n";
        cout << "5. Delete Lokasi\n";
        cout << "6. Delete Rute\n";
        cout << "7. Cari Rute (Dijkstra)\n";
        cout << "8. Tampilkan Semua Rute\n";
        cout << "9. Tampilkan Semua Lokasi\n";
        cout << "10. Simpan & Keluar\n";
        cout << string(40, '-') << endl;
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
                cout << "ID Lokasi Asal: ";
                cin >> asal;
                cout << "ID Lokasi Tujuan: ";
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
                cout << "ID Lokasi Asal: ";
                cin >> asal;
                cout << "ID Lokasi Tujuan: ";
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
                cout << "ID Lokasi Asal: ";
                cin >> asal;
                cout << "ID Lokasi Tujuan: ";
                cin >> tujuan;
                manager.deleteRute(asal, tujuan);
                break;
            }
            case 7: {
                string asal, tujuan;
                cout << "ID Lokasi Asal: ";
                cin >> asal;
                cout << "ID Lokasi Tujuan: ";
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
                cout << "Terima kasih! Program selesai.\n";
                break;
            default:
                cout << "Pilihan tidak valid! Coba lagi.\n";
        }
    } while(pilihan != 10);

    return 0;
}