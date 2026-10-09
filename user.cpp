#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
using namespace std;

#define SIZE 100

struct User
{
    string username;
    int password;
    string role;
};

struct Node
{
    User data;
    Node *next;
};

Node *table[SIZE];

int hashFunction(string username)
{
    int sum = 0;

    for(int i = 0; i < username.length(); i++)
        sum += username[i];

    return sum % SIZE;
}

int hashPassword(string password)
{
    int hash = 0;

    for(int i = 0; i < password.length(); i++)
        hash = hash * 31 + password[i];

    return hash;
}

bool validPassword(string password)
{
    bool upper = false;
    bool lower = false;
    bool digit = false;
    bool special = false;

    if(password.length() < 6)
        return false;

    for(int i = 0; i < password.length(); i++)
    {
        if(isupper(password[i]))
            upper = true;
        else if(islower(password[i]))
            lower = true;
        else if(isdigit(password[i]))
            digit = true;
        else
            special = true;
    }

    return upper && lower && digit && special;
}

void initialize()
{
    for(int i = 0; i < SIZE; i++)
        table[i] = NULL;
}

bool usernameExists(string username)
{
    int index = hashFunction(username);
    Node *temp = table[index];

    while(temp != NULL)
    {
        if(temp->data.username == username)
            return true;

        temp = temp->next;
    }

    return false;
}

void insertUser(User u)
{
    int index = hashFunction(u.username);

    Node *newNode = new Node;

    newNode->data = u;
    newNode->next = table[index];

    table[index] = newNode;
}

void saveUser(User u)
{
    ofstream file("users.txt", ios::app);

    file << u.username << " "
         << u.password << " "
         << u.role << endl;

    file.close();
}

void loadUsers()
{
    ifstream file("users.txt");

    User u;

    while(file >> u.username >> u.password >> u.role)
        insertUser(u);

    file.close();
}

void registerUser()
{
    User u;
    string password;

    cout << "\nEnter username: ";
    cin >> ws;
    getline(cin, u.username);

    if(u.username.find(' ') != string::npos)
    {
        cout << "Username cannot contain spaces!\n";
        return;
    }

    if(usernameExists(u.username))
    {
        cout << "Username already exists!\n";
        return;
    }

    cout << "\nPassword must contain:\n";
    cout << "- Minimum 6 characters\n";
    cout << "- One uppercase letter\n";
    cout << "- One lowercase letter\n";
    cout << "- One digit\n";
    cout << "- One special character\n";

    cout << "\nEnter password: ";
    cin >> password;

    if(!validPassword(password))
    {
        cout << "Invalid password!\n";
        return;
    }

    u.password = hashPassword(password);

    cout << "\nSelect Role\n";
    cout << "1. Patient\n";
    cout << "2. Doctor\n";
    cout << "3. Nurse\n";
    cout << "4. Receptionist\n";
    cout << "5. Admin\n";

    int choice;
    cout << "Enter choice: ";
    cin >> choice;

    if(choice == 1)
        u.role = "Patient";
    else if(choice == 2)
        u.role = "Doctor";
    else if(choice == 3)
        u.role = "Nurse";
    else if(choice == 4)
        u.role = "Receptionist";
    else if(choice == 5)
        u.role = "Admin";
    else
    {
        cout << "Invalid role!\n";
        return;
    }

    insertUser(u);
    saveUser(u);

    cout << "\nRegistration successful!\n";
}

void login()
{
    string username, password;

    cout << "\nEnter username: ";
    cin >> username;

    cout << "Enter password: ";
    cin >> password;

    int index = hashFunction(username);
    Node *temp = table[index];

    while(temp != NULL)
    {
        if(temp->data.username == username)
        {
            if(temp->data.password == hashPassword(password))
            {
                cout << "\n================================\n";
                cout << "        LOGIN SUCCESSFUL\n";
                cout << "================================\n";
                cout << "Username : " << username << endl;
                cout << "Role     : " << temp->data.role << endl;
                cout << "Dashboard: " << temp->data.role << endl;
                cout << "================================\n";
            }
            else
            {
                cout << "Wrong password!\n";
            }

            return;
        }

        temp = temp->next;
    }

    cout << "Username not found!\n";
}

void displayUsers()
{
    cout << "\n========== DOCTORS ==========\n";

    bool found = false;

    for(int i = 0; i < SIZE; i++)
    {
        Node *temp = table[i];

        while(temp != NULL)
        {
            if(temp->data.role == "Doctor")
            {
                cout << temp->data.username << endl;
                found = true;
            }

            temp = temp->next;
        }
    }

    if(!found)
        cout << "No doctors registered.\n";


    cout << "\n========== PATIENTS ==========\n";

    found = false;

    for(int i = 0; i < SIZE; i++)
    {
        Node *temp = table[i];

        while(temp != NULL)
        {
            if(temp->data.role == "Patient")
            {
                cout << temp->data.username << endl;
                found = true;
            }

            temp = temp->next;
        }
    }

    if(!found)
        cout << "No patients registered.\n";


    cout << "\n========== NURSES ==========\n";

    found = false;

    for(int i = 0; i < SIZE; i++)
    {
        Node *temp = table[i];

        while(temp != NULL)
        {
            if(temp->data.role == "Nurse")
            {
                cout << temp->data.username << endl;
                found = true;
            }

            temp = temp->next;
        }
    }

    if(!found)
        cout << "No nurses registered.\n";


    cout << "\n========== RECEPTIONISTS ==========\n";

    found = false;

    for(int i = 0; i < SIZE; i++)
    {
        Node *temp = table[i];

        while(temp != NULL)
        {
            if(temp->data.role == "Receptionist")
            {
                cout << temp->data.username << endl;
                found = true;
            }

            temp = temp->next;
        }
    }

    if(!found)
        cout << "No receptionists registered.\n";


    cout << "\n========== ADMINS ==========\n";

    found = false;

    for(int i = 0; i < SIZE; i++)
    {
        Node *temp = table[i];

        while(temp != NULL)
        {
            if(temp->data.role == "Admin")
            {
                cout << temp->data.username << endl;
                found = true;
            }

            temp = temp->next;
        }
    }

    if(!found)
        cout << "No admins registered.\n";
}

int main()
{
    initialize();
    loadUsers();

    int choice;

    do
    {
        cout << "\n============================================\n";
        cout << "       SMART HOSPITAL MANAGEMENT SYSTEM\n";
        cout << "============================================\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Display Users\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch(choice)
        {
            case 1:
                registerUser();
                break;

            case 2:
                login();
                break;

            case 3:
                displayUsers();
                break;

            case 4:
                cout << "Thank you!\n";
                break;

            default:
                cout << "Invalid choice!\n";
        }

    } while(choice != 4);

    return 0;
}