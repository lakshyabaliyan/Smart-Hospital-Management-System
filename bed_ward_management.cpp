#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <cctype>
using namespace std;

const int    MAX_BEDS      = 100;
const int    MAX_WAITING   = 50;
const string BED_FILE      = "beds.txt";
const string WAIT_FILE     = "waitlist.txt";
const string NO_PATIENT    = "None";

const int TYPE_GENERAL   = 1;
const int TYPE_ICU       = 2;
const int TYPE_PRIVATE   = 3;
const int TYPE_EMERGENCY = 4;

const int STATUS_AVAILABLE   = 1;
const int STATUS_OCCUPIED    = 2;
const int STATUS_MAINTENANCE = 3;

string bedTypeToString(int t) 
{
    if(t == TYPE_GENERAL)   
        return "General";
    if(t == TYPE_ICU)       
        return "ICU";
    if(t == TYPE_PRIVATE)  
         return "Private";
    if(t == TYPE_EMERGENCY) 
         return "Emergency";
    return "Unknown";
}

string statusToString(int s) 
{
    if(s == STATUS_AVAILABLE)   
        return "Available";
    if(s == STATUS_OCCUPIED)    
        return "Occupied";
    if(s == STATUS_MAINTENANCE) 
         return "Maintenance";
    return "Unknown";
}

string readLine(const string& prompt) 
{
    string line;
    cout << prompt;
    if(!getline(cin, line)) 
    {
        cout << "\nInput closed. Exiting.\n";
        exit(0);
    }
    return line;
}

void clearScreen() 
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() 
{
    readLine("\nPress Enter to return to the menu...");
}

void printTitle(const string& title) 
{
    cout << "==========================================\n";
    cout << "  " << title << "\n";
    cout << "==========================================\n\n";
}

int readInt(const string& prompt, int lo, int hi) 
{
    while(true) 
    {
        stringstream ss(readLine(prompt));
        int value;
        if((ss >> value) && (ss >> ws).eof() && value >= lo && value <= hi)
            return value;
        cout << "  Invalid input. Enter a number between "<< lo << " and " << hi << ".\n";
    }
}

string readText(const string& prompt, bool allowSpaces) 
{
    while(true) 
    {
        string s = readLine(prompt);
        size_t start = s.find_first_not_of(" \t");
        size_t end   = s.find_last_not_of(" \t");
        if (start == string::npos) 
        {
            cout << "  Input cannot be empty.\n";
            continue;
        }
        s = s.substr(start, end - start + 1);
        if(s.find('|') != string::npos) 
        {
            cout << "  The '|' character is not allowed.\n";
            continue;
        }
        if(!allowSpaces && s.find_first_of(" \t") != string::npos) 
        {
            cout << "  Spaces are not allowed here.\n";
            continue;
        }
        return s;
    }
}

int readint(const string& prompt) 
{
    cout << "  1. General   2. ICU   3. Private   4. Emergency\n";
    return readInt(prompt, 1, 4);
}

void printBedHeader() 
{
    cout << left << setw(8)  << "Bed ID"<< setw(16) << "Department"<< setw(12) << "Type"<< setw(14) << "Status"<< "Patient ID\n";
    cout << string(60, '-') << "\n";
}

bool equalsIgnoreCase(const string& a, const string& b) 
{
    if(a.size() != b.size()) 
         return false;
    for(size_t i = 0; i < a.size(); i++)
    {
        if(tolower(a[i]) != tolower(b[i]))
            return false;
    }
    return true;
}

bool toInt(const string& s, int& out) 
{
    try 
    {
        size_t pos;
        out = stoi(s, &pos);
        return pos == s.size();
    } 
    catch (...) 
    {
        return false;
    }
}

class Bed 
{
private:
    int bedId;
    string department;
    int type;
    int status;
    string patientId;

public:
    Bed() : bedId(0), department(""), type(TYPE_GENERAL),status(STATUS_AVAILABLE), patientId(NO_PATIENT) {}

