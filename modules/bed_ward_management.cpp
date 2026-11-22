#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <cctype>
using namespace std;

struct BedNode
{
    int bedID;
    string department;
    string wardType;
    string status;
    string patientID;
   BedNode* next;
};

class BedManager
{
private:
   BedNode* head;

    string toLowerCase(string str)
    {
        for (int i = 0; i < str.length(); i++)
            str[i] = tolower(str[i]);
        return str;
    }

    string readText(string message)
    {
        string value;
        cout << message;
        getline(cin, value);
        return value;
    }

    int readInt(string message)
    {
        string input;
        int value;
        stringstream ss;
        while (true)
        {
            cout << message;
            getline(cin, input);
            ss.clear();
            ss.str(input);
            if (ss >> value)
            {
                ss >> ws;
                if (ss.eof())
                    return value;
            }
            cout << "Invalid input. Enter a valid number.\n";
        }
    }

    bool validPatientID(string id)
    {
        if (id.empty())
            return false;
        for (int i = 0; i < id.length(); i++)
        {
            if (!isalnum(id[i]) && id[i] != '-')
                return false;
        }
        return true;
    }

    string readDepartment()
    {
        string department;
        while (true)
        {
            department = readText("Enter Department: ");
            if (department.empty())
            {
                cout << "Department cannot be empty.\n";
                continue;
            }
            bool valid = true;
            for (int i = 0; i < department.length(); i++)
            {
                if (!isalpha(department[i]) &&
                    department[i] != ' ' &&
                    department[i] != '-' &&
                    department[i] != '&')
                {
                    valid = false;
                    break;
                }
            }
            if (valid)
                return department;
            cout << "Invalid department name.\n";
        }
    }

    string readWardType()
    {
        string ward;
        while (true)
        {
            ward = toLowerCase(readText(
                "Enter Ward Type (General/ICU/Private/Emergency): "));
            if (ward == "general")
                return "General";
            else if (ward == "icu")
                return "ICU";
            else if (ward == "private")
                return "Private";
            else if (ward == "emergency")
                return "Emergency";

            cout << "Invalid ward type. Try again.\n";
        }
    }

    BedNode* searchByID(int id)
    {
        BedNode* temp = head;
        while (temp != NULL)
        {
            if (temp->bedID == id)
                return temp;

            temp = temp->next;
        }
        return NULL;
    }

    BedNode* searchByPatient(string id)
    {
       BedNode* temp = head;
        while (temp != NULL)
        {
            if (temp->patientID == id && temp->status == "Occupied")
                return temp;
            temp = temp->next;
        }
        return NULL;
    }

    void saveBeds()
    {
        ofstream file("beds.txt");
        if (!file)
        {
            cout << "Unable to save bed records.\n";
            return;
        }

        BedNode* temp = head;
        while (temp != NULL)
        {
            file << temp->bedID << "|"<< temp->department << "|"<< temp->wardType << "|"<< temp->status << "|"<< temp->patientID << "\n";
            temp = temp->next;
        }
        file.close();
    }

    void loadBeds()
    {
        ifstream file("beds.txt");
        if (!file)
            return;
        string line;
        while (getline(file, line))
        {
            stringstream ss(line);
            string id, department, wardType, status, patientID;

            if (!getline(ss, id, '|') ||!getline(ss, department, '|') ||!getline(ss, wardType, '|') ||!getline(ss, status, '|'))
            {
                cout << "Skipping invalid record in beds.txt.\n";
                continue;
            }
            // Patient ID may be empty for an available bed.
            getline(ss, patientID);
            // Reject records containing extra fields.
            if (patientID.find('|') != string::npos)
                continue;
            stringstream convert(id);
            int bedID;
            if (!(convert >> bedID))
                continue;
            convert >> ws;
            if (!convert.eof() || bedID <= 0 || department.empty())
                continue;

            wardType = toLowerCase(wardType);
            if (wardType == "general")
                wardType = "General";
            else if (wardType == "icu")
                wardType = "ICU";
            else if (wardType == "private")
                wardType = "Private";
            else if (wardType == "emergency")
                wardType = "Emergency";
            else
                continue;

            if (status != "Available" &&status != "Occupied" &&status != "Maintenance")
                continue;

            if (searchByID(bedID) != NULL)
                continue;

            if ((status == "Occupied" && !validPatientID(patientID)) ||(status != "Occupied" && !patientID.empty()))
                continue;

            if (status == "Occupied" && searchByPatient(patientID) != NULL)
                continue;

            BedNode* newNode = new BedNode;
            newNode->bedID = bedID;
            newNode->department = department;
            newNode->wardType = wardType;
            newNode->status = status;
            newNode->patientID = patientID;
            newNode->next = NULL;

            if (head == NULL)
                head = newNode;
            else
            {
                BedNode* temp = head;
                while (temp->next != NULL)
                    temp = temp->next;
                temp->next = newNode;
            }
        }
        file.close();
    }

    void displayHeader()
    {
        cout << left<< setw(10) << "BED ID"<< setw(20) << "DEPARTMENT"<< setw(16) << "WARD TYPE"<< setw(14) << "STATUS"<< setw(12) << "PATIENT ID" << endl;
        cout << string(72, '-') << endl;
    }

