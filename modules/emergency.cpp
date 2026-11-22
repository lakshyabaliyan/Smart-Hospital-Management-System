#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <string>
using namespace std;

class EmergencyCase
{
public:
    int caseID;
    int patientID;
    string patientName;
    int age;
    string department;
    int priority;
    int doctorID;
    string doctorName;
    string status;
    int arrival;
};

class EmergencyManagement
{
    vector<EmergencyCase> heap;
    list<EmergencyCase> history;

    int caseCounter;
    int arrivalCounter;

    void swapCase(EmergencyCase &a, EmergencyCase &b)
    {
        EmergencyCase temp = a;
        a = b;
        b = temp;
    }

    bool higherPriority(EmergencyCase a, EmergencyCase b)
    {
        if (a.priority == b.priority)
            return a.arrival < b.arrival;

        return a.priority < b.priority;
    }

    void insertHeap(EmergencyCase e)
    {
        heap.push_back(e);

        int i = heap.size() - 1;

        while (i > 0)
        {
            int parent = (i - 1) / 2;

            if (higherPriority(heap[i], heap[parent]))
            {
                swapCase(heap[i], heap[parent]);
                i = parent;
            }
            else
                break;
        }
    }

    EmergencyCase removeHeap()
    {
        EmergencyCase e = heap[0];

        heap[0] = heap.back();
        heap.pop_back();

        int i = 0;

        while (true)
        {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int smallest = i;

            if (left < heap.size() &&
                higherPriority(heap[left], heap[smallest]))
                smallest = left;

            if (right < heap.size() &&
                higherPriority(heap[right], heap[smallest]))
                smallest = right;

            if (smallest != i)
            {
                swapCase(heap[i], heap[smallest]);
                i = smallest;
            }
            else
                break;
        }

        return e;
    }

    void updateHistory(EmergencyCase e)
    {
        for (list<EmergencyCase>::iterator it = history.begin();
             it != history.end(); it++)
        {
            if (it->caseID == e.caseID)
            {
                *it = e;
                return;
            }
        }
    }

    void saveFile()
    {
        ofstream file("txt-files/emergency.txt");

        for (list<EmergencyCase>::iterator it = history.begin();
             it != history.end(); it++)
        {
            file << it->caseID << "|"
                 << it->patientID << "|"
                 << it->patientName << "|"
                 << it->age << "|"
                 << it->department << "|"
                 << it->priority << "|"
                 << it->doctorID << "|"
                 << it->doctorName << "|"
                 << it->status << "|"
                 << it->arrival << endl;
        }

        file.close();
    }

    void loadFile()
    {
        ifstream file("txt-files/emergency.txt");
        string line;

        while (getline(file, line))
        {
            string data[10];
            int count = 0;
            size_t pos;

            while ((pos = line.find('|')) != string::npos && count < 9)
            {
                data[count++] = line.substr(0, pos);
                line.erase(0, pos + 1);
            }

            data[count] = line;

            if (count != 9)
                continue;

            try
            {
                EmergencyCase e;

                e.caseID = stoi(data[0]);
                e.patientID = stoi(data[1]);
                e.patientName = data[2];
                e.age = stoi(data[3]);
                e.department = data[4];
                e.priority = stoi(data[5]);
                e.doctorID = stoi(data[6]);
                e.doctorName = data[7];
                e.status = data[8];
                e.arrival = stoi(data[9]);

                history.push_back(e);

                if (e.status == "Waiting")
                    insertHeap(e);

                if (e.caseID >= caseCounter)
                    caseCounter = e.caseID + 1;

                if (e.arrival >= arrivalCounter)
                    arrivalCounter = e.arrival + 1;
            }
            catch (...)
            {
                continue;
            }
        }

        file.close();
    }

    void displayCase(EmergencyCase e)
    {
        cout << "\nCase ID: " << e.caseID;
        cout << "\nPatient ID: " << e.patientID;
        cout << "\nPatient Name: " << e.patientName;
        cout << "\nAge: " << e.age;
        cout << "\nDepartment: " << e.department;
        cout << "\nPriority: ";

        if (e.priority == 1)
            cout << "Critical";
        else if (e.priority == 2)
            cout << "Serious";
        else
            cout << "Normal";

        cout << "\nDoctor: " << e.doctorName;
        cout << "\nDoctor ID: " << e.doctorID;
        cout << "\nStatus: " << e.status << endl;
    }

public:

    EmergencyManagement()
    {
        caseCounter = 1;
        arrivalCounter = 1;

        loadFile();
    }

    void createEmergency()
    {
        EmergencyCase e;

        cout << "\nEnter Patient ID: ";
        cin >> e.patientID;

        cout << "Enter Patient Name: ";
        cin.ignore();
        getline(cin, e.patientName);

        cout << "Enter Patient Age: ";
        cin >> e.age;

        cout << "\nEnter Department: ";
        cin.ignore();
        getline(cin, e.department);

        cout << "\nSelect Emergency Priority\n";
        cout << "1. Critical\n";
        cout << "2. Serious\n";
        cout << "3. Normal\n";
        cout << "Enter choice: ";
        cin >> e.priority;

        if (e.patientID <= 0 || e.age <= 0 ||
            e.priority < 1 || e.priority > 3)
        {
            cout << "Invalid details.\n";
            return;
        }

        for (list<EmergencyCase>::iterator it = history.begin();
             it != history.end(); it++)
        {
            if (it->patientID == e.patientID &&
                it->status != "Completed")
            {
                cout << "This patient already has an active case.\n";
                return;
            }
        }

        e.caseID = caseCounter++;
        e.doctorID = -1;
        e.doctorName = "Not Assigned";
        e.status = "Waiting";
        e.arrival = arrivalCounter++;

        insertHeap(e);
        history.push_back(e);

        saveFile();

        cout << "\nEmergency case created successfully.\n";
        cout << "Case ID: " << e.caseID << endl;
    }