    Bed(int id, const string& departmentName, int bedType): bedId(id), department(departmentName), type(bedType),status(STATUS_AVAILABLE), patientId(NO_PATIENT) {}

    int getId() const 
    { 
        return bedId; 
    }
    string getDepartment() const 
    { 
        return department; 
    }
    int getType() const 
    { 
        return type; 
    }
    int getStatus() const 
    { 
        return status; 
    }
    string getPatientId() const 
    { 
        return patientId; 
    }
    bool isAvailable() const 
    { 
        return status == STATUS_AVAILABLE; 
    }

    void assignPatient(const string& pid) 
    {
        status = STATUS_OCCUPIED;
        patientId = pid;
    }

    void release() 
    {
        status = STATUS_AVAILABLE;
        patientId = NO_PATIENT;
    }

    void restore(int s, const string& pid) 
    {
        status= s;
        patientId = pid;
    }

    bool setMaintenance(bool on) 
    {
        if(status == STATUS_OCCUPIED) 
            return false;
        status = on ? STATUS_MAINTENANCE : STATUS_AVAILABLE;
        return true;
    }

    void printRow() const 
    {
        cout << left << setw(8)  << bedId<< setw(16) << department<< setw(12) << bedTypeToString(type)<< setw(14) << statusToString(status)<< patientId << "\n";
    }
};

struct WaitEntry 
{
    string patientId;
    string department;
    int required;
    int priority;
};

class BedManager 
{
private:
    Bed beds[MAX_BEDS];
    int bedCount;

    WaitEntry waitList[MAX_WAITING];
    int waitCount;

    int findBedIndex(int id) const 
    {
        int low = 0, high = bedCount - 1;
        while(low <= high) 
        {
            int mid = low + (high - low) / 2;
            if(beds[mid].getId() == id)      
                 return mid;
            else if(beds[mid].getId() < id)  
                 low = mid + 1;
            else                              
                high = mid - 1;
        }
        return -1;
    }

    int findBedByPatient(const string& pid) const 
    {
        for(int i = 0; i < bedCount; i++)
            if(beds[i].getStatus() == STATUS_OCCUPIED && beds[i].getPatientId() == pid)
                return i;
        return -1;
    }

    int findInWaitList(const string& pid) const 
    {
        for(int i = 0; i < waitCount; i++)
            if(waitList[i].patientId == pid) 
                return i;
        return -1;
    }

    void insertSorted(const Bed& b) 
    {
        int i = bedCount - 1;
        while(i >= 0 && beds[i].getId() > b.getId()) 
        {
            beds[i + 1] = beds[i];
            i--;
        }
        beds[i + 1] = b;
        bedCount++;
    }

    int fallbackOrder(int req, int order[]) const 
    {
        switch(req) 
        {
            case TYPE_ICU:
                order[0] = TYPE_ICU;
                order[1] = TYPE_EMERGENCY;
                return 2;
            case TYPE_EMERGENCY:
                order[0] = TYPE_EMERGENCY;
                order[1] = TYPE_ICU;
                return 2;
            case TYPE_PRIVATE:
                order[0] = TYPE_PRIVATE;
                order[1] = TYPE_GENERAL;
                return 2;
            case TYPE_GENERAL:
            default:
                order[0] = TYPE_GENERAL;
                order[1] = TYPE_PRIVATE;
                return 2;
        }
    }

    bool addToWaitList(const string& pid, const string& department, int req, int priority) 
    {
        if(waitCount >= MAX_WAITING) 
            return false;

        int i = waitCount - 1;
        while(i >= 0 && waitList[i].priority > priority) 
        {
            waitList[i + 1] = waitList[i];
            i--;
        }
        waitList[i + 1].patientId = pid;
        waitList[i + 1].department = department;
        waitList[i + 1].required= req;
        waitList[i + 1].priority= priority;
        waitCount++;
        saveWaitList();
        return true;
    }

