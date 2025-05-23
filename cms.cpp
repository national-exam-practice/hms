#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <sstream>

using namespace std;

// Data structures to store cities and roads
map<int, string> cities;
vector<pair<pair<int, int>, long long>> roads; // ((city1, city2), budget)

// File names
const string CITIES_FILE = "cities.txt";
const string ROADS_FILE = "roads.txt";

// Function prototypes
void loadData();
void saveData();
void addCity();
void viewAllCities();
void searchCityByIndex();
void searchCityByName();
void updateCity();
void deleteCity();
void registerRoad();
void viewAllRoads();
void updateRoad();
void deleteRoad();
void displayGraph();
void printMenu();

int main() {
    loadData();
    
    int choice;
    do {
        printMenu();
        cout << "Enter your choice: ";
        cin >> choice;
        
        switch(choice) {
            case 1: addCity(); break;
            case 2: viewAllCities(); break;
            case 3: searchCityByIndex(); break;
            case 4: searchCityByName(); break;
            case 5: updateCity(); break;
            case 6: deleteCity(); break;
            case 7: registerRoad(); break;
            case 8: viewAllRoads(); break;
            case 9: updateRoad(); break;
            case 10: deleteRoad(); break;
            case 11: displayGraph(); break;
            case 12: saveData(); break;
            case 13: cout << "Exiting program...\n"; break;
            default: cout << "Invalid choice. Please try again.\n";
        }
    } while(choice != 13);
    
    return 0;
}

void loadData() {
    // Load cities
    ifstream cityFile(CITIES_FILE);
    if (cityFile.is_open()) {
        string line;
        while (getline(cityFile, line)) {
            istringstream iss(line);
            int index;
            string name;
            if (iss >> index) {
                // Read the rest of the line as the city name
                getline(iss >> ws, name);
                cities[index] = name;
            }
        }
        cityFile.close();
    }
    
    // Load roads
    ifstream roadFile(ROADS_FILE);
    if (roadFile.is_open()) {
        int city1, city2;
        long long budget;
        while (roadFile >> city1 >> city2 >> budget) {
            // Store with smaller index first to avoid duplicates
            if (city1 > city2) swap(city1, city2);
            roads.push_back({{city1, city2}, budget});
        }
        roadFile.close();
    }
}

void saveData() {
    // Save cities
    ofstream cityFile(CITIES_FILE);
    for (const auto& city : cities) {
        cityFile << city.first << " " << city.second << "\n";
    }
    cityFile.close();
    
    // Save roads
    ofstream roadFile(ROADS_FILE);
    for (const auto& road : roads) {
        roadFile << road.first.first << " " << road.first.second << " " << road.second << "\n";
    }
    roadFile.close();
    
    cout << "Data saved successfully to " << CITIES_FILE << " and " << ROADS_FILE << ".\n";
}

void addCity() {
    int num;
    cout << "Enter the number of cities to be added: ";
    cin >> num;
    
    // Clear the newline left in the buffer after reading num
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    if (num <= 0) {
        cout << "Invalid number of cities. Please enter a positive number.\n";
        return;
    }

    for (int i = 0; i < num; i++) {
        string name;
        cout << "Enter city name #" << (i+1) << ": ";
        getline(cin, name);
        
        // Find the next available index
        int newIndex = 1;
        while (cities.find(newIndex) != cities.end()) {
            newIndex++;
        }
        
        cities[newIndex] = name;
        cout << "City '" << name << "' added with index " << newIndex << ".\n";
    }
}

void viewAllCities() {
    if (cities.empty()) {
        cout << "No cities registered yet.\n";
        return;
    }
    
    cout << "\nList of Cities:\n";
    cout << "Index\tCity Name\n";
    cout << "-----\t----------\n";
    for (const auto& city : cities) {
        cout << city.first << "\t" << city.second << "\n";
    }
}

void searchCityByIndex() {
    int index;
    cout << "Enter city index: ";
    cin >> index;
    
    auto it = cities.find(index);
    if (it != cities.end()) {
        cout << "City found:\n";
        cout << "Index: " << it->first << "\n";
        cout << "Name: " << it->second << "\n";
        
        // Find connected roads
        bool hasRoads = false;
        for (const auto& road : roads) {
            if (road.first.first == index || road.first.second == index) {
                if (!hasRoads) {
                    cout << "Connected roads:\n";
                    hasRoads = true;
                }
                int otherCity = (road.first.first == index) ? road.first.second : road.first.first;
                cout << "  - To " << cities[otherCity] << " (Budget: " << road.second << " RWF)\n";
            }
        }
        if (!hasRoads) cout << "No connected roads.\n";
    } else {
        cout << "City not found.\n";
    }
}