    void displayRow(BedNode* temp)
    {
        cout << left<< setw(10) << temp->bedID<< setw(20) << temp->department<< setw(16) << temp->wardType<< setw(14) << temp->status
             << setw(12) << temp->patientID << endl;
    }

public:
    BedManager()
    {
        head = NULL;
        loadBeds();
    }

    ~BedManager()
    {
        BedNode* temp = head;
        while (temp != NULL)
        {
            BedNode* nextNode = temp->next;
            delete temp;
            temp = nextNode;
        }
    }

    void addBed()
    {
        cout << "\n===== ADD BED =====\n";
        int id = readInt("Enter Bed ID: ");
        if (id <= 0)
        {
            cout << "Bed ID must be positive.\n";
            return;
        }
        if (searchByID(id) != NULL)
        {
            cout << "Bed ID already exists.\n";
            return;
        }
        string department = readDepartment();
        string ward = readWardType();
        BedNode* newNode = new BedNode;

        newNode->bedID = id;
        newNode->department = department;
        newNode->wardType = ward;
        newNode->status = "Available";
        newNode->patientID = "";
        newNode->next = NULL;

        if (head == NULL)
            head = newNode;
        else
        {
            BedNode* temp = head;
            while (temp->next != NULL)
                temp = temp->next;
            temp->next = newNode;
        }
        saveBeds();
        cout << "\nBed added successfully!\n";
    }

    void removeBed()
    {
        cout << "\n===== REMOVE BED =====\n";
        int id = readInt("Enter Bed ID: ");
        BedNode* temp = head;
        BedNode* previous = NULL;
        while (temp != NULL && temp->bedID != id)
        {
            previous = temp;
            temp = temp->next;
        }
        if (temp == NULL)
        {
            cout << "Bed not found.\n";
            return;
        }

        if (temp->status == "Occupied")
        {
            cout << "Cannot remove an occupied bed.\n";
            return;
        }

        if (previous == NULL)
            head = temp->next;
        else
            previous->next = temp->next;

        delete temp;
        saveBeds();
        cout << "Bed removed successfully.\n";
    }

    void displayBeds()
    {
        cout << "\n===== ALL BEDS =====\n";
        if (head == NULL)
        {
            cout << "No beds available.\n";
            return;
        }

        displayHeader();
        BedNode* temp = head;
        int count = 0;
        while (temp != NULL)
        {
            displayRow(temp);
            count++;
            temp = temp->next;
        }
        cout << "\nTotal Beds: " << count << endl;
    }

    void searchBedByID()
    {
        cout << "\n===== SEARCH BED BY ID =====\n";
        int id = readInt("Enter Bed ID: ");
        BedNode* temp = searchByID(id);
        if (temp == NULL)
        {
            cout << "Bed not found.\n";
            return;
        }
        displayHeader();
        displayRow(temp);
    }

    void searchByDepartment()
    {
        cout << "\n===== SEARCH BY DEPARTMENT =====\n";
        string department = readText("Enter Department: ");
        bool found = false;
        BedNode* temp = head;
        while (temp != NULL)
        {
            if (toLowerCase(temp->department) ==toLowerCase(department))
            {
                if (!found)
                    displayHeader();

                displayRow(temp);
                found = true;
            }
            temp = temp->next;
        }
        if (!found)
            cout << "No beds found in this department.\n";
    }

    void searchByPatientID()
    {
        cout << "\n===== SEARCH BY PATIENT ID =====\n";
        string id = readText("Enter Patient ID: ");
        BedNode* temp = searchByPatient(id);
        if (temp == NULL)
        {
            cout << "No bed allocated to this patient.\n";
            return;
        }
        displayHeader();
        displayRow(temp);
    }

    void displayAvailableBeds()
    {
        cout << "\n===== AVAILABLE BEDS =====\n";
        BedNode* temp = head;
        bool found = false;
        int count = 0;
        while (temp != NULL)
        {
            if (temp->status == "Available")
            {
                if (!found)
                    displayHeader();

                displayRow(temp);
                found = true;
                count++;
            }
            temp = temp->next;
        }
        if (!found)
            cout << "No available beds.\n";

        cout << "\nAvailable Beds: " << count << endl;
    }

    void allocateBed()
    {
        cout << "\n===== ALLOCATE BED =====\n";
        int id = readInt("Enter Bed ID: ");
        BedNode* temp = searchByID(id);
        if (temp == NULL)
        {
            cout << "Bed not found.\n";
            return;
        }
        if (temp->status != "Available")
        {
            cout << "Bed is not available for allocation.\n";
            return;
        }
        string patientID = readText("Enter Patient ID: ");
        if (!validPatientID(patientID))
        {
            cout << "Invalid Patient ID.\n";
            return;
        }

        if (searchByPatient(patientID) != NULL)
        {
            cout << "This patient already has an allocated bed.\n";
            return;
        }

        temp->patientID = patientID;
        temp->status = "Occupied";
        saveBeds();
        cout << "\nBed allocated successfully!\n";
    }