    void removeFromWaitList(int index) 
    {
        for(int i = index; i < waitCount - 1; i++)
            waitList[i] = waitList[i + 1];
        waitCount--;
        saveWaitList();
    }

    void serveWaitingList() 
    {
        int i = 0;
        while(i < waitCount) 
        {
            int bedId = allocateSuitableBed(waitList[i].patientId,waitList[i].department,waitList[i].required);
            if(bedId != -1) 
            {
                cout << "  >> Waiting patient " << waitList[i].patientId<< " (priority " << waitList[i].priority<< ") auto-assigned to Bed " << bedId << ".\n";
                removeFromWaitList(i);
            } 
            else 
            {
                i++;
            }
        }
    }

    void saveBeds() const 
    {
        ofstream file(BED_FILE.c_str());
        if(!file) 
        {
            cout << "Error: could not save bed records!\n";
            return;
        }
        for(int i = 0; i < bedCount; i++) 
        {
            file << beds[i].getId() << "|"<< beds[i].getDepartment() << "|"<< beds[i].getType() << "|"<< beds[i].getStatus() << "|"<< beds[i].getPatientId() << "\n";
        }
    }

    void saveWaitList() const 
    {
        ofstream file(WAIT_FILE.c_str());
        if(!file) 
        {
            cout << "Error: could not save waiting list!\n";
            return;
        }
        for(int i = 0; i < waitCount; i++) 
        {
            file << waitList[i].patientId << "|"<< waitList[i].department << "|"<< waitList[i].required << "|"<< waitList[i].priority << "\n";
        }
    }

    void loadBeds() 
    {
        ifstream file(BED_FILE.c_str());
        string line;
        while(getline(file, line) && bedCount < MAX_BEDS) 
        {
            stringstream ss(line);
            string sId, department, sType, sStatus, pid;
            if(!getline(ss, sId, '|') || !getline(ss, department, '|') ||!getline(ss, sType, '|') || !getline(ss, sStatus, '|') ||!getline(ss, pid, '|'))
                continue;

            int id, t, s;
            if(!toInt(sId, id) || !toInt(sType, t) || !toInt(sStatus, s))
                continue;
            if(id <= 0 || t < 1 || t > 4 || s < 1 || s > 3 || department.empty())
                continue;
            if(findBedIndex(id) != -1) 
                continue;
            if(s == 2 && (pid.empty() || pid == NO_PATIENT)) 
                continue;

            Bed b(id, department, t);
            b.restore(s,s == 2 ? pid : NO_PATIENT);
            insertSorted(b);
        }
    }

    void loadWaitList() 
    {
        ifstream file(WAIT_FILE.c_str());
        string line;
        while(getline(file, line) && waitCount < MAX_WAITING) 
        {
            stringstream ss(line);
            string pid, department, sType, sPri;
            if(!getline(ss, pid, '|') || !getline(ss, department, '|') ||!getline(ss, sType, '|') || !getline(ss, sPri, '|'))
                continue;

            int t, p;
            if(pid.empty() || department.empty() ||!toInt(sType, t) || !toInt(sPri, p)) 
                 continue;
            if(t < 1 || t > 4 || p < 1 || p > 5) 
                continue;

            int i = waitCount - 1;
            while(i >= 0 && waitList[i].priority > p) 
            {
                waitList[i + 1] = waitList[i];
                i--;
            }
            waitList[i + 1].patientId = pid;
            waitList[i + 1].department = department;
            waitList[i + 1].required= t;
            waitList[i + 1].priority = p;
            waitCount++;
        }
    }

public:
    BedManager() : bedCount(0), waitCount(0) 
    {
        loadBeds();
        loadWaitList();
    }

