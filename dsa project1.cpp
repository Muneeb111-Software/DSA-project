#include<iostream>
#include<string>
#include<vector>
#include<queue>
#include<stack>
#include<fstream>
#include<algorithm>  // searcha nd sort y name binary
#include<unordered_map> //for O(1) average-time to search  d/r  ID
#include<iomanip>  // for decimal output(4.4,)
#include<sstream> // for parsing CSV lines using stringstream
using namespace std;


class Driver {
public:
    int id;
    string name;
    string contact;
    bool available;         // True if driver is free to accept rides
    double rating;          // Average rating from riders (0.0 to 5.0)
    int totalRides;         // Count of completed rides
    int totalRatedRides;    // Number of rides that were rated (for avg calc)
    double earnings;        // Total money earned

    // Default constructor — O(1)
    Driver() {
        id = 0; name = ""; contact = ""; available = true;
        rating = 0.0; totalRides = 0; totalRatedRides = 0; earnings = 0.0;
    }

    // Parameterized constructor — O(1)
    Driver(int i, string n, string c) {
        id = i; name = n; contact = c; available = true;
        rating = 0.0; totalRides = 0; totalRatedRides = 0; earnings = 0.0;
    }

    // Display driver info — O(1)
    void show() {
        cout<< "\nID: "<<id;
        cout<< "\nName: "<<name;
        cout<< "\nContact: " <<contact;
        cout<< "\nAvailable: "<< (available ? "Yes" : "No");
        cout<< "\nRating: " <<fixed<<setprecision(2)<<rating<<"/5";
        cout<< "\nTotal Rides: " <<totalRides;
        cout<< "\nTotal Rated Rides: "<<totalRatedRides;
        cout<< "\nEarnings: Rs. " <<fixed<<setprecision(2)<<earnings;
    }
};
// Represents a user who requests rides
class Rider {
public:
    int id;
    string name;
    string contact;
    int totalBookings; // Total number of rides requested

    // Default constructor — O(1)
    Rider() { id = 0; name = ""; contact = ""; totalBookings = 0; }

    // Parameterized constructor — O(1)
    Rider(int i, string n, string c) {
        id = i; name = n; contact = c; totalBookings = 0;
    }
    // Display rider info — O(1)
    void show() {
        cout<< "\nID: " <<id;
        cout<< "\nName: " <<name;
        cout<< "\nContact: "<<contact;
        cout<< "\nTotal Bookings: "<<totalBookings;
    }
};
class Ride {
public:
    int rideID, driverID, riderID;
    string pickup, dropoff, status; // Status: Pending, Booked, Completed, Cancelled
    double distance, fare;
    bool rated;            // True if rider has rated the driver
    double driverRating;   // Rating given by rider (1–5)

    // Default constructor — O(1)
    Ride() {
        rideID = 0; driverID = 0; riderID = 0;
        pickup = dropoff = status = "";
        distance = 0.0; fare = 0.0; rated = false; driverRating = 0.0;
        status = "Pending";
    }

    // Parameterized constructor — O(1)
    Ride(int rID, int dID, int rdID, string p, string d, double dist) {
        rideID = rID;
        driverID = dID;
        riderID = rdID;
        pickup = p;
        dropoff = d;
        distance = dist;
        fare = dist * 10;  // Rs. 10 per km — fixed pricing model
        status = "Booked";
        rated = false;
        driverRating = 0.0;
    }
};

// Core system managing drivers, riders, and rides
class MiniUber {
private:
    // Hash maps for O(1) average-time lookups by ID
    unordered_map<int, Driver> driverMap;   // ID -> Driver
    unordered_map<int, Rider> riderMap;     // ID -> Rider
    unordered_map<int, Ride> rideMap;       // RideID -> Ride

    // Stack to track cancelled rides (LIFO) — O(1) push/pop
    stack<Ride> cancelledRides;

    // Queue to manage available drivers (FIFO) — O(1) enqueue/dequeue
    queue<int> availableDriverIDs;

    // Auto-incrementing ID counters
    int nextDriverID, nextRiderID, nextRideID;

public:
    // Constructor — O(n) due to loadData()
    MiniUber() {
        nextDriverID = 1;
        nextRiderID = 1;
        nextRideID = 1;
        loadData(); // Load existing data from CSV files on startup
    }