    void assignDoctor()
    {
        if (heap.empty())
        {
            cout << "No waiting emergency cases.\n";
            return;
        }

        EmergencyCase e = heap[0];

        cout << "\nNext Emergency Case\n";
        displayCase(e);

        int id;
        string name, department;

        cout << "\nEnter Doctor ID: ";
        cin >> id;

        cout << "Enter Doctor Name: ";
        cin.ignore();
        getline(cin, name);

        cout << "Enter Doctor Department: ";
        getline(cin, department);

        if (id <= 0 || department != e.department)
        {
            cout << "Invalid doctor ID or department mismatch.\n";
            return;
        }

        e = removeHeap();

        e.doctorID = id;
        e.doctorName = name;
        e.status = "Assigned";

        updateHistory(e);
        saveFile();

        cout << "\nDoctor assigned successfully.\n";
    }

    void viewWaitingQueue()
    {
        if (heap.empty())
        {
            cout << "No waiting emergency cases.\n";
            return;
        }

        vector<EmergencyCase> temp = heap;

        cout << "\nEmergency Waiting Queue\n";

        while (!temp.empty())
        {
            EmergencyCase e = temp[0];

            cout << "\nCase ID: " << e.caseID;
            cout << "\nPatient: " << e.patientName;
            cout << "\nDepartment: " << e.department;
            cout << "\nPriority: " << e.priority << endl;

            EmergencyCase last = temp.back();
            temp.pop_back();

            if (!temp.empty())
            {
                temp[0] = last;

                int i = 0;

                while (true)
                {
                    int left = 2 * i + 1;
                    int right = 2 * i + 2;
                    int smallest = i;

                    if (left < temp.size() &&
                        higherPriority(temp[left], temp[smallest]))
                        smallest = left;

                    if (right < temp.size() &&
                        higherPriority(temp[right], temp[smallest]))
                        smallest = right;

                    if (smallest != i)
                    {
                        swapCase(temp[i], temp[smallest]);
                        i = smallest;
                    }
                    else
                        break;
                }
            }
        }
    }

    void doctorDashboard()
    {
        int doctorID;

        cout << "\nEnter Doctor ID: ";
        cin >> doctorID;

        cout << "\nAssigned Emergency Cases\n";

        bool found = false;

        for (list<EmergencyCase>::iterator it = history.begin();
             it != history.end(); it++)
        {
            if (it->doctorID == doctorID &&
                it->status != "Completed")
            {
                displayCase(*it);
                found = true;
            }
        }

        if (!found)
        {
            cout << "No assigned cases.\n";
            return;
        }

        int caseID, choice;

        cout << "\nEnter Case ID to update: ";
        cin >> caseID;

        for (list<EmergencyCase>::iterator it = history.begin();
             it != history.end(); it++)
        {
            if (it->caseID == caseID && it->doctorID == doctorID)
            {
                cout << "\n1. Start Treatment\n";
                cout << "2. Complete Treatment\n";
                cout << "Enter choice: ";
                cin >> choice;

                if (choice == 1)
                    it->status = "Treatment Started";
                else if (choice == 2)
                    it->status = "Completed";
                else
                {
                    cout << "Invalid choice.\n";
                    return;
                }

                saveFile();

                cout << "Case status updated successfully.\n";
                return;
            }
        }

        cout << "Invalid Case ID.\n";
    }

    void staffDashboard()
    {
        int choice;

        do
        {
            cout << "\n===== STAFF EMERGENCY DASHBOARD =====\n";
            cout << "1. Create Emergency Case\n";
            cout << "2. View Waiting Queue\n";
            cout << "3. Assign Doctor\n";
            cout << "4. Exit\n";
            cout << "Enter choice: ";
            cin >> choice;

            if (choice == 1)
                createEmergency();
            else if (choice == 2)
                viewWaitingQueue();
            else if (choice == 3)
                assignDoctor();

        } while (choice != 4);
    }

    void doctorDashboardMenu()
    {
        int choice;

        do
        {
            cout << "\n===== DOCTOR EMERGENCY DASHBOARD =====\n";
            cout << "1. View and Update Emergency Cases\n";
            cout << "2. Exit\n";
            cout << "Enter choice: ";
            cin >> choice;

            if (choice == 1)
                doctorDashboard();

        } while (choice != 2);
    }
};
#if 0
int main()
{
    EmergencyManagement emergency;

    int choice;

    do
    {
        cout << "\n===== AROGYA EMERGENCY MODULE =====\n";
        cout << "1. Staff Dashboard\n";
        cout << "2. Doctor Dashboard\n";
        cout << "3. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1)
            emergency.staffDashboard();
        else if (choice == 2)
            emergency.doctorDashboardMenu();

    } while (choice != 3);

    return 0;
}
#endif