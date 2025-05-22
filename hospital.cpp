#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <limits>
#include <iomanip>

class Hospital {
private:
    int id;
    std::string name;
    std::string address;
    std::string phone;

public:
    Hospital(int id, const std::string& name, const std::string& address, const std::string& phone)
        : id(id), name(name), address(address), phone(phone) {}

    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getAddress() const { return address; }
    std::string getPhone() const { return phone; }

    void setName(const std::string& newName) { name = newName; }
    void setAddress(const std::string& newAddress) { address = newAddress; }
    void setPhone(const std::string& newPhone) { phone = newPhone; }
};

class HospitalManager {
private:
    std::vector<Hospital> hospitals;
    std::map<std::pair<int, int>, double> distances;

    void sortPair(int& a, int& b) {
        if (a > b) std::swap(a, b);
    }

    bool phoneExists(const std::string& phone, int excludeId = -1) const {
        return std::any_of(hospitals.begin(), hospitals.end(),
            [&](const Hospital& h) { return h.getId() != excludeId && h.getPhone() == phone; });
    }

    bool idExists(int id) const {
        return std::any_of(hospitals.begin(), hospitals.end(),
            [id](const Hospital& h) { return h.getId() == id; });
    }

public:
    bool addHospital(const Hospital& hospital) {
        if (idExists(hospital.getId()) || phoneExists(hospital.getPhone())) {
            return false;
        }
        hospitals.push_back(hospital);
        return true;
    }

    Hospital* findHospital(int id) {
        auto it = std::find_if(hospitals.begin(), hospitals.end(),
                            [id](const Hospital& h) { return h.getId() == id; });
        return it != hospitals.end() ? &*it : nullptr;
    }

    bool updateHospital(int id, const std::string& name, const std::string& address, const std::string& phone) {
        Hospital* h = findHospital(id);
        if (h && !phoneExists(phone, id)) {
            h->setName(name);
            h->setAddress(address);
            h->setPhone(phone);
            return true;
        }
        return false;
    }

    bool deleteHospital(int id) {
        auto it = std::remove_if(hospitals.begin(), hospitals.end(),
                                [id](const Hospital& h) { return h.getId() == id; });
        if (it != hospitals.end()) {
            hospitals.erase(it, hospitals.end());
            // Remove related distances
            auto distIt = distances.begin();
            while (distIt != distances.end()) {
                if (distIt->first.first == id || distIt->first.second == id) {
                    distIt = distances.erase(distIt);
                } else {
                    ++distIt;
                }
            }
            return true;
        }
        return false;
    }

    bool addDistance(int id1, int id2, double distance) {
        if (id1 == id2 || !findHospital(id1) || !findHospital(id2)) return false;
        sortPair(id1, id2);
        distances[{id1, id2}] = distance;
        return true;
    }

    bool deleteDistance(int id1, int id2) {
        sortPair(id1, id2);
        auto it = distances.find({id1, id2});
        if (it != distances.end()) {
            distances.erase(it);
            return true;
        }
        return false;
    }

    std::string getDistance(int id1, int id2) {
        sortPair(id1, id2);
        auto it = distances.find({id1, id2});
        return it != distances.end() ? std::to_string(it->second) + " km" : "N/A";
    }

    void loadFromCSV() {
        hospitals.clear();
        distances.clear();

        // Load hospitals
        std::ifstream hospFile("hospitals.csv");
        std::string line;
        while (std::getline(hospFile, line)) {
            std::stringstream ss(line);
            std::vector<std::string> tokens;
            std::string token;
            
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() == 4) {
                try {
                    int id = std::stoi(tokens[0]);
                    std::string phone = tokens[3];
                    if (!idExists(id) && !phoneExists(phone)) {
                        hospitals.emplace_back(id, tokens[1], tokens[2], phone);
                    }
                } catch (...) {
                    // Invalid entry, skip
                }
            }
        }

        // Load distances
        std::ifstream distFile("distances.csv");
        while (std::getline(distFile, line)) {
            std::stringstream ss(line);
            std::vector<std::string> tokens;
            std::string token;
            
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() == 3) {
                try {
                    int id1 = std::stoi(tokens[0]);
                    int id2 = std::stoi(tokens[1]);
                    double distance = std::stod(tokens[2]);
                    if (findHospital(id1) && findHospital(id2)) {
                        addDistance(id1, id2, distance);
                    }
                } catch (...) {
                    // Invalid entry, skip
                }
            }
        }
    }

    void saveToCSV() const {
        std::ofstream hospFile("hospitals.csv");
        for (const auto& h : hospitals) {
            hospFile << h.getId() << "," << h.getName() << ","
                    << h.getAddress() << "," << h.getPhone() << "\n";
        }

        std::ofstream distFile("distances.csv");
        for (const auto& d : distances) {
            distFile << d.first.first << "," << d.first.second << ","
                    << d.second << "\n";
        }
    }

    void displayHospitals() const {
        if (hospitals.empty()) {
            std::cout << "No hospitals found.\n";
            return;
        }
        for (const auto& h : hospitals) {
            std::cout << "ID: " << h.getId() << "\nName: " << h.getName()
                     << "\nAddress: " << h.getAddress() << "\nPhone: " 
                     << h.getPhone() << "\n\n";
        }
    }

    void displayDistances() const {
        if (distances.empty()) {
            std::cout << "No distances recorded.\n";
            return;
        }
        for (const auto& d : distances) {
            std::cout << "Hospital " << d.first.first << " and " << d.first.second
                     << " - Distance: " << std::fixed << std::setprecision(2)
                     << d.second << " km\n";
        }
    }
};