    int allocateSuitableBed(const string& pid, const string& department, int required) 
    {
        int order[4];
        int n = fallbackOrder(required, order);

        for(int k = 0; k < n; k++) 
        {
            for(int i = 0; i < bedCount; i++) 
            {
                if (beds[i].isAvailable() &&beds[i].getType() == order[k] &&equalsIgnoreCase(beds[i].getDepartment(), department)) 
                {
                    beds[i].assignPatient(pid);
                    saveBeds();
                    return beds[i].getId();
                }
            }
        }
        return -1;
    }

    void addBed() 
    {
        if(bedCount >= MAX_BEDS) 
        {
            cout << "Bed storage is full!\n";
            return;
        }
        int id = readInt("Enter Bed ID (positive number): ", 1, 999999);
        if(findBedIndex(id) != -1) 
        {
            cout << "Bed ID already exists!\n";
            return;
        }
        string department = readText("Enter Department Name: ", true);
        int type = readint("Select Bed Type (1-4): ");

        insertSorted(Bed(id, department, type));
        saveBeds();
        cout << "Bed " << id << " added successfully.\n";
    }

    void removeBed() 
    {
        int id = readInt("Enter Bed ID to remove: ", 1, 999999);
        int idx = findBedIndex(id);
        if(idx == -1) 
        { 
            cout << "Bed not found!\n"; return; 
        }
        if(beds[idx].getStatus() == STATUS_OCCUPIED) 
        {
            cout << "Cannot remove an occupied bed. Discharge the patient first.\n";
            return;
        }
        for(int i = idx; i < bedCount - 1; i++) beds[i] = beds[i + 1];
        bedCount--;
        saveBeds();
        cout << "Bed " << id << " removed.\n";
    }

    void displayBeds() const 
    {
        if(bedCount == 0) 
        { 
            cout << "No bed records found.\n"; 
            return; 
        }
        printBedHeader();
        for(int i = 0; i < bedCount; i++) 
            beds[i].printRow();
    }

    void searchBedById() const 
    {
        int id = readInt("Enter Bed ID to search: ", 1, 999999);
        int idx = findBedIndex(id);
        if(idx == -1) 
        { 
            cout << "Bed not found!\n"; 
            return; 
        }
        printBedHeader();
        beds[idx].printRow();
    }

    void searchByDepartment() const 
    {
        string department = readText("Enter Department Name: ", true);
        bool found = false;
        for(int i = 0; i < bedCount; i++) 
        {
            if(equalsIgnoreCase(beds[i].getDepartment(), department)) 
            {
                if(!found) printBedHeader();
                beds[i].printRow();
                found = true;
            }
        }
        if(!found) 
             cout << "No beds found in department '" << department << "'.\n";
    }

    void findAvailableBeds() const 
    {
        cout << "Filter by type?  0. All types\n";
        cout << "  1. General   2. ICU   3. Private   4. Emergency\n";
        int choice = readInt("Choice (0-4): ", 0, 4);

        bool found = false;
        for(int i = 0; i < bedCount; i++) 
        {
            if(!beds[i].isAvailable()) 
                continue;
            if(choice != 0 && beds[i].getType() != choice)
                continue;
            if(!found) 
            {
                printBedHeader();
            }
            beds[i].printRow();
            found = true;
        }
        if(!found) 
            cout << "No matching beds currently available.\n";
    }

