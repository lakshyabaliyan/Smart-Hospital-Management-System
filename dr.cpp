#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;

class Doctor
{
public:
    int id;
    string name;
    string specialization;
    string department;
    string shift;
    string status;

    void input()
    {
        cout << "Enter Doctor ID: ";
        cin >> id;
        cin.ignore(10000, '\n');

        cout << "Enter Doctor Name: ";
        getline(cin, name);

        cout << "Enter Specialization: ";
        getline(cin, specialization);

        cout << "Enter Department: ";
        getline(cin, department);

        cout << "Enter Shift (Morning/Evening/Night): ";
        getline(cin, shift);

        cout << "Enter Availability (Available/Unavailable): ";
        getline(cin, status);
    }

    void display()
    {
        cout << "\nDoctor ID: " << id;
        cout << "\nName: " << name;
        cout << "\nSpecialization: " << specialization;
        cout << "\nDepartment: " << department;
        cout << "\nShift: " << shift;
        cout << "\nAvailability: " << status;
        cout << "\n--------------------------------\n";
    }
};

bool doctorIdExists(int id)
{
    ifstream file("doctors.txt");
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

bool doctorIdValidForNewRecord(int id)
{
    if (doctorIdExists(id))
        return false;

    ifstream file("staff.txt");
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

void saveDoctor(Doctor d)
{
    ofstream file("doctors.txt", ios::app);

    file << d.id << '|'
         << d.name << '|'
         << d.specialization << '|'
         << d.department << '|'
         << d.shift << '|'
         << d.status << '\n';
}

vector<Doctor> loadDoctors()
{
    vector<Doctor> doctors;
    ifstream file("doctors.txt");
    string line;

    while (getline(file, line))
    {
        stringstream ss(line);
        Doctor d;
        string value;

        if (!getline(ss, value, '|'))
            continue;

        try
        {
            d.id = stoi(value);
        }
        catch (...)
        {
            continue;
        }

        if (!getline(ss, d.name, '|'))
            continue;

        if (!getline(ss, d.specialization, '|'))
            continue;

        if (!getline(ss, d.department, '|'))
            continue;

        if (!getline(ss, d.shift, '|'))
            continue;

        if (!getline(ss, d.status))
            continue;

        doctors.push_back(d);
    }

    return doctors;
}

void saveAllDoctors(vector<Doctor> doctors)
{
    ofstream file("doctors.txt");

    for (Doctor d : doctors)
    {
        file << d.id << '|'
             << d.name << '|'
             << d.specialization << '|'
             << d.department << '|'
             << d.shift << '|'
             << d.status << '\n';
    }
}

void sortDoctors(vector<Doctor> &doctors)
{
    if (doctors.size() <= 1)
        return;

    int mid = doctors.size() / 2;

    vector<Doctor> left(doctors.begin(), doctors.begin() + mid);
    vector<Doctor> right(doctors.begin() + mid, doctors.end());

    sortDoctors(left);
    sortDoctors(right);

    int i = 0, j = 0;
    doctors.clear();

    while (i < left.size() && j < right.size())
    {
        if (left[i].name <= right[j].name)
            doctors.push_back(left[i++]);
        else
            doctors.push_back(right[j++]);
    }

    while (i < left.size())
        doctors.push_back(left[i++]);

    while (j < right.size())
        doctors.push_back(right[j++]);
}

void addDoctor()
{
    Doctor d;
    d.input();

    if (d.id <= 0)
    {
        cout << "\nDoctor ID must be a positive number.\n";
        return;
    }

    if (!doctorIdValidForNewRecord(d.id))
    {
        cout << "\nThis ID already belongs to a doctor or staff member.\n";
        cout << "Please use a unique ID.\n";
        return;
    }

    saveDoctor(d);
    cout << "\nDoctor registered successfully.\n";
}

void viewDoctors()
{
    vector<Doctor> doctors = loadDoctors();

    if (doctors.empty())
    {
        cout << "\nNo doctor records found.\n";
        return;
    }

    sortDoctors(doctors);

    cout << "\n========== DOCTOR LIST ==========\n";

    for (Doctor d : doctors)
        d.display();
}

void searchDoctor()
{
    int id;

    cout << "Enter Doctor ID to search: ";
    cin >> id;

    vector<Doctor> doctors = loadDoctors();

    unordered_map<int, int> doctorIndex;

    for (int i = 0; i < doctors.size(); i++)
        doctorIndex[doctors[i].id] = i;

    if (doctorIndex.find(id) != doctorIndex.end())
        doctors[doctorIndex[id]].display();
    else
        cout << "\nDoctor not found.\n";
}

void updateDoctor()
{
    int id;
    bool found = false;

    cout << "Enter Doctor ID to update: ";
    cin >> id;

    vector<Doctor> doctors = loadDoctors();

    for (Doctor &d : doctors)
    {
        if (d.id == id)
        {
            found = true;

            cin.ignore(10000, '\n');

            cout << "Enter New Name: ";
            getline(cin, d.name);

            cout << "Enter New Specialization: ";
            getline(cin, d.specialization);

            cout << "Enter New Department: ";
            getline(cin, d.department);

            cout << "Enter New Shift: ";
            getline(cin, d.shift);

            cout << "Enter New Availability: ";
            getline(cin, d.status);

            break;
        }
    }

    if (found)
    {
        saveAllDoctors(doctors);
        cout << "\nDoctor details updated successfully.\n";
    }
    else
    {
        cout << "\nDoctor not found.\n";
    }
}

void doctorMenu()
{
    int choice;

    do
    {
        cout << "\n========== DOCTOR MANAGEMENT ==========\n";
        cout << "1. Add Doctor\n";
        cout << "2. View All Doctors\n";
        cout << "3. Search Doctor\n";
        cout << "4. Update Doctor Details\n";
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
            addDoctor();
            break;

        case 2:
            viewDoctors();
            break;

        case 3:
            searchDoctor();
            break;

        case 4:
            updateDoctor();
            break;

        case 5:
            cout << "Returning to Main Menu...\n";
            break;

        default:
            cout << "Invalid choice.\n";
        }

    } while (choice != 5);
}