void displayMainMenu() {
    std::cout << "\nHospital Management System\n";
    std::cout << "1. Manage Hospitals\n";
    std::cout << "2. Manage Distances\n";
    std::cout << "3. Find Distance Between Hospitals\n";
    std::cout << "4. Exit\n";
    std::cout << "Enter choice: ";
}

void manageHospitals(HospitalManager& hm) {
    int choice;
    do {
        std::cout << "\nHospital Management\n";
        std::cout << "1. Add Hospital\n";
        std::cout << "2. View Hospitals\n";
        std::cout << "3. Update Hospital\n";
        std::cout << "4. Delete Hospital\n";
        std::cout << "5. Back to Main Menu\n";
        std::cout << "Enter choice: ";
        
        while (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number: ";
        }
        std::cin.ignore();

        switch (choice) {
            case 1: {
                int id;
                std::string name, address, phone;
                
                std::cout << "Enter Hospital ID: ";
                while (!(std::cin >> id)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid ID. Please enter a number: ";
                }
                std::cin.ignore();
                
                std::cout << "Enter Name: ";
                std::getline(std::cin, name);
                
                std::cout << "Enter Address: ";
                std::getline(std::cin, address);
                
                std::cout << "Enter Phone: ";
                std::getline(std::cin, phone);
                
                if (hm.addHospital(Hospital(id, name, address, phone))) {
                    std::cout << "Hospital added successfully.\n";
                } else {
                    std::cout << "Failed to add hospital. ID or phone number already exists.\n";
                }
                break;
            }
            case 2:
                hm.displayHospitals();
                break;
            case 3: {
                int id;
                std::string name, address, phone;
                
                std::cout << "Enter Hospital ID to update: ";
                while (!(std::cin >> id)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid ID. Please enter a number: ";
                }
                std::cin.ignore();
                
                std::cout << "Enter New Name: ";
                std::getline(std::cin, name);
                
                std::cout << "Enter New Address: ";
                std::getline(std::cin, address);
                
                std::cout << "Enter New Phone: ";
                std::getline(std::cin, phone);
                
                if (hm.updateHospital(id, name, address, phone)) {
                    std::cout << "Hospital updated successfully.\n";
                } else {
                    std::cout << "Update failed. Hospital not found or phone number already exists.\n";
                }
                break;
            }
            case 4: {
                int id;
                std::cout << "Enter Hospital ID to delete: ";
                while (!(std::cin >> id)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid ID. Please enter a number: ";
                }
                if (hm.deleteHospital(id)) {
                    std::cout << "Hospital deleted successfully.\n";
                } else {
                    std::cout << "Hospital not found!\n";
                }
                break;
            }
        }
    } while (choice != 5);
}

void manageDistances(HospitalManager& hm) {
    int choice;
    do {
        std::cout << "\nDistance Management\n";
        std::cout << "1. Add/Update Distance\n";
        std::cout << "2. View Distances\n";
        std::cout << "3. Delete Distance\n";
        std::cout << "4. Back to Main Menu\n";
        std::cout << "Enter choice: ";
        
        while (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number: ";
        }
        std::cin.ignore();

        switch (choice) {
            case 1: {
                int id1, id2;
                double distance;
                
                std::cout << "Enter First Hospital ID: ";
                while (!(std::cin >> id1)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid ID. Please enter a number: ";
                }
                
                std::cout << "Enter Second Hospital ID: ";
                while (!(std::cin >> id2)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid ID. Please enter a number: ";
                }
                
                std::cout << "Enter Distance (km): ";
                while (!(std::cin >> distance)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid distance. Please enter a number: ";
                }
                
                if (hm.addDistance(id1, id2, distance)) {
                    std::cout << "Distance added/updated successfully.\n";
                } else {
                    std::cout << "Invalid hospital IDs or same hospital!\n";
                }
                break;
            }
            case 2:
                hm.displayDistances();
                break;
            case 3: {
                int id1, id2;
                
                std::cout << "Enter First Hospital ID: ";
                while (!(std::cin >> id1)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid ID. Please enter a number: ";
                }
                
                std::cout << "Enter Second Hospital ID: ";
                while (!(std::cin >> id2)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid ID. Please enter a number: ";
                }
                
                if (hm.deleteDistance(id1, id2)) {
                    std::cout << "Distance deleted successfully.\n";
                } else {
                    std::cout << "Distance not found!\n";
                }
                break;
            }
        }
    } while (choice != 4);
}

void findDistance(HospitalManager& hm) {
    int id1, id2;
    
    std::cout << "Enter First Hospital ID: ";
    while (!(std::cin >> id1)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid ID. Please enter a number: ";
    }
    
    std::cout << "Enter Second Hospital ID: ";
    while (!(std::cin >> id2)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid ID. Please enter a number: ";
    }
    
    std::cout << "Distance between hospitals " << id1 << " and " << id2
             << ": " << hm.getDistance(id1, id2) << "\n";
}

int main() {
    HospitalManager hm;
    hm.loadFromCSV();

    int choice;
    do {
        displayMainMenu();
        
        while (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number: ";
        }
        std::cin.ignore();

        switch (choice) {
            case 1:
                manageHospitals(hm);
                break;
            case 2:
                manageDistances(hm);
                break;
            case 3:
                findDistance(hm);
                break;
            case 4:
                hm.saveToCSV();
                std::cout << "Data saved. Exiting...\n";
                break;
            default:
                std::cout << "Invalid choice!\n";
        }
    } while (choice != 4);

    return 0;
}