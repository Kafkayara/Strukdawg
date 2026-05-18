#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>  // Untuk stopwatch
using namespace std;
using namespace chrono;

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
            cout << "✅ Load " << daftarLokasi.size() << " lokasi" << endl;
        } else {
            cout << "❌ File " << FILE_LOKASI << " tidak ditemukan!" << endl;
        }

        ifstream fileRute(FILE_RUTE);
        if (fileRute.is_open()) {
            string line;
            getline(fileRute, line);
            
            int sukses = 0;
            
            while (getline(fileRute, line)) {
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
            cout << "✅ Load " << sukses << " rute" << endl;
        } else {
            cout << "❌ File " << FILE_RUTE << " tidak ditemukan!" << endl;
        }
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
        cout << "✅ Data berhasil disimpan!\n";
    }

    // =========================================================
    // INSERT LOKASI (dengan timer)
    // =========================================================
    bool insertLokasi(string nama, string tipe) {
        auto start = high_resolution_clock::now();  // Start timer
        
        counterLokasi++;
        string id = "L" + string(3 - to_string(counterLokasi).length(), '0') + to_string(counterLokasi);
        daftarLokasi[id] = {id, nama, tipe};
        daftarRute[id] = vector<Rute>();
        
        auto end = high_resolution_clock::now();    // End timer
        auto duration = duration_cast<microseconds>(end - start);
        
        cout << "✅ Lokasi ditambahkan: " << id << " - " << nama << endl;
        cout << "⏱️  Waktu eksekusi: " << duration.count() / 1000.0 << " ms" << endl;
        return true;
    }

    // =========================================================
    // INSERT RUTE (dengan timer)
    // =========================================================
    bool insertRute(string asal, string tujuan, float jarak) {
        auto start = high_resolution_clock::now();  // Start timer
        
        if (daftarLokasi.find(asal) == daftarLokasi.end()) {
            cout << "❌ Error: Lokasi asal " << asal << " tidak ditemukan!\n";
            return false;
        }
        if (daftarLokasi.find(tujuan) == daftarLokasi.end()) {
            cout << "❌ Error: Lokasi tujuan " << tujuan << " tidak ditemukan!\n";
            return false;
        }
        
        counterRute++;
        string id_rute = "R" + string(4 - to_string(counterRute).length(), '0') + to_string(counterRute);
        daftarRute[asal].push_back(Rute(id_rute, tujuan, jarak));
        
        auto end = high_resolution_clock::now();    // End timer
        auto duration = duration_cast<microseconds>(end - start);
        
        cout << "✅ Rute ditambahkan: " << id_rute << " (" << asal << " → " << tujuan << " = " << jarak << " km)" << endl;
        cout << "⏱️  Waktu eksekusi: " << duration.count() / 1000.0 << " ms" << endl;
        return true;
    }

    // =========================================================
    // SEARCH RUTE (dengan timer)
    // =========================================================
    void searchRute(string asal, string tujuan) {
        auto start = high_resolution_clock::now();  // Start timer
        
        if (daftarRute.find(asal) == daftarRute.end()) {
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start);
            cout << "❌ Lokasi asal " << asal << " tidak memiliki rute" << endl;
            cout << "⏱️  Waktu eksekusi: " << duration.count() / 1000.0 << " ms" << endl;
            return;
        }
        
        bool ditemukan = false;
        for (auto& rute : daftarRute[asal]) {
            if (rute.tujuan == tujuan) {
                ditemukan = true;
                auto end = high_resolution_clock::now();
                auto duration = duration_cast<microseconds>(end - start);
                
                cout << "✅ RUTE DITEMUKAN!" << endl;
                cout << "   ID Rute : " << rute.id_rute << endl;
                cout << "   Asal    : " << asal << endl;
                cout << "   Tujuan  : " << tujuan << endl;
                cout << "   Jarak   : " << rute.jarak << " km" << endl;
                cout << "⏱️  Waktu eksekusi: " << duration.count() / 1000.0 << " ms" << endl;
                return;
            }
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        
        cout << "❌ Rute dari " << asal << " ke " << tujuan << " tidak ditemukan" << endl;
        cout << "⏱️  Waktu eksekusi: " << duration.count() / 1000.0 << " ms" << endl;
    }

    void displayAllRute() {
        cout << "\n" << string(50, '=') << endl;
        cout << "           DAFTAR SEMUA RUTE" << endl;
        cout << string(50, '=') << endl;
        
        int total = 0;
        for (auto& pair : daftarRute) {
            string asal = pair.first;
            for (auto& rute : pair.second) {
                cout << asal << " → " << rute.tujuan << " : " << rute.jarak << " km (ID: " << rute.id_rute << ")\n";
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
};

// =========================================================
// FUNGSI MAIN - MENU UTAMA
// =========================================================
int main() {
    cout << "\n" << string(60, '=') << endl;
    cout << "   SISTEM MANAJEMEN RUTE DISTRIBUSI" << endl;
    cout << string(60, '=') << endl;
    
    RuteManager manager;
    
    int pilihan;
    do {
        cout << "\n" << string(40, '-') << endl;
        cout << "MENU UTAMA:\n";
        cout << "1. Insert Lokasi Baru\n";
        cout << "2. Insert Rute Baru\n";
        cout << "3. Cari Rute\n";
        cout << "4. Tampilkan Semua Rute\n";
        cout << "5. Tampilkan Semua Lokasi\n";
        cout << "6. Simpan & Keluar\n";
        cout << string(40, '-') << endl;
        cout << "Pilihan [1-6]: ";
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
                string asal, tujuan;
                cout << "ID Lokasi Asal: ";
                cin >> asal;
                cout << "ID Lokasi Tujuan: ";
                cin >> tujuan;
                manager.searchRute(asal, tujuan);
                break;
            }
            case 4:
                manager.displayAllRute();
                break;
            case 5:
                manager.displayAllLokasi();
                break;
            case 6:
                manager.saveData();
                cout << "Terima kasih! Program selesai.\n";
                break;
            default:
                cout << "Pilihan tidak valid! Coba lagi.\n";
        }
    } while(pilihan != 6);

    return 0;
}