    // Load all data from CSV files — O(n) where n = total records
    void loadData() {
        loadDrivers(); 
        loadRiders(); 
        loadRides();
    }

    // Load drivers from CSV — O(n), n = number of drivers
    void loadDrivers() {
        ifstream file("drivers.csv");
        if (!file) return; // File doesn't exist ? skip
        string line;
        bool first = true;
        while (getline(file, line)) {
            if (line.empty()) continue;
            if (first) { first = false; continue; } 
            stringstream ss(line);
            string idStr, name, contact, availStr, ratingStr, totalRidesStr, totalRatedRidesStr, earningsStr;
            getline(ss, idStr, ',');
            getline(ss, name, ',');
            getline(ss, contact, ',');
            getline(ss, availStr, ',');
            getline(ss, ratingStr, ',');
            getline(ss, totalRidesStr, ',');
            getline(ss, totalRatedRidesStr, ',');
            getline(ss, earningsStr, ',');
            if (idStr.empty()) continue;
            try {
                int id = stoi(idStr);
                bool available = (availStr == "1");
                double rating = stod(ratingStr);
                int totalRides = stoi(totalRidesStr);
                int totalRatedRides = stoi(totalRatedRidesStr);
                double earnings = stod(earningsStr);
                Driver d(id, name, contact);
                d.available = available;
                d.rating = rating;
                d.totalRides = totalRides;
                d.totalRatedRides = totalRatedRides;
                d.earnings = earnings;
                driverMap[id] = d;
                if (d.available) 
                    availableDriverIDs.push(id); // Re-add to queue if available
                if (id >= nextDriverID) 
                    nextDriverID = id + 1; // Ensure new IDs don't collide
            } catch (...) {
                // Skip  the corrupted lines if test is in numbr filed etc
            }
        }
        file.close();
    }
    // Load riders from CSV — O(n), n = number of riders
    void loadRiders() {
        ifstream file("riders.csv");
        if (!file) return;
        string line;
        bool first = true;
        while (getline(file, line)) {
            if (line.empty()) continue;
            if (first) { first = false; continue; }
            stringstream ss(line);
            string idStr, name, contact, totalBookingsStr;
            getline(ss, idStr, ',');
            getline(ss, name, ',');
            getline(ss, contact, ',');
            getline(ss, totalBookingsStr, ',');
            if (idStr.empty()) continue;
            try {
                int id = stoi(idStr);
                int totalBookings = stoi(totalBookingsStr);
                Rider r(id, name, contact);
                r.totalBookings = totalBookings;
                riderMap[id] = r;
                if (id >= nextRiderID) 
                    nextRiderID = id + 1;
            } catch (...) {
                // Skip bad lines
            }
        }
        file.close();
    }
    // Load rides from CSV — O(n), n = number of rides
    void loadRides() {
        ifstream file("rides.csv");
        if (!file) return;
        string line;
        bool first = true;
        while (getline(file, line)) {
            if (line.empty()) continue;
            if (first) { first = false; continue; }
            stringstream ss(line);
            string rideIdStr, driverIdStr, riderIdStr, pickup, dropoff, distanceStr, fareStr, status, ratedStr, ratingStr;
            getline(ss, rideIdStr, ',');
            getline(ss, driverIdStr, ',');
            getline(ss, riderIdStr, ',');
            getline(ss, pickup, ',');
            getline(ss, dropoff, ',');
            getline(ss, distanceStr, ',');
            getline(ss, fareStr, ',');
            getline(ss, status, ',');
            getline(ss, ratedStr, ',');
            getline(ss, ratingStr, ',');
            if (rideIdStr.empty()) continue;
            try {
                int rideID = stoi(rideIdStr);
                int driverID = stoi(driverIdStr);
                int riderID = stoi(riderIdStr);
                double distance = stod(distanceStr);
                double fare = stod(fareStr);
                bool rated = (ratedStr == "1");
                double driverRating = (ratingStr.empty() ? 0.0 : stod(ratingStr));
                Ride r;
                r.rideID = rideID;
                r.driverID = driverID;
                r.riderID = riderID;
                r.pickup = pickup;
                r.dropoff = dropoff;
                r.distance = distance;
                r.fare = fare;
                r.status = status;
                r.rated = rated;
                r.driverRating = driverRating;
                rideMap[rideID] = r;
                if (r.status == "Cancelled") {
                    cancelledRides.push(r); // Restore to stack
                }
                if (rideID >= nextRideID) 
                    nextRideID = rideID + 1;
            } catch (...) {
                // Skip invalid lines
            }
        }
        file.close();
    }

