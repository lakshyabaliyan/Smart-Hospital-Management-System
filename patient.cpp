#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <algorithm>

using namespace std;
struct Medication
{
    string name;
    string dosage;
    string frequency;
    string duration;
};
struct MedicalTest
{
    string name;
    string date;
    string result;
};

class Patient
{
public: 
    int patientID;
    string name;
    int age;
    string gender;
    string bloodGroup;
    string phone;
    string address;
    string emergencyContact;
    string admissionDate;
    string dischargeDate;
    string department;
    string doctor;
    string reasonForAdmission;
    string diagnosis;
    string medicalHistory;
    string previousSurgeries;
    string chronicConditions;
    string familyHistory;
    string allergies;
    vector<Medication> medications;
    vector<MedicalTest> tests;
    string dischargeCondition;
    string followUpDate;
    string instructions;
    void addPatient();
    void displayPatient() const;
    void updatePatient();
    void addMedication();
    void addTest();

    string serialize() const;
    static Patient deserialize(const string& line);
};
const string DATA_FILE = "patients.db";
string sanitize(string value)
{
    replace(value.begin(), value.end(), '|', '/');
    return value;
}
string getLine(const string& prompt)
{
    cout << prompt;
    string value;
    getline(cin, value);
    return sanitize(value);
}
int getInt(const string& prompt)
{
    while (true)
    {
        cout << prompt;
        int value;
        if (cin >> value)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cout << "Invalid input! Please enter a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}
vector<string> split(const string& line, char delimiter)
{
    vector<string> result;
    string item;
    stringstream ss(line);
    while (getline(ss, item, delimiter))
    {
        result.push_back(item);
    }
    return result;
}
void Patient::addPatient()
{
    cout << "\n";
     cout << "          REGISTER NEW PATIENT\n";
    patientID = getInt("Patient ID: ");
    name = getLine("Full Name: ");
    age = getInt("Age: ");
    gender = getLine("Gender: ");
    bloodGroup = getLine("Blood Group: ");
    phone = getLine("Phone Number: ");
    address = getLine("Address: ");
    emergencyContact = getLine("Emergency Contact: ");
    cout << "\n ADMISSION DETAILS \n";
    admissionDate = getLine("Date of Admission (DD/MM/YYYY): ");
    dischargeDate = getLine("Date of Discharge (leave blank if admitted): ");
    department = getLine("Department: ");
    doctor = getLine("Doctor Assigned: ");
    reasonForAdmission = getLine("Reason for Admission: ");
    diagnosis = getLine("Diagnosis: ");
    cout << "\n MEDICAL HISTORY \n";
    medicalHistory = getLine("Previous Illnesses: ");
    previousSurgeries = getLine("Previous Surgeries: ");
    chronicConditions =
        getLine("Chronic Conditions: ");

    familyHistory =
        getLine("Family Medical History: ");

    allergies =
        getLine("Allergies: ");

    cout << "\nPatient registered successfully!\n";
}
void Patient::displayPatient() const
{
    cout << "\n";
    cout << " PATIENT RECORD\n";
    cout << "Patient ID        : " << patientID << endl;
    cout << "Name              : " << name << endl;
    cout << "Age               : " << age << endl;
    cout << "Gender            : " << gender << endl;
    cout << "Blood Group       : " << bloodGroup << endl;
    cout << "Phone             : " << phone << endl;
    cout << "Address           : " << address << endl;
    cout << "Emergency Contact : " << emergencyContact << endl;

    cout << "\n ADMISSION \n";

    cout << "Admission Date    : " << admissionDate << endl;
    cout << "Discharge Date    : " << dischargeDate << endl;
    cout << "Department        : " << department << endl;
    cout << "Doctor            : " << doctor << endl;
    cout << "Reason            : " << reasonForAdmission << endl;
    cout << "Diagnosis         : " << diagnosis << endl;

    cout << "\n MEDICAL HISTORY\n";
    cout << "Previous Illnesses : " << medicalHistory << endl;
    cout << "Previous Surgeries : " << previousSurgeries << endl;
    cout << "Chronic Conditions : "  << chronicConditions << endl;
    cout << "Family History     : " << familyHistory << endl;
    cout << "Allergies          : "  << allergies << endl;
    cout << "\n MEDICATIONS\n";

   if (medications.empty())
    {
        cout << "No medications recorded.\n";
    }
    else
    {
        for (size_t i = 0; i < medications.size(); i++)
        {
            cout << i + 1 << ". "
                 << medications[i].name
                 << " | Dosage: "
                 << medications[i].dosage
                 << " | Frequency: "
                 << medications[i].frequency
                 << " | Duration: "
                 << medications[i].duration
                 << endl;
        }
    }

    cout << "\n TESTS / REPORTS\n";

    if (tests.empty())
    {
        cout << "No tests recorded.\n";
    }
    else
    {
        for (size_t i = 0; i < tests.size(); i++)
        {
            cout << i + 1 << ". "
                 << tests[i].name
                 << " | Date: "
                 << tests[i].date
                 << " | Result: "
                 << tests[i].result
                 << endl;
        }
    }

    cout << "\n DISCHARGE / FOLLOW-UP\n";
    cout << "Condition at Discharge : " << dischargeCondition << endl;
    cout << "Follow-up Date         : "<< followUpDate << endl;
    cout << "Instructions           : " << instructions << endl;

}

void Patient::updatePatient()
{
    cout << "\n";
    cout << "             UPDATE PATIENT\n";
    cout << "Leave the field blank to keep the old value.\n\n";
    string value;
    value = getLine("Name [" + name + "]: ");
    if (!value.empty())
        name = value;
    value = getLine("Phone [" + phone + "]: ");
    if (!value.empty())
        phone = value;
    value = getLine("Address [" + address + "]: ");
    if (!value.empty())
        address = value;
    value = getLine("Doctor [" + doctor + "]: ");
    if (!value.empty())
        doctor = value;
    value = getLine("Diagnosis [" + diagnosis + "]: ");
    if (!value.empty())
        diagnosis = value;
    value = getLine("Allergies [" + allergies + "]: ");
    if (!value.empty())
        allergies = value;
    value = getLine("Discharge Date [" + dischargeDate + "]: ");
    if (!value.empty())
        dischargeDate = value;
    value = getLine("Follow-up Date [" + followUpDate + "]: ");
    if (!value.empty())
        followUpDate = value;
    value = getLine("Instructions [" + instructions + "]: ");
    if (!value.empty())
        instructions = value;
    cout << "\nPatient record updated successfully!\n";
}

void Patient::addMedication()
{
    Medication medicine;

    cout << "\n";
    cout << " ADD MEDICATION\n";
    medicine.name = getLine("Medicine Name: ");
    medicine.dosage = getLine("Dosage: ");
    medicine.frequency = getLine("Frequency: ");
    medicine.duration = getLine("Duration: ");
    medications.push_back(medicine);
    cout << "\nMedication added successfully!\n";
}
void Patient::addTest()
{
    MedicalTest test;

    cout << "\n";
    cout << " ADD TEST / REPORT\n";
    test.name =  getLine("Test Name: ");
    test.date =  getLine("Test Date: ");
    test.result = getLine("Result / Report Summary: ");
    tests.push_back(test);
    cout << "\nTest/report added successfully!\n";
}

string Patient::serialize() const
{
    vector<string> fields =
    {
        to_string(patientID),
        name,
        to_string(age),
        gender,
        bloodGroup,
        phone,
        address,
        emergencyContact,
        admissionDate,
        dischargeDate,
        department,
        doctor,
        reasonForAdmission,
        diagnosis,
        medicalHistory,
        previousSurgeries,
        chronicConditions,
        familyHistory,
        allergies,
        dischargeCondition,
        followUpDate,
        instructions
    };

    string line;

    for (const auto& field : fields)
    {
        if (!line.empty())
            line += "|";

        line += sanitize(field);
    }

    line += "|MEDS|";

    for (const auto& medicine : medications)
    {
        line += sanitize(medicine.name);
        line += "~";

        line += sanitize(medicine.dosage);
        line += "~";

        line += sanitize(medicine.frequency);
        line += "~";

        line += sanitize(medicine.duration);
        line += ";";
    }

    line += "|TESTS|";

    for (const auto& test : tests)
    {
        line += sanitize(test.name);
        line += "~";

        line += sanitize(test.date);
        line += "~";

        line += sanitize(test.result);
        line += ";";
    }


    return line;
}

Patient Patient::deserialize(const string& line)
{
    Patient patient;

    vector<string> parts =
        split(line, '|');

    if (parts.size() < 22)
        return patient;

    patient.patientID = stoi(parts[0]);

    patient.name = parts[1];

    patient.age = stoi(parts[2]);

    patient.gender = parts[3];

    patient.bloodGroup = parts[4];

    patient.phone = parts[5];

    patient.address = parts[6];

    patient.emergencyContact = parts[7];

    patient.admissionDate = parts[8];

    patient.dischargeDate = parts[9];

    patient.department = parts[10];

    patient.doctor = parts[11];

    patient.reasonForAdmission = parts[12];

    patient.diagnosis = parts[13];

    patient.medicalHistory = parts[14];

    patient.previousSurgeries = parts[15];

    patient.chronicConditions = parts[16];

    patient.familyHistory = parts[17];

    patient.allergies = parts[18];

    patient.dischargeCondition = parts[19];

    patient.followUpDate = parts[20];

    patient.instructions = parts[21];

    for (size_t i = 22; i < parts.size(); i++)
    {
        if (parts[i] == "MEDS" && i + 1 < parts.size())
        {
            vector<string> medicationRecords =
                split(parts[i + 1], ';');

            for (const auto& record : medicationRecords)
            {
                if (record.empty())
                    continue;

                vector<string> m =
                    split(record, '~');

                if (m.size() == 4)
                {
                    patient.medications.push_back(
                    {
                        m[0],
                        m[1],
                        m[2],
                        m[3]
                    });
                }
            }

            i++;
        }

        else if (parts[i] == "TESTS" && i + 1 < parts.size())
        {
            vector<string> testRecords =
                split(parts[i + 1], ';');

            for (const auto& record : testRecords)
            {
                if (record.empty())
                    continue;

                vector<string> t =
                    split(record, '~');

                if (t.size() == 3)
                {
                    patient.tests.push_back(
                    {
                        t[0],
                        t[1],
                        t[2]
                    });
                }
            }

            i++;
        }
    }

    return patient;
}

vector<Patient> loadPatients()
{
    vector<Patient> patients;

    ifstream file(DATA_FILE);

    string line;

    while (getline(file, line))
    {
        if (!line.empty())
        {
            patients.push_back( Patient::deserialize(line)
            );
        }
    }

    return patients;
}

void savePatients(const vector<Patient>& patients)
{
    ofstream file(DATA_FILE);

    for (const auto& patient : patients)
    {
        file << patient.serialize() << '\n';
    }
}
int findPatientIndex( const vector<Patient>& patients, int id)
{
    for (size_t i = 0; i < patients.size(); i++)
    {
        if (patients[i].patientID == id)
        {
            return static_cast<int>(i);
        }
    }

    return -1;
}
void registerPatient(vector<Patient>& patients)
{
    Patient patient;

    patient.addPatient();

    if (findPatientIndex(patients, patient.patientID) != -1)
    {
        cout << "\nPatient ID already exists!\n";
        return;
    }

    patients.push_back(patient);

    savePatients(patients);
}

void searchPatient(
    const vector<Patient>& patients)
{
    cout << "\n";
    cout << " SEARCH PATIENT\n";

    int id = getInt("Enter Patient ID: ");
    int index = findPatientIndex(patients, id);

    if (index == -1)
    {
        cout << "\nPatient not found!\n";
        return;
    }

    patients[index].displayPatient();
}

void updatePatient(
    vector<Patient>& patients)
{
    int id = getInt("Enter Patient ID to update: ");
    int index = findPatientIndex(patients, id);

    if (index == -1)
    {
        cout << "\nPatient not found!\n";
        return;
    }

    patients[index].updatePatient();

    savePatients(patients);
}

void addMedicationToPatient(
    vector<Patient>& patients)
{
    int id = getInt("Enter Patient ID: ");
    int index = findPatientIndex(patients, id);

    if (index == -1)
    {
        cout << "\nPatient not found!\n";
        return;
    }

    patients[index].addMedication();

    savePatients(patients);
}

void addTestToPatient(
    vector<Patient>& patients)
{
    int id =  getInt("Enter Patient ID: ");
    int index = findPatientIndex(patients, id);

    if (index == -1)
    {
        cout << "\nPatient not found!\n";
        return;
    }

    patients[index].addTest();

    savePatients(patients);
}

void dischargePatient(
    vector<Patient>& patients)
{
    int id = getInt("Enter Patient ID: ");


    int index =
        findPatientIndex(patients, id);

    if (index == -1)
    {
        cout << "\nPatient not found!\n";
        return;
    }

    cout << "\n";
    cout << " DISCHARGE PATIENT\n";
    patients[index].dischargeDate =  getLine("Discharge Date: ");
    patients[index].dischargeCondition = getLine("Condition at Discharge: ");
    patients[index].followUpDate = getLine("Follow-up Date: ");
    patients[index].instructions = getLine("Post-treatment Instructions: ");
    savePatients(patients);
    cout << "\nPatient discharge information saved!\n";
}
void viewAllPatients(
    const vector<Patient>& patients)
{
    cout << "\n";
    cout << "ALL PATIENTS\n";
    if (patients.empty())
    {
        cout << "No patient records found.\n";
        return;
    }
    cout << left
         << setw(12) << "ID"
         << setw(25) << "Name"
         << setw(8) << "Age"
         << setw(15) << "Blood Group"
         << setw(20) << "Department"
         << endl;

    cout << string(80, '-') << endl;


    for (const auto& patient : patients)
    {
        cout << left
             << setw(12) << patient.patientID
             << setw(25) << patient.name
             << setw(8) << patient.age
             << setw(15) << patient.bloodGroup
             << setw(20) << patient.department
             << endl;
    }
}

void deletePatient(
    vector<Patient>& patients)
{
    int id = getInt("Enter Patient ID to delete: ");
    int index = findPatientIndex(patients, id);

    if (index == -1)
    {
        cout << "\nPatient not found!\n";
        return;
    }

    string confirmation = getLine( "Type YES to permanently delete: " );
    if (confirmation == "YES")
    {
        patients.erase( patients.begin() + index);
        savePatients(patients);
        cout << "\nPatient record deleted successfully!\n";
    }
    else
    {
        cout << "\nDeletion cancelled.\n";
    }
}

void showMenu()
{
    cout << " PATIENT MANAGEMENT SYSTEM\n";
    cout << "1.  Register New Patient\n";
    cout << "2.  View Patient Details\n";
    cout << "3.  Search Patient\n";
    cout << "4.  Update Patient Record\n";
    cout << "5.  Add Medication\n";
    cout << "6.  Add Test / Report\n";
    cout << "7.  Discharge Patient\n";
    cout << "8.  View All Patients\n";
    cout << "9.  Delete Patient Record\n";
    cout << "10. Exit\n";

}

{
    vector<Patient> patients =  loadPatients();

    cout << "       WELCOME TO PATIENT MANAGEMENT SYSTEM\n";
    cout << "Stored records loaded: " << patients.size() << endl;

    while (true)
    {
        showMenu();


        int choice =  getInt("Enter your choice: ");

        switch (choice)
        {
            case 1:
                registerPatient(patients);
                break;


            case 2:
                searchPatient(patients);
                break;


            case 3:
                searchPatient(patients);
                break;


            case 4:
                updatePatient(patients);
                break;


            case 5:
                addMedicationToPatient(patients);
                break;


            case 6:
                addTestToPatient(patients);
                break;


            case 7:
                dischargePatient(patients);
                break;


            case 8:
                viewAllPatients(patients);
                break;


            case 9:
                deletePatient(patients);
                break;


            case 10:
                cout << "\nThank you for using the system!\n";
                return 0;


            default:
                cout << "\nInvalid choice! Please try again.\n";
        }
    }


    return 0;
}