void searchCityByName() {
    string name;
    cout << "Enter city name: ";
    cin.ignore();
    getline(cin, name);
    
    bool found = false;
    for (const auto& city : cities) {
        if (city.second == name) {
            cout << "City found:\n";
            cout << "Index: " << city.first << "\n";
            cout << "Name: " << city.second << "\n";
            
            // Find connected roads
            bool hasRoads = false;
            for (const auto& road : roads) {
                if (road.first.first == city.first || road.first.second == city.first) {
                    if (!hasRoads) {
                        cout << "Connected roads:\n";
                        hasRoads = true;
                    }
                    int otherCity = (road.first.first == city.first) ? road.first.second : road.first.first;
                    cout << "  - To " << cities[otherCity] << " (Budget: " << road.second << " RWF)\n";
                }
            }
            if (!hasRoads) cout << "No connected roads.\n";
            
            found = true;
            break;
        }
    }
    
    if (!found) {
        cout << "City not found.\n";
    }
}

void updateCity() {
    viewAllCities();
    
    int index;
    cout << "Enter index of city to update: ";
    cin >> index;
    
    auto it = cities.find(index);
    if (it != cities.end()) {
        string newName;
        cout << "Current name: " << it->second << "\n";
        cout << "Enter new name: ";
        cin.ignore();
        getline(cin, newName);
        
        it->second = newName;
        cout << "City updated successfully.\n";
    } else {
        cout << "City not found.\n";
    }
}

void deleteCity() {
    viewAllCities();
    
    int index;
    cout << "Enter index of city to delete: ";
    cin >> index;
    
    auto it = cities.find(index);
    if (it != cities.end()) {
        // Check if city has any roads
        bool hasRoads = false;
        for (const auto& road : roads) {
            if (road.first.first == index || road.first.second == index) {
                hasRoads = true;
                break;
            }
        }
        
        if (hasRoads) {
            cout << "Cannot delete city because it has roads connected to it.\n";
            cout << "Please delete connected roads first.\n";
            return;
        }
        
        cout << "Are you sure you want to delete " << it->second << "? (y/n): ";
        char confirm;
        cin >> confirm;
        
        if (tolower(confirm) == 'y') {
            cities.erase(it);
            cout << "City deleted successfully.\n";
        } else {
            cout << "Deletion cancelled.\n";
        }
    } else {
        cout << "City not found.\n";
    }
}

void registerRoad() {
    if (cities.size() < 2) {
        cout << "Need at least 2 cities to register a road.\n";
        return;
    }
    
    viewAllCities();
    
    int city1, city2;
    long long budget;
    
    cout << "Enter first city index: ";
    cin >> city1;
    cout << "Enter second city index: ";
    cin >> city2;
    
    if (city1 == city2) {
        cout << "Cannot create road between the same city.\n";
        return;
    }
    
    if (cities.find(city1) == cities.end() || cities.find(city2) == cities.end()) {
        cout << "Invalid city index.\n";
        return;
    }
    
    // Ensure the road is stored with the smaller index first to avoid duplicates
    if (city1 > city2) {
        swap(city1, city2);
    }
    
    // Check if road already exists
    for (const auto& road : roads) {
        if (road.first.first == city1 && road.first.second == city2) {
            cout << "Road already exists. Use 'Update Road' to modify it.\n";
            return;
        }
    }
    
    cout << "Enter budget (in RWF): ";
    cin >> budget;
    
    roads.push_back({{city1, city2}, budget});
    cout << "Road registered between " << cities[city1] << " and " << cities[city2] 
         << " with budget " << budget << " RWF.\n";
}

void viewAllRoads() {
    if (roads.empty()) {
        cout << "No roads registered yet.\n";
        return;
    }
    
    cout << "\nList of Roads:\n";
    cout << "City 1\tCity 2\tBudget (RWF)\n";
    cout << "------\t------\t------------\n";
    for (const auto& road : roads) {
        cout << cities[road.first.first] << " (" << road.first.first << ")\t"
             << cities[road.first.second] << " (" << road.first.second << ")\t"
             << road.second << "\n";
    }
}