    void allocateBedAuto() 
    {
        string pid = readText("Enter Patient ID: ", false);

        if(findBedByPatient(pid) != -1) 
        {
            cout << "Patient " << pid << " already occupies a bed!\n";
            return;
        }
        if(findInWaitList(pid) != -1) 
        {
            cout << "Patient " << pid << " is already on the waiting list.\n";
            return;
        }

        cout << "Triage level: 1 = Critical, 2 = Severe, 3 = Moderate, 4 = Mild, 5 = Minor\n";
        int priority = readInt("Enter triage level (1-5): ", 1, 5);
        string department = readText("Enter required department (e.g. ICU, Paediatrics): ", true);
        cout << "Required bed type:\n";
        int req = readint("Select Bed Type (1-4): ");

        int bedId = allocateSuitableBed(pid, department, req);
        if(bedId != -1) 
        {
            int idx = findBedIndex(bedId);
            cout << "Allocated Bed " << bedId << " ("<< beds[idx].getDepartment() << ", "<< bedTypeToString(beds[idx].getType()) << ") to patient "<< pid << ".\n";
            if(beds[idx].getType() != req)
                cout << "Note: no " << bedTypeToString(req) << " bed was free, so the closest suitable type was used.\n";
            return;
        }

        cout << "No suitable bed of type " << bedTypeToString(req) << " is available in department " << department << ".\n";
        if(readInt("Add patient to the waiting list? (1 = Yes, 0 = No): ", 0, 1)) 
        {
            if(addToWaitList(pid, department, req, priority))
                cout << "Patient added to waiting list (priority " << priority << ").\n";
            else
                cout << "Waiting list is full!\n";
        }
    }

    void allocateBedManual() 
    {
        int id = readInt("Enter Bed ID: ", 1, 999999);
        int idx = findBedIndex(id);
        if(idx == -1) 
        {  
             cout << "Bed not found!\n";
             return; 
        }
        if(!beds[idx].isAvailable()) 
        {
            cout << "Bed is not available (" << statusToString(beds[idx].getStatus())<< ").\n";
            return;
        }
        string pid = readText("Enter Patient ID: ", false);
        if(findBedByPatient(pid) != -1) 
        {
            cout << "Patient " << pid << " already occupies a bed!\n";
            return;
        }
        int w = findInWaitList(pid);
        if(w != -1) removeFromWaitList(w);

        beds[idx].assignPatient(pid);
        saveBeds();
        cout << "Bed " << id << " allocated to patient " << pid << ".\n";
    }

    void dischargePatient() 
    {
        int id = readInt("Enter Bed ID: ", 1, 999999);
        int idx = findBedIndex(id);
        if(idx == -1) 
        { 
            cout << "Bed not found!\n"; 
            return; 
        }
        if(beds[idx].getStatus() != STATUS_OCCUPIED) 
        {
            cout << "This bed is not occupied!\n";
            return;
        }
        string pid = beds[idx].getPatientId();
        beds[idx].release();
        saveBeds();
        cout << "Patient " << pid << " discharged. Bed " << id<< " is now available.\n";
        serveWaitingList();
    }

    void toggleMaintenance() 
    {
        int id = readInt("Enter Bed ID: ", 1, 999999);
        int idx = findBedIndex(id);
        if(idx == -1) 
        { 
            cout << "Bed not found!\n"; 
            return; 
        }
        bool putUnder = (beds[idx].getStatus() != STATUS_MAINTENANCE);
        if(!beds[idx].setMaintenance(putUnder)) 
        {
            cout << "Cannot change an occupied bed. Discharge the patient first.\n";
            return;
        }
        saveBeds();
        cout << "Bed " << id << (putUnder ? " is now under maintenance.\n": " is back in service.\n");
        if(!putUnder) 
            serveWaitingList();
    }

    void displayWaitList() const 
    {
        if(waitCount == 0) 
        { 
            cout << "Waiting list is empty.\n"; 
            return; 
        }
        cout << left << setw(6) << "No." << setw(16) << "Patient ID"<< setw(12) << "Needs" << "Triage Level\n";
        cout << string(46, '-') << "\n";
        for(int i = 0; i < waitCount; i++) 
        {
            cout << left << setw(6) << (i + 1)<< setw(16) << waitList[i].patientId<< setw(12) << bedTypeToString(waitList[i].required)<< waitList[i].priority << "\n";
        }
    }