    // Save all data to CSV files — O(n)
    void saveData() {
        saveDrivers(); 
        saveRiders(); 
        saveRides();
    }
    // Save drivers to CSV — O(n), n = number of drivers
    void saveDrivers() {
        ofstream file("drivers.csv");
        file << "ID,Name,Contact,Available,Rating,TotalRides,TotalRatedRides,Earnings\n";
        for (auto& kv : driverMap) {
            Driver& d = kv.second;
            file << d.id << "," << d.name<< "," <<d.contact << ","
                 << (d.available ? "1" : "0")<< "," <<d.rating << ","
                 << d.totalRides << "," << d.totalRatedRides<< "," << d.earnings << "\n";
        }
        file.close();
    }

    // Save riders to CSV — O(n), n = number of riders
    void saveRiders() {
        ofstream file("riders.csv");
        file << "ID,Name,Contact,TotalBookings\n";
        for (auto& kv : riderMap) {
            Rider& r = kv.second;
            file<<r.id<< ","<<r.name<< ","<< r.contact<< "," <<r.totalBookings<<"\n";
        }
        file.close();
    }

    // Save rides to CSV — O(n), n = number of rides
    void saveRides() {
        ofstream file("rides.csv");
        file << "RideID,DriverID,RiderID,Pickup,Dropoff,Distance,Fare,Status,Rated,Rating\n";
        for (auto& kv : rideMap) {
            Ride& r = kv.second;
            file <<r.rideID<< "," <<r.driverID << "," <<r.riderID << ","
                 <<r.pickup<< "," <<r.dropoff << "," <<r.distance << ","
                 <<r.fare << "," << r.status<< "," <<(r.rated ? "1" : "0") << ","
                 << r.driverRating << "\n";
        }
        file.close();
    }

    // Add a new driver — O(1)
    void addDriver() {
        string name, contact;
        cout<< "\n=== ADD DRIVER ===\n";
        cout<< "Name: "; cin.ignore(); getline(cin, name);
        cout<< "Contact: "; getline(cin, contact);
        Driver d(nextDriverID, name, contact);
        driverMap[nextDriverID] = d;
        availableDriverIDs.push(nextDriverID);
        cout << "\n Driver added! ID: " << nextDriverID << endl;
        nextDriverID++;
    }

    // Add a new rider — O(1)
    void addRider() {
        string name, contact;
        cout<< "\n=== ADD RIDER ===\n";
        cout<< "Name: "; cin.ignore(); getline(cin, name);
        cout<< "Contact: "; getline(cin, contact);
        riderMap[nextRiderID] = Rider(nextRiderID, name, contact);
        cout<< "\n? Rider added! ID: " << nextRiderID << endl;
        nextRiderID++;
    }

    // View all drivers — O(n), n = number of drivers
    void viewAllDrivers() {
        if (driverMap.empty()) { cout << "\nNo drivers registered.\n"; return; }
        cout<< "\n=== ALL DRIVERS ===\n";
        for (auto& kv : driverMap) {
            kv.second.show();
            cout << "\n--------------------\n";
        }
    }