void updateRoad() {
    if (roads.empty()) {
        cout << "No roads registered yet.\n";
        return;
    }
    
    viewAllRoads();
    
    int city1, city2;
    cout << "Enter first city index of road to update: ";
    cin >> city1;
    cout << "Enter second city index of road to update: ";
    cin >> city2;
    
    // Ensure we search with smaller index first
    if (city1 > city2) {
        swap(city1, city2);
    }
    
    bool found = false;
    for (auto& road : roads) {
        if (road.first.first == city1 && road.first.second == city2) {
            found = true;
            cout << "Current budget: " << road.second << " RWF\n";
            cout << "Enter new budget: ";
            cin >> road.second;
            cout << "Road budget updated successfully.\n";
            break;
        }
    }
    
    if (!found) {
        cout << "Road not found.\n";
    }
}

void deleteRoad() {
    if (roads.empty()) {
        cout << "No roads registered yet.\n";
        return;
    }
    
    viewAllRoads();
    
    int city1, city2;
    cout << "Enter first city index of road to delete: ";
    cin >> city1;
    cout << "Enter second city index of road to delete: ";
    cin >> city2;
    
    // Ensure we search with smaller index first
    if (city1 > city2) {
        swap(city1, city2);
    }
    
    auto it = find_if(roads.begin(), roads.end(), [city1, city2](const auto& road) {
        return road.first.first == city1 && road.first.second == city2;
    });
    
    if (it != roads.end()) {
        cout << "Are you sure you want to delete the road between " 
             << cities[city1] << " and " << cities[city2] << "? (y/n): ";
        char confirm;
        cin >> confirm;
        
        if (tolower(confirm) == 'y') {
            roads.erase(it);
            cout << "Road deleted successfully.\n";
        } else {
            cout << "Deletion cancelled.\n";
        }
    } else {
        cout << "Road not found.\n";
    }
}

void displayGraph() {
    if (cities.empty()) {
        cout << "No cities registered yet.\n";
        return;
    }
    
    // Get the maximum city index
    int maxIndex = cities.rbegin()->first;
    
    // Create adjacency matrix for connections
    vector<vector<int>> connectionMatrix(maxIndex + 1, vector<int>(maxIndex + 1, 0));
    // Create adjacency matrix for budgets
    vector<vector<long long>> budgetMatrix(maxIndex + 1, vector<long long>(maxIndex + 1, 0));
    
    // Fill matrices
    for (const auto& road : roads) {
        int city1 = road.first.first;
        int city2 = road.first.second;
        connectionMatrix[city1][city2] = 1;
        connectionMatrix[city2][city1] = 1;
        budgetMatrix[city1][city2] = road.second;
        budgetMatrix[city2][city1] = road.second;
    }
    
    // Display connection matrix
    cout << "\nRoad Connection Matrix (1 = connected, 0 = not connected):\n";
    cout << "      ";
    for (const auto& city : cities) {
        cout << setw(4) << city.first;
    }
    cout << "\n";
    
    for (const auto& city1 : cities) {
        cout << setw(4) << city1.first << " ";
        for (const auto& city2 : cities) {
            cout << setw(4) << connectionMatrix[city1.first][city2.first];
        }
        cout << "\n";
    }
    
    // Display budget matrix
    cout << "\nRoad Budget Matrix (in billions RWF):\n";
    cout << "      ";
    for (const auto& city : cities) {
        cout << setw(10) << city.first;
    }
    cout << "\n";
    
    for (const auto& city1 : cities) {
        cout << setw(4) << city1.first << " ";
        for (const auto& city2 : cities) {
            if (budgetMatrix[city1.first][city2.first] == 0) {
                cout << setw(10) << "-";
            } else {
                cout << setw(10) << fixed << setprecision(2) 
                     << budgetMatrix[city1.first][city2.first] / 1000000000.0;
            }
        }
        cout << "\n";
    }
}

void printMenu() {
    cout << "\nRwanda Infrastructure Management System\n";
    cout << "1. Add City\n";
    cout << "2. View All Cities\n";
    cout << "3. Search City by Index\n";
    cout << "4. Search City by Name\n";
    cout << "5. Update City\n";
    cout << "6. Delete City\n";
    cout << "7. Register Road\n";
    cout << "8. View All Roads\n";
    cout << "9. Update Road\n";
    cout << "10. Delete Road\n";
    cout << "11. Display Graph (Adjacency Matrices)\n";
    cout << "12. Save Data\n";
    cout << "13. Exit\n";
}
