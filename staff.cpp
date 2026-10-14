#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;

class Staff
{
public:
    int id;
    string name;
    string role;
    string department;
    string shift;
    string status;

    void input()
    {
        cout << "Enter Staff ID: ";
        cin >> id;
        cin.ignore(10000, '\n');

        cout << "Enter Staff Name: ";
        getline(cin, name);

        cout << "Enter Staff Role (Nurse/Receptionist/Technician): ";
        getline(cin, role);

        cout << "Enter Department: ";
        getline(cin, department);

        cout << "Enter Shift (Morning/Evening/Night): ";
        getline(cin, shift);

        cout << "Enter Availability (Available/Unavailable): ";
        getline(cin, status);
    }

    void display()
    {
        cout << "\nStaff ID: " << id;
        cout << "\nName: " << name;
        cout << "\nRole: " << role;
        cout << "\nDepartment: " << department;
        cout << "\nShift: " << shift;
        cout << "\nAvailability: " << status;
        cout << "\n--------------------------------\n";
    }
};

bool staffIdExists(int id)
{
    ifstream file("staff.txt");
    string line;

    while (getline(file, line))
    {
        stringstream ss(line);
        string value;

        if (getline(ss, value, '|') && stoi(value) == id)
            return true;
    }

    return false;
}

bool staffIdValidForNewRecord(int id)
{
    if (staffIdExists(id))
        return false;

    ifstream file("doctors.txt");
    string line;

    while (getline(file, line))
    {
        stringstream ss(line);
        string value;

        if (getline(ss, value, '|') && stoi(value) == id)
            return false;
    }

    return true;
}

void saveStaff(Staff s)
{
    ofstream file("staff.txt", ios::app);

    file << s.id << '|'
         << s.name << '|'
         << s.role << '|'
         << s.department << '|'
         << s.shift << '|'
         << s.status << '\n';
}

vector<Staff> loadStaff()
{
    vector<Staff> staffList;
    ifstream file("staff.txt");
    string line;

    while (getline(file, line))
    {
        stringstream ss(line);
        Staff s;
        string value;

        if (!getline(ss, value, '|'))
            continue;

        try
        {
            s.id = stoi(value);
        }
        catch (...)
        {
            continue;
        }

        if (!getline(ss, s.name, '|'))
            continue;

        if (!getline(ss, s.role, '|'))
            continue;

        if (!getline(ss, s.department, '|'))
            continue;

        if (!getline(ss, s.shift, '|'))
            continue;

        if (!getline(ss, s.status))
            continue;

        staffList.push_back(s);
    }

    return staffList;
}

void saveAllStaff(vector<Staff> staffList)
{
    ofstream file("staff.txt");

    for (Staff s : staffList)
    {
        file << s.id << '|'
             << s.name << '|'
             << s.role << '|'
             << s.department << '|'
             << s.shift << '|'
             << s.status << '\n';
    }
}

void sortStaff(vector<Staff> &staffList)
{
    if (staffList.size() <= 1)
        return;

    int mid = staffList.size() / 2;

    vector<Staff> left(staffList.begin(), staffList.begin() + mid);
    vector<Staff> right(staffList.begin() + mid, staffList.end());

    sortStaff(left);
    sortStaff(right);

    int i = 0, j = 0;
    staffList.clear();

    while (i < left.size() && j < right.size())
    {
        if (left[i].name <= right[j].name)
            staffList.push_back(left[i++]);
        else
            staffList.push_back(right[j++]);
    }

    while (i < left.size())
        staffList.push_back(left[i++]);

    while (j < right.size())
        staffList.push_back(right[j++]);
}

void addStaff()
{
    Staff s;
    s.input();

    if (s.id <= 0)
    {
        cout << "\nStaff ID must be a positive number.\n";
        return;
    }

    if (!staffIdValidForNewRecord(s.id))
    {
        cout << "\nThis ID already belongs to a doctor or staff member.\n";
        cout << "Please use a unique ID.\n";
        return;
    }

    saveStaff(s);
    cout << "\nStaff member registered successfully.\n";
}

void viewStaff()
{
    vector<Staff> staffList = loadStaff();

    if (staffList.empty())
    {
        cout << "\nNo staff records found.\n";
        return;
    }

    sortStaff(staffList);

    cout << "\n========== STAFF LIST ==========\n";

    for (Staff s : staffList)
        s.display();
}

void searchStaff()
{
    int id;

    cout << "Enter Staff ID to search: ";
    cin >> id;

    vector<Staff> staffList = loadStaff();

    unordered_map<int, int> staffIndex;

    for (int i = 0; i < staffList.size(); i++)
        staffIndex[staffList[i].id] = i;

    if (staffIndex.find(id) != staffIndex.end())
        staffList[staffIndex[id]].display();
    else
        cout << "\nStaff member not found.\n";
}

void updateStaff()
{
    int id;
    bool found = false;

    cout << "Enter Staff ID to update: ";
    cin >> id;

    vector<Staff> staffList = loadStaff();

    for (Staff &s : staffList)
    {
        if (s.id == id)
        {
            found = true;

            cin.ignore(10000, '\n');

            cout << "Enter New Name: ";
            getline(cin, s.name);

            cout << "Enter New Role: ";
            getline(cin, s.role);

            cout << "Enter New Department: ";
            getline(cin, s.department);

            cout << "Enter New Shift: ";
            getline(cin, s.shift);

            cout << "Enter New Availability: ";
            getline(cin, s.status);

            break;
        }
    }

    if (found)
    {
        saveAllStaff(staffList);
        cout << "\nStaff details updated successfully.\n";
    }
    else
    {
        cout << "\nStaff member not found.\n";
    }
}

void staffMenu()
{
    int choice;

    do
    {
        cout << "\n========== STAFF MANAGEMENT ==========\n";
        cout << "1. Add Staff\n";
        cout << "2. View All Staff\n";
        cout << "3. Search Staff\n";
        cout << "4. Update Staff Details\n";
        cout << "5. Back to Main Menu\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Enter a number.\n";
            continue;
        }

        switch (choice)
        {
        case 1:
            addStaff();
            break;

        case 2:
            viewStaff();
            break;

        case 3:
            searchStaff();
            break;

        case 4:
            updateStaff();
            break;

        case 5:
            cout << "Returning to Main Menu...\n";
            break;

        default:
            cout << "Invalid choice.\n";
        }

    } while (choice != 5);
}