    // View all riders — O(n), n = number of riders
    void viewAllRiders() {
        if (riderMap.empty()) { cout << "\nNo riders registered.\n"; return; }
        cout << "\n=== ALL RIDERS ===\n";
        for (auto& kv : riderMap) {
            kv.second.show();
            cout << "\n--------------------\n";
        }
    }
    // Request a new ride — O(1)
    void requestRide() {
        if (riderMap.empty()) { cout << "\nNo riders! Add a rider first.\n"; return; }
        if (availableDriverIDs.empty()) { cout << "\nNo available drivers!\n"; return; }

        int riderID; double distance; string pickup, dropoff;
        cout<< "\n=== REQUEST RIDE ===\n";
        cout<< "Rider ID: "; cin >> riderID;
        if (riderMap.find(riderID) == riderMap.end()) {
            cout << "\nRider not found!\n";
            return;
        }

        cin.ignore();
        cout<< "Pickup: "; getline(cin, pickup);
        cout<< "Dropoff: "; getline(cin, dropoff);
        cout<< "Distance (km): "; cin >> distance;
        if (distance <= 0) {
            cout << "\nInvalid distance!\n";
            return;
        }

        int driverID = availableDriverIDs.front();
        availableDriverIDs.pop();
        driverMap[driverID].available = false;

        Ride newRide(nextRideID, driverID, riderID, pickup, dropoff, distance);
        rideMap[nextRideID] = newRide;

        cout<< "\n----------- RIDE BOOKED ----------\n";
        cout<< "Ride ID: " << newRide.rideID<< "\n";
        cout<< "From: " << newRide.pickup<< "\n";
        cout<< "To: " << newRide.dropoff<< "\n";
        cout<< "Distance: "<< fixed<< setprecision(1)<< newRide.distance<< " km\n";
        cout << "Fare: Rs. " << fixed << setprecision(2)<< newRide.fare<< "\n";
        cout << "Status: Booked\n";
        cout << "-----------------------------------\n";

        nextRideID++;
    }

    // Cancel a booked ride — O(1)
    void cancelRide() {
        int rideID; cout << "\nRide ID to cancel: "; cin >> rideID;
        if (rideMap.find(rideID) == rideMap.end()) {
            cout << "\nRide not found!\n";
            return;
        }
        Ride& r = rideMap[rideID];
        if (r.status != "Booked") {
            cout << "\nOnly 'Booked' rides can be cancelled!\n";
            return;
        }
        r.status = "Cancelled";
        cancelledRides.push(r);
        driverMap[r.driverID].available = true;
        availableDriverIDs.push(r.driverID);
        cout << "\n? Ride cancelled successfully!\n";
    }

    // Complete a booked ride — O(1)
    void completeRide() {
        int rideID; cout << "\nRide ID to complete: "; cin >> rideID;
        if (rideMap.find(rideID) == rideMap.end()) {
            cout << "\nRide not found!\n";
            return;
        }
        Ride& r = rideMap[rideID];
        if (r.status != "Booked") {
            cout << "\nOnly 'Booked' rides can be completed\n";
            return;
        }

        Rider& rider = riderMap[r.riderID];
        rider.totalBookings++;
        Driver& driver = driverMap[r.driverID];
        driver.totalRides++;
        driver.earnings += r.fare;
        driver.available = true;
        availableDriverIDs.push(r.driverID);
        r.status = "Completed";

        cout<< "\n? Ride " <<rideID<<" COMPLETED!\n";
        cout<< "Fare: Rs. " <<r.fare<<"\n";
        cout<< "Rider Total Bookings: " <<rider.totalBookings<< "\n";
        cout<< "Driver Total Earnings: Rs. " <<fixed <<setprecision(2) <<driver.earnings<< "\n";
    }

    // Rate a completed ride — O(1)
    void rateDriver() {
        int rideID; double rating;
        cout<< "\nRide ID: "; cin >>rideID;
        if (!rideMap.count(rideID)) { cout << "\nRide not found!\n"; return; }
        Ride& r = rideMap[rideID];
        if (r.status != "Completed") { cout << "\nOnly completed rides can be rated!\n"; return; }
        if (r.rated) { cout << "\nAlready rated!\n"; return; }
        cout<< "Rating (1-5): "; cin >> rating;
        if (rating < 1 || rating > 5) { cout << "\nInvalid rating!\n"; return; }

        r.rated = true;
        r.driverRating = rating;
        Driver& d = driverMap[r.driverID];
        d.totalRatedRides++;
        if (d.totalRatedRides == 1) {
            d.rating = rating;
        } else {
            d.rating = (d.rating * (d.totalRatedRides - 1) + rating) / d.totalRatedRides;
        }
        cout<< "\n Rating saved!\n";
    }

