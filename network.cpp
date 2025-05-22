#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <set>
#include <algorithm>
#include <cctype>
using namespace std;

const int MAX = 1000;

struct HealthCenter {
    int id;
    string name;
    string district;
    float lat;
    float lon;
    int capacity;
};

struct Connection {
    int to;
    float distance;
    int time;
    string description;
};

vector<HealthCenter> centers;
vector<Connection> adjList[MAX];

// File I/O Functions
void readHealthCenters() {
    ifstream file("health_centers.csv");
    if (!file.good()) {
        ofstream createFile("health_centers.csv");
        createFile << "ID,Name,District,Latitude,Longitude,Capacity\n";
        createFile.close();
        return;
    }

    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> tokens;
        string token;
        
        while (getline(ss, token, ',')) tokens.push_back(token);
        if (tokens.size() != 6) continue;

        try {
            HealthCenter hc{
                stoi(tokens[0]),
                tokens[1],
                tokens[2],
                stof(tokens[3]),
                stof(tokens[4]),
                stoi(tokens[5])
            };

            bool exists = any_of(centers.begin(), centers.end(),
                [&hc](const HealthCenter& c) { return c.id == hc.id; });
            if (!exists) centers.push_back(hc);
        } catch (...) {}
    }
    file.close();
}

void readConnections() {
    ifstream file("connections.csv");
    if (!file.good()) {
        ofstream createFile("connections.csv");
        createFile << "FromID,ToID,DistanceKM,TimeMinutes,Description\n";
        createFile.close();
        return;
    }

    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> tokens;
        string token;
        
        while (getline(ss, token, ',')) tokens.push_back(token);
        if (tokens.size() != 5) continue;

        try {
            int fromID = stoi(tokens[0]);
            Connection conn{
                stoi(tokens[1]),
                stof(tokens[2]),
                stoi(tokens[3]),
                tokens[4]
            };

            bool fromExists = any_of(centers.begin(), centers.end(),
                [fromID](const HealthCenter& c) { return c.id == fromID; });
            bool toExists = any_of(centers.begin(), centers.end(),
                [conn](const HealthCenter& c) { return c.id == conn.to; });
            
            if (fromExists && toExists) adjList[fromID].push_back(conn);
        } catch (...) {}
    }
    file.close();
}

void saveHealthCenters() {
    ofstream file("health_centers.csv");
    file << "ID,Name,District,Latitude,Longitude,Capacity\n";
    for (const auto& hc : centers) {
        file << hc.id << "," << hc.name << "," << hc.district << ","
             << hc.lat << "," << hc.lon << "," << hc.capacity << "\n";
    }
    file.close();
}

void saveConnections() {
    ofstream file("connections.csv");
    file << "FromID,ToID,DistanceKM,TimeMinutes,Description\n";
    for (int fromID = 0; fromID < MAX; fromID++) {
        for (const auto& conn : adjList[fromID]) {
            file << fromID << "," << conn.to << "," << conn.distance << ","
                 << conn.time << "," << conn.description << "\n";
        }
    }
    file.close();
}

// CRUD Operations
void addHealthCenter() {
    HealthCenter hc;
    cout << "Enter ID: ";
    cin >> hc.id;
    
    if (any_of(centers.begin(), centers.end(),
        [hc](const HealthCenter& c) { return c.id == hc.id; })) {
        cout << "ID already exists!\n";
        return;
    }

    cin.ignore();
    cout << "Name: "; getline(cin, hc.name);
    cout << "District: "; getline(cin, hc.district);
    cout << "Latitude: "; cin >> hc.lat;
    cout << "Longitude: "; cin >> hc.lon;
    cout << "Capacity: "; cin >> hc.capacity;

    centers.push_back(hc);
    saveHealthCenters();
    cout << "Health center added!\n";
}

void editHealthCenter() {
    int id;
    cout << "Enter health center ID: ";
    cin >> id;

    auto it = find_if(centers.begin(), centers.end(),
        [id](const HealthCenter& hc) { return hc.id == id; });
    if (it == centers.end()) {
        cout << "Health center not found!\n";
        return;
    }

    cin.ignore();
    cout << "New name (" << it->name << "): ";
    string name; getline(cin, name);
    if (!name.empty()) it->name = name;

    cout << "New district (" << it->district << "): ";
    string district; getline(cin, district);
    if (!district.empty()) it->district = district;

    cout << "New latitude (" << it->lat << "): ";
    string lat; getline(cin, lat);
    if (!lat.empty()) it->lat = stof(lat);

    cout << "New longitude (" << it->lon << "): ";
    string lon; getline(cin, lon);
    if (!lon.empty()) it->lon = stof(lon);

    cout << "New capacity (" << it->capacity << "): ";
    string cap; getline(cin, cap);
    if (!cap.empty()) it->capacity = stoi(cap);

    saveHealthCenters();
    cout << "Health center updated!\n";
}

