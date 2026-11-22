#include <iostream>
#include <string>
#include <limits>
using namespace std;

const int TOTAL_SERVICES = 7;

string services[TOTAL_SERVICES] = {
    "OPD Consultation",
    "Emergency Department",
    "Laboratory",
    "X-Ray / Radiology",
    "Pharmacy",
    "Follow-up Consultation",
    "Specialist Consultation"
};

struct QueueNode
{
    int patientID;
    string patientName;
    int priority;
    QueueNode* next;

    QueueNode(int id, string name, int p) 
{
        patientID = id;
        patientName = name;
        priority = p;
        next = nullptr;
    }
};
class HospitalWaitingQueue 
{

private:

    QueueNode* front[TOTAL_SERVICES];
    QueueNode* rear[TOTAL_SERVICES];

public:

    HospitalWaitingQueue()
{

        for (int i = 0; i < TOTAL_SERVICES; i++) 
        {
            front[i] = nullptr;
            rear[i] = nullptr;
        }
    }
    ~HospitalWaitingQueue() 
{

        for (int i = 0; i < TOTAL_SERVICES; i++) 
        {

            while (front[i] != nullptr) 
            {

                QueueNode* temp = front[i];
                front[i] = front[i]->next;

                delete temp;
            }
        }
    }
    bool isEmpty(int service) 
{

        return front[service] == nullptr;
    }
    bool contains(int id, int service) 
{

        QueueNode* temp = front[service];

        while (temp != nullptr) 
        {

            if (temp->patientID == id)
                return true;

            temp = temp->next;
        }

        return false;
    }

    void enqueue(int id, string name, int service, int priority) 
{

        if (service < 0 || service >= TOTAL_SERVICES) 
        {
            cout << "Invalid department!\n";
            return;
        }

        if (priority != 1 && priority != 2) 
        {
            cout << "Invalid priority!\n";
            return;
        }

        if (contains(id, service)) 
        {
            cout << "Patient already exists in this queue!\n";
            return;
        }

        QueueNode* newNode = new QueueNode(id, name, priority);

        if (front[service] == nullptr ||priority < front[service]->priority) 
        {

            newNode->next = front[service];
            front[service] = newNode;

            if (rear[service] == nullptr)
                rear[service] = newNode;
        }

        else 
        {

            QueueNode* temp = front[service];

            while (temp->next != nullptr &&
                   temp->next->priority <= priority)
              {

                temp = temp->next;
            }

            newNode->next = temp->next;
            temp->next = newNode;

            if (newNode->next == nullptr)
                rear[service] = newNode;
        }

        cout << "\nPatient added successfully!\n";
    }

    void dequeue(int service)
{

        if (isEmpty(service)) 
        {
            cout << "\nNo patients waiting!\n";
            return;
        }

        QueueNode* temp = front[service];

        cout << "\n NEXT PATIENT \n";

        cout << "Patient ID: " << temp->patientID << endl;
        cout << "Patient Name: " << temp->patientName << endl;
        cout << "Department: " << services[service] << endl;
        cout << "Priority: " << (temp->priority == 1 ? "Staff-assigned urgent" : "Normal") << endl;
        front[service] = front[service]->next;

        if (front[service] == nullptr)
            rear[service] = nullptr;

        delete temp;

        cout << "\nPatient called successfully!\n";
    }

    void display(int service)
{

        if (isEmpty(service))
        {
            cout << "\nQueue is empty!\n";
            return;
        }

        QueueNode* temp = front[service];
        int position = 1;

        cout << "\n "<< services[service]<< " WAITING LIST \n";

        while (temp != nullptr) {

            cout << "\nPosition: " << position++;
            cout << "\nPatient ID: " << temp->patientID;
            cout << "\nPatient Name: "<< temp->patientName;
            cout << "\nPriority: " << (temp->priority == 1 ? "Staff-assigned urgent" : "Normal");
            cout << "\n-------------------------\n";

            temp = temp->next;
        }
    }

    void displayAll()
{

        for (int i = 0; i < TOTAL_SERVICES; i++)
          {

            cout << "\nDepartment: " << services[i];

            display(i);
        }
    }