    // Search driver by ID (O(1)) or by Name (O(n log n)) 
    void searchDriver() {
        int choice;
        cout<< "\n=== SEARCH DRIVER ===\n";
        cout<< "1. By ID\n2. By Name\nChoice: "; cin>> choice;
        if (choice == 1) {
            int id; cout << "ID: "; cin >> id;
            if (driverMap.count(id)) 
                driverMap[id].show(); // O(1) average
            else 
                cout << "\nNot found!\n";
        } else if (choice == 2) {
            string name; 
            cout << "Name: "; cin.ignore(); getline(cin, name);
            vector<Driver> v;
            for (auto& d : driverMap) v.push_back(d.second); // O(n)
            sort(v.begin(), v.end(), [](const Driver& a, const Driver& b) {
                return a.name < b.name;
            }); // O(n log n)
            // Searching using binary search (lower_bound)
            auto it = lower_bound(v.begin(), v.end(), name,
                [](const Driver& d, const string& n) { return d.name < n; }); // O(log n)
            if (it != v.end() && it->name == name) 
                it->show();
            else 
                cout << "\nNot found!\n";
        }
    }

    // Search rider by ID (O(1)) or by Name (O(n log n))
    void searchRider() {
        int choice;
        cout << "\n--- SEARCH RIDER ---\n";
        cout << "1. By ID\n2. By Name\nChoice: "; cin >> choice;
        if (choice == 1) {
            int id; cout<< "ID: "; cin >> id;
            if (riderMap.count(id)) 
                riderMap[id].show(); // O(1)
            else 
                cout << "\nNot found!\n";
        } else if (choice == 2) {
            string name; 
            cout << "Name: "; cin.ignore(); getline(cin, name);
            vector<Rider> v;
            for (auto& r : riderMap) v.push_back(r.second); // O(n)
            sort(v.begin(), v.end(), [](const Rider& a, const Rider& b) {
                return a.name < b.name;
            }); // O(n log n)
            // Searching using binary search (lower_bound)
            auto it = lower_bound(v.begin(), v.end(), name,
                [](const Rider& r, const string& n) { return r.name < n; }); // O(log n)
            if (it != v.end() && it->name == name) 
                it->show();
            else 
                cout << "\nNot found!\n";
        }
    }