    void dischargePatient()
    {
        cout << "\n===== DISCHARGE PATIENT =====\n";
        string id = readText("Enter Patient ID: ");
        BedNode* temp = searchByPatient(id);
        if (temp == NULL)
        {
            cout << "Patient not found in occupied beds.\n";
            return;
        }
        temp->patientID = "";
        temp->status = "Available";
        saveBeds();
        cout << "Patient discharged successfully.\n";
        cout << "Bed " << temp->bedID << " is now Available.\n";
    }

    void toggleMaintenance()
    {
        cout << "\n===== MAINTENANCE MANAGEMENT =====\n";
        int id = readInt("Enter Bed ID: ");
        BedNode* temp = searchByID(id);
        if (temp == NULL)
        {
            cout << "Bed not found.\n";
            return;
        }
        if (temp->status == "Occupied")
        {
            cout << "Cannot change maintenance status of an occupied bed.\n";
            return;
        }

        if (temp->status == "Available")
        {
            temp->status = "Maintenance";
            cout << "Bed marked for maintenance.\n";
        }
        else
        {
            temp->status = "Available";
            cout << "Bed is now Available.\n";
        }
        saveBeds();
    }

    void sortBeds()
    {
        cout << "\n===== SORT BEDS BY ID =====\n";
        if (head == NULL || head->next == NULL)
        {
            cout << "Not enough beds to sort.\n";
            return;
        }
        bool swapped;
        do
        {
            swapped = false;
            BedNode* temp = head;
            while (temp->next != NULL)
            {
                if (temp->bedID > temp->next->bedID)
                {
                    swap(temp->bedID, temp->next->bedID);
                    swap(temp->department, temp->next->department);
                    swap(temp->wardType, temp->next->wardType);
                    swap(temp->status, temp->next->status);
                    swap(temp->patientID, temp->next->patientID);

                    swapped = true;
                }
                temp = temp->next;
            }

        } while (swapped);
        saveBeds();
        cout << "Beds sorted successfully.\n";
        displayBeds();
    }

    void occupancyReport()
    {
        cout << "\n===== BED OCCUPANCY REPORT =====\n";
        int total = 0;
        int occupied = 0;
        int available = 0;
        int maintenance = 0;
        BedNode* temp = head;
        while (temp != NULL)
        {
            total++;

            if (temp->status == "Occupied")
                occupied++;
            else if (temp->status == "Available")
                available++;
            else if (temp->status == "Maintenance")
                maintenance++;

            temp = temp->next;
        }

        cout << "\nTotal Beds       : " << total << endl;
        cout << "Occupied Beds    : " << occupied << endl;
        cout << "Available Beds   : " << available << endl;
        cout << "Maintenance Beds : " << maintenance << endl;

        if (total > 0)
        {
            double percentage = (occupied * 100.0) / total;
            cout << fixed << setprecision(2);
            cout << "Occupancy Rate   : " << percentage << "%\n";
        }
    }
};
int readMenuChoice()
{
    string input;
    int choice;
    stringstream ss;
    while (true)
    {
        cout << "Enter your choice: ";
        getline(cin, input);
        ss.clear();
        ss.str(input);
        if (ss >> choice)
        {
            ss >> ws;
            if (ss.eof())
                return choice;
        }

        cout << "Invalid input. Enter a number.\n";
    }
}
#if 0
int main()
{
    BedManager manager;
    int choice;
    do
    {
        cout << "\n===== BED MANAGEMENT SYSTEM =====\n";
        cout << "1. Add Bed\n";
        cout << "2. Remove Bed\n";
        cout << "3. Display All Beds\n";
        cout << "4. Search Bed by ID\n";
        cout << "5. Search by Department\n";
        cout << "6. Search by Patient ID\n";
        cout << "7. Display Available Beds\n";
        cout << "8. Allocate Bed\n";
        cout << "9. Discharge Patient\n";
        cout << "10. Change Maintenance Status\n";
        cout << "11. Sort Beds\n";
        cout << "12. Occupancy Report\n";
        cout << "0. Exit\n";

        choice = readMenuChoice();
        system("cls");
        switch (choice)
        {
            case 1:
                manager.addBed();
                break;

            case 2:
                manager.removeBed();
                break;

            case 3:
                manager.displayBeds();
                break;

            case 4:
                manager.searchBedByID();
                break;

            case 5:
                manager.searchByDepartment();
                break;

            case 6:
                manager.searchByPatientID();
                break;

            case 7:
                manager.displayAvailableBeds();
                break;

            case 8:
                manager.allocateBed();
                break;

            case 9:
                manager.dischargePatient();
                break;

            case 10:
                manager.toggleMaintenance();
                break;

            case 11:
                manager.sortBeds();
                break;

            case 12:
                manager.occupancyReport();
                break;

            case 0:
                cout << "Exiting Bed Management System...\n";
                break;

            default:
                cout << "Invalid choice. Try again.\n";
        }

        if (choice != 0)
        {
            cout << "\nPress Enter to return to menu...";
            cin.get();
            system("cls");
        }

    } while (choice != 0);

    return 0;
}
#endif