void viewHealthCenters() {
    if (centers.empty()) {
        cout << "No health centers found!\n";
        return;
    }

    cout << left << setw(5) << "ID" << setw(20) << "Name" << setw(15) << "District"
         << setw(10) << "Lat" << setw(10) << "Lon" << setw(10) << "Capacity\n";
    for (const auto& hc : centers) {
        cout << setw(5) << hc.id << setw(20) << hc.name << setw(15) << hc.district
             << setw(10) << hc.lat << setw(10) << hc.lon << setw(10) << hc.capacity << "\n";
    }
}

void removeHealthCenter() {
    int id;
    cout << "Enter health center ID to remove: ";
    cin >> id;

    auto it = remove_if(centers.begin(), centers.end(),
        [id](const HealthCenter& hc) { return hc.id == id; });
    if (it == centers.end()) {
        cout << "Health center not found!\n";
        return;
    }

    centers.erase(it, centers.end());
    for (int i = 0; i < MAX; i++) {
        adjList[i].erase(remove_if(adjList[i].begin(), adjList[i].end(),
            [id](const Connection& c) { return c.to == id; }), adjList[i].end());
    }
    adjList[id].clear();

    saveHealthCenters();
    saveConnections();
    cout << "Health center removed!\n";
}

// Connection Management
void addConnection() {
    int from, to;
    cout << "From ID: "; cin >> from;
    cout << "To ID: "; cin >> to;

    bool fromExists = any_of(centers.begin(), centers.end(),
        [from](const HealthCenter& hc) { return hc.id == from; });
    bool toExists = any_of(centers.begin(), centers.end(),
        [to](const HealthCenter& hc) { return hc.id == to; });

    if (!fromExists || !toExists) {
        cout << "Invalid health center IDs!\n";
        return;
    }

    Connection conn;
    cout << "Distance (km): "; cin >> conn.distance;
    cout << "Time (minutes): "; cin >> conn.time;
    cin.ignore();
    cout << "Description: "; getline(cin, conn.description);
    conn.to = to;

    adjList[from].push_back(conn);
    saveConnections();
    cout << "Connection added!\n";
}

void viewConnections() {
    bool found = false;
    for (int i = 0; i < MAX; i++) {
        if (!adjList[i].empty()) {
            for (const auto& conn : adjList[i]) {
                cout << i << " -> " << conn.to << " (" << conn.distance << " km, "
                     << conn.time << " min): " << conn.description << "\n";
                found = true;
            }
        }
    }
    if (!found) cout << "No connections found!\n";
}

void removeConnection() {
    int from, to;
    cout << "From ID: "; cin >> from;
    cout << "To ID: "; cin >> to;

    auto& connections = adjList[from];
    size_t originalSize = connections.size();
    connections.erase(remove_if(connections.begin(), connections.end(),
        [to](const Connection& c) { return c.to == to; }), connections.end());

    if (connections.size() < originalSize) {
        saveConnections();
        cout << "Connection removed!\n";
    } else {
        cout << "Connection not found!\n";
    }
}

// Graph Algorithms
void dijkstra() {
    int start, end;
    cout << "Start ID: "; cin >> start;
    cout << "End ID: "; cin >> end;

    vector<float> dist(MAX, numeric_limits<float>::max());
    vector<int> prev(MAX, -1);
    priority_queue<pair<float, int>, vector<pair<float, int>>, greater<>> pq;

    dist[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [currentDist, u] = pq.top();
        pq.pop();

        if (u == end) break;
        if (currentDist > dist[u]) continue;

        for (const auto& conn : adjList[u]) {
            float newDist = dist[u] + conn.distance;
            if (newDist < dist[conn.to]) {
                dist[conn.to] = newDist;
                prev[conn.to] = u;
                pq.push({newDist, conn.to});
            }
        }
    }

    if (dist[end] == numeric_limits<float>::max()) {
        cout << "No path exists!\n";
        return;
    }

    vector<int> path;
    for (int at = end; at != -1; at = prev[at]) path.push_back(at);
    reverse(path.begin(), path.end());

    cout << "Shortest path (" << dist[end] << " km): ";
    for (size_t i = 0; i < path.size(); i++) {
        cout << path[i];
        if (i != path.size()-1) cout << " -> ";
    }
    cout << "\n";
}

void bfsTraversal() {
    int start;
    cout << "Start ID: "; cin >> start;

    vector<bool> visited(MAX, false);
    queue<int> q;
    q.push(start);
    visited[start] = true;

    cout << "BFS Traversal: ";
    while (!q.empty()) {
        int u = q.front(); q.pop();
        cout << u << " ";

        for (const auto& conn : adjList[u]) {
            if (!visited[conn.to]) {
                visited[conn.to] = true;
                q.push(conn.to);
            }
        }
    }
    cout << "\n";
}

bool hasCycleUtil(int u, vector<bool>& visited, vector<bool>& recStack) {
    if (!visited[u]) {
        visited[u] = true;
        recStack[u] = true;

        for (const auto& conn : adjList[u]) {
            if (!visited[conn.to] && hasCycleUtil(conn.to, visited, recStack))
                return true;
            else if (recStack[conn.to])
                return true;
        }
    }
    recStack[u] = false;
    return false;
}