    // Sort drivers by name using Bubble Sort — O(n²)
    void bubbleSortDriversByName() {
        vector<Driver> v;
        for (auto& d : driverMap) v.push_back(d.second); // O(n)
        int n = v.size();
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n -i - 1; j++) {
                
                if (v[j].name > v[j+1].name) {
                    swap(v[j], v[j+1]);
                }
            }
        }
        cout << "\n=== DRIVERS (Bubble Sorted by Name) ===\n";
        for (auto& d : v) 
            cout << d.name << " (ID: " << d.id << ")\n";
    }

    // Sort rides by distance using Quick Sort — O(n log n) average
    void quickSortRidesByDistance() {
        vector<Ride> v;
        for (auto& r : rideMap) v.push_back(r.second); // O(n)
        if (v.empty()) { cout << "\nNo rides.\n"; return; }
        quickSort(v, 0, v.size()-1); // O(n log n) average
        cout << "\n=== RIDES (Quick Sorted by Distance) ===\n";
        for (auto& r : v)
            cout<< "Ride "<< r.rideID<< ": " << r.distance<< " km (Rs. "<<r.fare<<")\n";
    }

    // Recursive Quick Sort — O(n log n) average, O(n²) worst-case
    void quickSort(vector<Ride>& arr, int l, int r) {
        if (l < r) {
            int p = partition(arr, l, r);
            quickSort(arr, l, p-1);
            quickSort(arr, p+1, r);
        }
    }

    // Partition for Quick Sort — O(n)
    int partition(vector<Ride>& arr, int l, int r) {
        double pivot = arr[r].distance;
        int i = l - 1;
        for (int j = l; j < r; j++) {
            if (arr[j].distance <= pivot) {
                i++;
                // Swapping elements to partition around pivot
                swap(arr[i], arr[j]);
            }
        }
        // last  swap to place pivot in correct position
        swap(arr[i+1], arr[r]);
        return i + 1;
    }


    // Show live driver status — O(n²) in worst case (nested loop)
    void showDriverStatus() {
        if (driverMap.empty()) {
            cout << "\nNo drivers available.\n";
            return;
        }
        cout << "\n-------DRIVER LIVE STATUS --------\n";
        for (auto& kv : driverMap) { // O(n)
            Driver& d = kv.second;
            cout << "Driver ID: " << d.id << " | Name: " << d.name << "\n";
            if (d.available) {
                cout << "  Status: FREE\n";
            } else {
                cout << "  Status: BUSY\n";
                bool found = false;
                for (auto& rideKV : rideMap) { // O(n) — nested loop
                    if (rideKV.second.driverID == d.id && rideKV.second.status == "Booked") {
                        cout<< "  On Ride ID: " << rideKV.second.rideID 
                             << " (From: " <<rideKV.second.pickup 
                             << " ? " << rideKV.second.dropoff << ")\n";
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    cout << "  On Ride ID: None (All rides completed)\n";
                }
            }
            cout<< "  Total Rides Completed: " << d.totalRides << "\n";
            cout<< "  Rating: " << fixed << setprecision(2) << d.rating << "/5\n";
            cout<< "----------------------------------------\n";
        }
    }

    // Show cancelled rides (from stack) — O(n)
    void showCancelledRides() {
        if (cancelledRides.empty()) {
            cout << "\nNo cancelled rides.\n";
            return;
        }
        cout << "\n---- CANCELLED RIDES -----\n";
        stack<Ride> temp = cancelledRides;
        while (!temp.empty()) { // O(n)
            Ride r = temp.top();
            cout<<"\nRide ID: " << r.rideID;
            cout<<"\nFrom: " << r.pickup << " ? "<< r.dropoff;
            cout<< "\nStatus: " << r.status << "\n";
            cout<< "\n---\n";
            temp.pop();
        }
    }

    // Generate final report — O(n)
    void showFinalReport() {
        cout << "\n-------- MINI-UBER FINAL REPORT --------\n";
        int completed = 0, cancelled = 0;
        double totalEarnings = 0, totalDistance = 0;
        for (auto& kv : rideMap) { // O(n)
            if (kv.second.status == "Completed") {
                completed++;
                totalEarnings += kv.second.fare;
                totalDistance += kv.second.distance;
            } else if (kv.second.status == "Cancelled") {
                cancelled++;
            }
        }
        cout<<"Total Rides: " <<rideMap.size() << "\n";
        cout<< "Completed: " <<completed << " | Cancelled: "<< cancelled<< "\n";
        cout<< "Total Earnings: Rs. " << fixed << setprecision(2)<< totalEarnings<< "\n";
        if (completed > 0) {
            cout << "Avg Fare: Rs. " <<(totalEarnings / completed)<< "\n";
            cout << "Avg Distance: " << (totalDistance / completed)<< " km\n";
        }
        cout << "\n---- COMPLETED RIDES -----\n";
        for (auto& kv : rideMap) { // O(n)
            if (kv.second.status == "Completed") {
                cout << "Ride " << kv.second.rideID<< ": "
                     << kv.second.pickup << " ? " << kv.second.dropoff
                     << " | " <<kv.second.distance<< " km | Rs. " <<kv.second.fare<< "\n";
            }
        }

        // Export to file — O(n)
        ofstream f("uber_final_report.txt");
        f<< "---------- MINI-UBER FINAL REPORT ---------\n";
        f<< "Total Rides: " << rideMap.size() << "\n";
        f<< "Completed: " << completed << " | Cancelled: " << cancelled<< "\n";
        f<< "Total Earnings: Rs. " <<totalEarnings << "\n";
        if (completed > 0) {
            f << "Avg Fare: Rs. " << (totalEarnings / completed)<< "\n";
            f << "Avg Distance: " << (totalDistance / completed) << " km\n";
        }
        f << "\n=== COMPLETED RIDES ===\n";
        for (auto& kv : rideMap) {
            if (kv.second.status == "Completed") {
                f << "Ride " << kv.second.rideID << ": "
                  << kv.second.pickup << " ? " << kv.second.dropoff
                  << " | " << kv.second.distance <<" km | Rs. " << kv.second.fare<< "\n";
            }
        }
        f.close();
        cout << "\n? Report exported to 'uber_final_report.txt'\n";
    }

    // for handle menu (each is O(1) for menu display O(function) for action) 

    void managePeople() {
        int ch;
        do {
            cout<< "\n=== MANAGE DRIVERS & RIDERS ===\n";
            cout<< "1. Add Driver\n";
            cout<< "2. Add Rider\n";
            cout<< "3. View All Drivers\n";
            cout<< "4. View All Riders\n";
            cout<< "5. Back to Main Menu\n";
            cout<< "Choice: ";
            cin>> ch;
            switch (ch) {
                case 1: addDriver(); break;
                case 2: addRider(); break;
                case 3: viewAllDrivers(); break;
                case 4: viewAllRiders(); break;
                case 5: break;
                default: cout << "\nInvalid choice!\n";
            }
        } while (ch != 5);
    }

    void manageRides() {
        int ch;
        do {
            cout<< "\n=== MANAGE RIDES ===\n";
            cout<< "1. Request Ride\n";
            cout<< "2. Cancel Ride\n";
            cout<< "3. Complete Ride\n";
            cout<< "4. Rate Driver\n";
            cout<< "5. Back to Main Menu\n";
            cout<< "Choice: ";
            cin>> ch;
            switch (ch) {
                case 1: requestRide(); break;
                case 2: cancelRide(); break;
                case 3: completeRide(); break;
                case 4: rateDriver(); break;
                case 5: break;
                default: cout << "\nInvalid choice!\n";
            }
        } while (ch != 5);
    }

    void searchAndSort() {
        int ch;
        do {
            cout<< "\n---- SEARCH & SORT -----\n";
            cout<< "1. Search Driver\n";
            cout<< "2. Search Rider\n";
            cout<< "3. Sort Drivers by Name (Bubble Sort)\n";
            cout<< "4. Sort Rides by Distance (Quick Sort)\n";
            cout<< "5. Back to Main Menu\n";
            cout<< "Choice: ";
            cin>> ch;
            switch (ch) {
                case 1: searchDriver(); break;
                case 2: searchRider(); break;
                case 3: bubbleSortDriversByName(); break;
                case 4: quickSortRidesByDistance(); break;
                case 5: break;
                default: cout << "\nInvalid choice!\n";
            }
        } while (ch != 5);
    }

    void reports() {
        int ch;
        do {
            cout<< "\n----- REPORTS & ANALYTICS -----\n";
            cout<< "1. Live Driver Status\n";
            cout<< "2. Cancelled Rides\n";
            cout<< "3. Final System Report\n";
            cout<< "4. Back to Main Menu\n";
            cout<< "Choice: ";
            cin>> ch;
            switch (ch) {
                case 1: showDriverStatus(); break;
                case 2: showCancelledRides(); break;
                case 3: showFinalReport(); break;
                case 4: break;
                default: cout << "\nInvalid choice!\n";
            }
        } while (ch != 4);
    }
};