    void showReport() const 
    {
        if(bedCount == 0) 
        { 
            cout << "No bed records found.\n"; 
            return; 
        }

        int total[5] = {0}, avail[5] = {0}, occ[5] = {0}, maint[5] = {0};
        for(int i = 0; i < bedCount; i++) 
        {
            int t = beds[i].getType();
            total[t]++;
            if(beds[i].getStatus() == STATUS_AVAILABLE)        
                avail[t]++;
            else if(beds[i].getStatus() == STATUS_OCCUPIED)   
                 occ[t]++;
            else                                                   
                maint[t]++;
        }

        cout << left << setw(12) << "Type" << setw(8) << "Total"<< setw(12) << "Available" << setw(10) << "Occupied"<< "Maintenance\n";
        cout << string(54, '-') << "\n";

        int tt = 0, ta = 0, to = 0, tm = 0;
        for(int t = 1; t <= 4; t++) 
        {
            cout << left << setw(12) << bedTypeToString(t)<< setw(8) << total[t] << setw(12) << avail[t]<< setw(10) << occ[t] << maint[t] << "\n";
            tt += total[t]; ta += avail[t]; to += occ[t]; tm += maint[t];
        }
        cout << string(54, '-') << "\n";
        cout << left << setw(12) << "ALL" << setw(8) << tt << setw(12) << ta<< setw(10) << to << tm << "\n";

        int usable = tt - tm;
        double pct = usable > 0 ? (100.0 * to / usable) : 0.0;
        cout << fixed << setprecision(1)<< "Occupancy rate: " << pct << "%   |   Patients waiting: " << waitCount << "\n";
    }

};

int main() 
{
    BedManager manager;
    int choice;
    do 
    {
        clearScreen();
        cout << "==========================================\n"
             << "        BED & DEPARTMENT MANAGEMENT\n"
             << "==========================================\n"
             << "  BED RECORDS\n"
             << "    1. Add Bed\n"
             << "    2. Remove Bed\n"
             << "    3. Display All Beds\n"
             << "    4. Toggle Maintenance Mode\n"
             << "\n  SEARCHING\n"
             << "    5. Search Bed by ID\n"
             << "    6. Search Beds by Department\n"
             << "    7. Find Available Beds\n"
             << "\n  BED ALLOCATION\n"
             << "    8. Allocate Suitable Bed (auto)\n"
             << "    9. Allocate Specific Bed (manual)\n"
             << "   10. Discharge Patient\n"
             << "\n  WAITING LIST & REPORTS\n"
             << "   11. View Waiting List\n"
             << "   12. Occupancy Report\n"
             << "\n    0. Exit\n"
             << "------------------------------------------\n";

        choice = readInt("Enter your choice: ", 0, 12);
        if(choice == 0) 
            break;

        clearScreen();
        switch(choice) 
        {
            case 1:  printTitle("ADD BED");
                manager.addBed();            
                break;
            case 2:  printTitle("REMOVE BED");      
                manager.removeBed();         
                break;
            case 3:  printTitle("ALL BEDS");                   
                manager.displayBeds();       
                break;
            case 4:  printTitle("MAINTENANCE MODE");           
                manager.toggleMaintenance(); 
                break;
            case 5:  printTitle("SEARCH BED BY ID");           
                manager.searchBedById();     
                break;
            case 6:  printTitle("SEARCH BEDS BY DEPARTMENT");        
                manager.searchByDepartment();      
                break;
            case 7:  printTitle("FIND AVAILABLE BEDS");        
                 manager.findAvailableBeds(); 
                 break;
            case 8:  printTitle("ALLOCATE SUITABLE BED");      
                manager.allocateBedAuto();   
                break;
            case 9:  printTitle("ALLOCATE SPECIFIC BED");     
                 manager.allocateBedManual(); 
                 break;
            case 10: printTitle("DISCHARGE PATIENT");          
                 manager.dischargePatient();  
                 break;
            case 11: printTitle("WAITING LIST");               
                manager.displayWaitList();   
                break;
            case 12: printTitle("OCCUPANCY REPORT");           
                manager.showReport();        
                break;
        }
        pauseScreen();
    }while(true);
    clearScreen();
    cout << "Exiting Bed and Department Section...\n";
    return 0;
}