void detectCycle() {
    vector<bool> visited(MAX, false);
    vector<bool> recStack(MAX, false);

    for (int i = 0; i < MAX; i++) {
        if (!visited[i] && hasCycleUtil(i, visited, recStack)) {
            cout << "Cycle detected!\n";
            return;
        }
    }
    cout << "No cycles found!\n";
}

void floydWarshall() {
    vector<vector<float>> dist(MAX, vector<float>(MAX, numeric_limits<float>::max()));
    
    for (int i = 0; i < MAX; i++) {
        dist[i][i] = 0;
        for (const auto& conn : adjList[i])
            dist[i][conn.to] = conn.distance;
    }

    for (int k = 0; k < MAX; k++) {
        for (int i = 0; i < MAX; i++) {
            for (int j = 0; j < MAX; j++) {
                if (dist[i][k] + dist[k][j] < dist[i][j])
                    dist[i][j] = dist[i][k] + dist[k][j];
            }
        }
    }

    cout << "All-pairs shortest paths computed!\n";
}

void primMST() {
    vector<bool> visited(MAX, false);
    priority_queue<pair<float, pair<int, int>>, 
                   vector<pair<float, pair<int, int>>>, 
                   greater<>> pq;

    int start = centers.empty() ? 0 : centers[0].id;
    visited[start] = true;
    for (const auto& conn : adjList[start])
        pq.push({conn.distance, {start, conn.to}});

    float total = 0;
    cout << "Minimum Spanning Tree:\n";
    while (!pq.empty()) {
        auto [weight, edge] = pq.top();
        auto [u, v] = edge;
        pq.pop();

        if (visited[v]) continue;
        visited[v] = true;
        total += weight;
        cout << u << " - " << v << " : " << weight << " km\n";

        for (const auto& conn : adjList[v]) {
            if (!visited[conn.to])
                pq.push({conn.distance, {v, conn.to}});
        }
    }
    cout << "Total distance: " << total << " km\n";
}

void emergencyRouting() {
    int start, minCapacity;
    cout << "Current location ID: "; cin >> start;
    cout << "Minimum capacity required: "; cin >> minCapacity;

    vector<float> dist(MAX, numeric_limits<float>::max());
    priority_queue<pair<float, int>, vector<pair<float, int>>, greater<>> pq;

    dist[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [currentDist, u] = pq.top();
        pq.pop();

        auto hc = find_if(centers.begin(), centers.end(),
            [u](const HealthCenter& c) { return c.id == u; });
        if (hc != centers.end() && hc->capacity >= minCapacity) {
            cout << "Nearest center: " << hc->name << " (" << u << ")\n";
            cout << "Distance: " << currentDist << " km\n";
            return;
        }

        for (const auto& conn : adjList[u]) {
            if (dist[conn.to] > dist[u] + conn.distance) {
                dist[conn.to] = dist[u] + conn.distance;
                pq.push({dist[conn.to], conn.to});
            }
        }
    }
    cout << "No suitable health center found!\n";
}

// Additional Features
void viewRelationships() {
    ofstream file("relationship_table.csv");
    file << "FromID,ToID,DistanceKM,TimeMinutes,Description\n";
    for (int from = 0; from < MAX; from++) {
        for (const auto& conn : adjList[from]) {
            file << from << "," << conn.to << "," << conn.distance << ","
                 << conn.time << "," << conn.description << "\n";
        }
    }
    file.close();
    cout << "Relationship table saved to relationship_table.csv\n";
}

// Menu System
void displayMenu() {
    cout << "\n==== Health Center Network System ====\n";
    cout << "1. Add Health Center\n2. Edit Health Center\n3. View Health Centers\n4. Remove Health Center\n";
    cout << "5. Add Connection\n6. Edit Connection\n7. View Connections\n8. Remove Connection\n";
    cout << "9. View Relationships\n10. Dijkstra's Shortest Path\n11. BFS Traversal\n12. Detect Cycle\n";
    cout << "13. Floyd-Warshall All-Pairs\n14. Prim's MST\n15. Emergency Routing\n0. Exit\n";
    cout << "Enter choice: ";
}

int main() {
    readHealthCenters();
    readConnections();

    int choice;
    do {
        displayMenu();
        cin >> choice;
        cin.ignore();

        try {
            switch (choice) {
                case 1: addHealthCenter(); break;
                case 2: editHealthCenter(); break;
                case 3: viewHealthCenters(); break;
                case 4: removeHealthCenter(); break;
                case 5: addConnection(); break;
                case 7: viewConnections(); break;
                case 8: removeConnection(); break;
                case 9: viewRelationships(); break;
                case 10: dijkstra(); break;
                case 11: bfsTraversal(); break;
                case 12: detectCycle(); break;
                case 13: floydWarshall(); break;
                case 14: primMST(); break;
                case 15: emergencyRouting(); break;
                case 0: 
                    saveHealthCenters();
                    saveConnections();
                    cout << "Exiting...\n";
                    break;
                default: cout << "Invalid choice!\n";
            }
        } catch (...) {
            cout << "Invalid input detected!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (choice != 0);

    return 0;
}