// Simple login — O(1)
bool loginSystem() {
    string username, password;
    cout<< "\nUsername: ";
    cin>> username;
    cout<< "Password: ";
    cin>> password;
    if (username == "admin" && password == "12345") {
        cout << " Login Successful\n";
        return true;
    } else {
        cout << " Invalid passowrd try again\n";
        return false;
    }
}

// Main function — O(n) due to system loading
int main() {
    cout<< "============================================"<<endl;
    cout<<endl;
    cout<< "       MINI-UBER RIDE-SHARING SYSTEM        \n";
    cout<<"============================================\n";

    while (!loginSystem()) {
        cout << "Please try again.\n";
    }

    MiniUber system;
    int choice;

    do {
        cout<< "\n------ MAIN MENU -----------\n";
        cout<< "1. Manage Drivers & Riders\n";
        cout<< "2. Manage Rides\n";
        cout<< "3. Search & Sort (DSA Algorithms)\n";
        cout<< "4. Reports & Analytics\n";
        cout<< "5. Exit\n";
        
        cout<< "Choice: ";
        cin>> choice;
        switch (choice) {
            case 1: system.managePeople(); break;
            case 2: system.manageRides(); break;
            case 3: system.searchAndSort(); break;
            case 4: system.reports(); break;
            case 5: break;
            default: cout << "\nInvalid choice!\n";
        }
    } while (choice != 5);

    system.saveData();
    cout<<"\n Data saved. Thank you!\n";
    return 0;
}