    void searchPatient(int id, int service)
{

        QueueNode* temp = front[service];
        int position = 1;

        while (temp != nullptr)
          {

            if (temp->patientID == id) {

                cout << "\nPatient Found!\n";
                cout << "Patient ID: " << temp->patientID << endl;
                cout << "Patient Name: "<< temp->patientName << endl;
                cout << "Waiting Position: "<< position << endl;
                cout << "Department: "<< services[service] << endl;
                return;
            }

            temp = temp->next;
            position++;
        }

        cout << "\nPatient not found!\n";
    }
    void removePatient(int id, int service) 
{

        QueueNode* temp = front[service];
        QueueNode* previous = nullptr;

        while (temp != nullptr &&
               temp->patientID != id) 
        {

            previous = temp;
            temp = temp->next;
        }

        if (temp == nullptr) 
        {
            cout << "\nPatient not found!\n";
            return;
        }

        if (previous == nullptr)
            front[service] = temp->next;

        else
            previous->next = temp->next;

        if (temp == rear[service])
            rear[service] = previous;

        delete temp;

        cout << "\nPatient removed successfully!\n";
    }
};

int selectService() {

    cout << "\n HOSPITAL DEPARTMENTS \n";

    for (int i = 0; i < TOTAL_SERVICES; i++)
      {

        cout << i + 1 << ". "<< services[i] << endl;
    }

    int choice;

    cout << "\nSelect Department: ";
    cin >> choice;

    if (cin.fail() || choice < 1 ||choice > TOTAL_SERVICES) 
    {

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid department!\n";
        return -1;
    }

    return choice - 1;
}
#if 0
int main() 
{

    HospitalWaitingQueue hospital;

    int choice;

    do 
    {
        cout << " HOSPITAL WAITING QUEUE MANAGEMENT\n";
        cout << "1. Add Patient to Queue\n";
        cout << "2. Call Next Patient\n";
        cout << "3. Display Waiting List\n";
        cout << "4. Search Patient\n";
        cout << "5. Remove Patient\n";
        cout << "6. Display All Department Queues\n";
        cout << "7. Exit\n";

        cout << "\nEnter your choice: ";
        cin >> choice;

        if (cin.fail()) 
        {

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "Invalid input!\n";
            continue;
        }

        switch (choice)
          {

            case 1: 
              {

                int id, service, priority;
                string name;

                cout << "\nEnter Patient ID: ";
                cin >> id;

                if (cin.fail())
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid ID!\n";
                    break;
                }

                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                cout << "Enter Patient Name: ";
                getline(cin, name);

                if (name.empty()) 
                {
                    cout << "Name cannot be empty!\n";
                    break;
                }

                service = selectService();

                if (service == -1)
                    break;

                cout << "\n1. Staff-assigned urgent\n";
                cout << "2. Normal\n";

                cout << "Enter Priority: ";
                cin >> priority;

                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid priority!\n";
                    break;
                }

                hospital.enqueue(id, name, service, priority);
                break;
            }

            case 2: 
              {

                int service = selectService();

                if (service != -1)
                    hospital.dequeue(service);

                break;
            }

            case 3:
              {

                int service = selectService();

                if (service != -1)
                    hospital.display(service);

                break;
            }

            case 4:
              {
               int id;
                cout << "Enter Patient ID: ";
                cin >> id;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid ID!\n";
                    break;
                }

                int service = selectService();
                if (service != -1)
                    hospital.searchPatient(id, service);

                break;
            }

            case 5:
              {

                int id;

                cout << "Enter Patient ID: ";
                cin >> id;

                if (cin.fail())
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid ID!\n";
                    break;
                }

                int service = selectService();
                if (service != -1)
                    hospital.removePatient(id, service);
                break;
            }

            case 6:

                hospital.displayAll();
                break;

            case 7:

                cout << "\nThank you for using Hospital "<< "Waiting Queue Management System!\n";

                break;

            default:

                cout << "\nInvalid choice! Try again.\n";
        }

    } while (choice != 7);

    return 0;
}
#endif