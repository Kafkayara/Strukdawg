#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <iomanip>
using namespace std;
using namespace chrono;

class MatrixManager {
private:
    vector<vector<float>> matriks;
    map<string, int> idKeIndex;
    map<int, string> indexKeId;
    int jumlahLokasi = 0;
    int jumlahRute = 0;
    const string FILE_LOKASI = "data/lokasi.txt";
    const string FILE_RUTE = "data/rute.txt";

public:
    MatrixManager() {
        loadData();
    }

    void loadData() {
        idKeIndex.clear();
        indexKeId.clear();
        
        ifstream fileLokasi(FILE_LOKASI);
        if (!fileLokasi.is_open()) {
            cout << "File " << FILE_LOKASI << " tidak ditemukan!\n";
            return;
        }
        
        string line;
        getline(fileLokasi, line);
        
        int idx = 0;
        while (getline(fileLokasi, line)) {
            size_t pos1 = line.find(',');
            string id = line.substr(0, pos1);
            idKeIndex[id] = idx;
            indexKeId[idx] = id;
            idx++;
        }
        fileLokasi.close();
        
        jumlahLokasi = idx;
        matriks.assign(jumlahLokasi, vector<float>(jumlahLokasi, 0.0f));
        
        ifstream fileRute(FILE_RUTE);
        if (!fileRute.is_open()) {
            cout << "File " << FILE_RUTE << " tidak ditemukan!\n";
            return;
        }
        
        getline(fileRute, line);
        
        int ruteCount = 0;
        while (getline(fileRute, line)) {
            size_t pos1 = line.find(',');
            size_t pos2 = line.find(',', pos1 + 1);
            size_t pos3 = line.find(',', pos2 + 1);
            
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
        }
        fileRute.close();
        jumlahRute = ruteCount;
        
        cout << "Load: " << jumlahLokasi << " lokasi, " << jumlahRute << " rute\n";
    }

    void saveData() {
        auto start = high_resolution_clock::now();
        
        // Simpan lokasi (overwrite)
        ofstream fileLokasi(FILE_LOKASI);
        fileLokasi << "id_lokasi,nama_lokasi,tipe\n";
        for (int i = 0; i < jumlahLokasi; i++) {
            fileLokasi << indexKeId[i] << ",Lokasi_" << (i+1) << ",Tujuan\n";
        }
        fileLokasi.close();

        // Simpan rute (overwrite)
        ofstream fileRute(FILE_RUTE);
        fileRute << "id_rute,asal,tujuan,jarak\n";
        int ruteCounter = 0;
        for (int i = 0; i < jumlahLokasi; i++) {
            for (int j = 0; j < jumlahLokasi; j++) {
                if (matriks[i][j] > 0) {
                    ruteCounter++;
                    fileRute << "R" << setw(4) << setfill('0') << ruteCounter
                             << "," << indexKeId[i] << "," << indexKeId[j]
                             << "," << matriks[i][j] << "\n";
                }
            }
        }
        fileRute.close();
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "✅ Data berhasil disimpan! ⏱️ " << duration.count() / 1000.0 << " ms\n";
    }

    void searchRute(string asal, string tujuan) {
        auto start = high_resolution_clock::now();
        
        if (idKeIndex.find(asal) == idKeIndex.end() || 
            idKeIndex.find(tujuan) == idKeIndex.end()) {
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start);
            cout << "❌ Lokasi tidak ditemukan!\n";
            cout << "⏱️  Waktu: " << duration.count() / 1000.0 << " ms\n";
            return;
        }
        
        int i = idKeIndex[asal];
        int j = idKeIndex[tujuan];
        float jarak = matriks[i][j];
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        
        if (jarak > 0) {
            cout << "✅ RUTE DITEMUKAN! Jarak: " << jarak << " km\n";
        } else {
            cout << "❌ Rute tidak ditemukan!\n";
        }
        cout << "⏱️  Waktu: " << duration.count() / 1000.0 << " ms\n";
    }

    // =========================================================
    // FITUR INSERT RUTE (BARU!)
    // =========================================================
    void insertRute(string asal, string tujuan, float jarak) {
        auto start = high_resolution_clock::now();
        
        // Cek apakah lokasi asal dan tujuan valid
        if (idKeIndex.find(asal) == idKeIndex.end()) {
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start);
            cout << "❌ Lokasi asal " << asal << " tidak ditemukan!\n";
            cout << "⏱️  Waktu: " << duration.count() / 1000.0 << " ms\n";
            return;
        }
        
        if (idKeIndex.find(tujuan) == idKeIndex.end()) {
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start);
            cout << "❌ Lokasi tujuan " << tujuan << " tidak ditemukan!\n";
            cout << "⏱️  Waktu: " << duration.count() / 1000.0 << " ms\n";
            return;
        }
        
        int i = idKeIndex[asal];
        int j = idKeIndex[tujuan];
        
        // Cek apakah rute sudah ada
        if (matriks[i][j] > 0) {
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start);
            cout << "⚠️  Rute sudah ada! (Lama: " << matriks[i][j] << " km, Baru: " << jarak << " km)\n";
            cout << "⏱️  Waktu: " << duration.count() / 1000.0 << " ms\n";
            return;
        }
        
        // Tambah rute baru
        matriks[i][j] = jarak;
        jumlahRute++;
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        
        cout << "✅ Rute ditambahkan: " << asal << " → " << tujuan << " = " << jarak << " km\n";
        cout << "⏱️  Waktu: " << duration.count() / 1000.0 << " ms\n";
    }

    void displayAllRute() {
        auto start = high_resolution_clock::now();
        
        cout << "\n=== DAFTAR RUTE ===\n";
        int total = 0;
        for (int i = 0; i < jumlahLokasi; i++) {
            for (int j = 0; j < jumlahLokasi; j++) {
                if (matriks[i][j] > 0) {
                    cout << indexKeId[i] << " → " << indexKeId[j] 
                         << " : " << matriks[i][j] << " km\n";
                    total++;
                }
            }
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        
        cout << "Total: " << total << " rute\n";
        cout << "⏱️  Waktu tampil: " << duration.count() / 1000.0 << " ms\n";
    }

    void displayAllLokasi() {
        auto start = high_resolution_clock::now();
        
        cout << "\n=== DAFTAR LOKASI ===\n";
        for (int i = 0; i < jumlahLokasi; i++) {
            cout << indexKeId[i] << " | Lokasi_" << (i+1) << "\n";
        }
        cout << "Total: " << jumlahLokasi << " lokasi\n";
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "⏱️  Waktu tampil: " << duration.count() / 1000.0 << " ms\n";
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
        cout << "1. Cari Rute\n";
        cout << "2. Insert Rute (BARU!)\n";
        cout << "3. Tampil Semua Rute\n";
        cout << "4. Tampil Semua Lokasi\n";
        cout << "5. Simpan & Keluar\n";
        cout << "========================================\n";
        cout << "Pilihan: ";
        cin >> pilihan;
        
        switch(pilihan) {
            case 1: {
                string asal, tujuan;
                cout << "Asal: ";
                cin >> asal;
                cout << "Tujuan: ";
                cin >> tujuan;
                manager.searchRute(asal, tujuan);
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
            case 3:
                manager.displayAllRute();
                break;
            case 4:
                manager.displayAllLokasi();
                break;
            case 5:
                manager.saveData();
                cout << "Terima kasih!\n";
                break;
            default:
                cout << "Pilihan tidak valid!\n";
        }
    } while(pilihan != 5);
    
    return 0;
}