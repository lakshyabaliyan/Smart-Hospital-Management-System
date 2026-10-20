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
    cout << "============================================================\n";
    cout << "                    PATIENT RECORD\n";
    cout << "============================================================\n";

    cout << "Patient ID        : " << patientID << endl;
    cout << "Name              : " << name << endl;
    cout << "Age               : " << age << endl;
    cout << "Gender            : " << gender << endl;
    cout << "Blood Group       : " << bloodGroup << endl;
    cout << "Phone             : " << phone << endl;
    cout << "Address           : " << address << endl;
    cout << "Emergency Contact : " << emergencyContact << endl;

    cout << "\n---------------- ADMISSION ----------------\n";

    cout << "Admission Date    : " << admissionDate << endl;
    cout << "Discharge Date    : " << dischargeDate << endl;
    cout << "Department        : " << department << endl;
    cout << "Doctor            : " << doctor << endl;
    cout << "Reason            : " << reasonForAdmission << endl;
    cout << "Diagnosis         : " << diagnosis << endl;

    cout << "\n---------------- MEDICAL HISTORY ----------------\n";

    cout << "Previous Illnesses : "
         << medicalHistory << endl;

    cout << "Previous Surgeries : "
         << previousSurgeries << endl;

    cout << "Chronic Conditions : "
         << chronicConditions << endl;

    cout << "Family History     : "
         << familyHistory << endl;

    cout << "Allergies          : "
         << allergies << endl;

    cout << "\n---------------- MEDICATIONS ----------------\n";

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

    cout << "\n---------------- TESTS / REPORTS ----------------\n";

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

    cout << "\n---------------- DISCHARGE / FOLLOW-UP ----------------\n";

    cout << "Condition at Discharge : "
         << dischargeCondition << endl;

    cout << "Follow-up Date         : "
         << followUpDate << endl;

    cout << "Instructions           : "
         << instructions << endl;

    cout << "============================================================\n";
}


// ============================================================
// UPDATE PATIENT
// ============================================================

void Patient::updatePatient()
{
    cout << "\n";
    cout << "=============================================\n";
    cout << "             UPDATE PATIENT\n";
    cout << "=============================================\n";

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


// ============================================================
// ADD MEDICATION
// ============================================================

void Patient::addMedication()
{
    Medication medicine;

    cout << "\n";
    cout << "=============================================\n";
    cout << "             ADD MEDICATION\n";
    cout << "=============================================\n";

    medicine.name =
        getLine("Medicine Name: ");

    medicine.dosage =
        getLine("Dosage: ");

    medicine.frequency =
        getLine("Frequency: ");

    medicine.duration =
        getLine("Duration: ");

    medications.push_back(medicine);

    cout << "\nMedication added successfully!\n";
}


// ============================================================
// ADD TEST
// ============================================================

void Patient::addTest()
{
    MedicalTest test;

    cout << "\n";
    cout << "=============================================\n";
    cout << "              ADD TEST / REPORT\n";
    cout << "=============================================\n";

    test.name =
        getLine("Test Name: ");

    test.date =
        getLine("Test Date: ");

    test.result =
        getLine("Result / Report Summary: ");

    tests.push_back(test);

    cout << "\nTest/report added successfully!\n";
}


// ============================================================
// CONVERT PATIENT OBJECT INTO STRING
// ============================================================

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


    // Store medications

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


    // Store tests

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


// ============================================================
// CONVERT STRING INTO PATIENT OBJECT
// ============================================================

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


    // Read medications

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


        // Read tests

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


// ============================================================
// LOAD PATIENTS FROM FILE
// ============================================================

vector<Patient> loadPatients()
{
    vector<Patient> patients;

    ifstream file(DATA_FILE);

    string line;

    while (getline(file, line))
    {
        if (!line.empty())
        {
            patients.push_back(
                Patient::deserialize(line)
            );
        }
    }

    return patients;
}


// ============================================================
// SAVE PATIENTS TO FILE
// ============================================================

void savePatients(const vector<Patient>& patients)
{
    ofstream file(DATA_FILE);

    for (const auto& patient : patients)
    {
        file << patient.serialize() << '\n';
    }
}


// ============================================================
// FIND PATIENT BY ID
// ============================================================

int findPatientIndex(
    const vector<Patient>& patients,
    int id)
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


// ============================================================
// REGISTER PATIENT
// ============================================================

void registerPatient(vector<Patient>& patients)
{
    Patient patient;

    patient.addPatient();


    if (findPatientIndex(
            patients,
            patient.patientID) != -1)
    {
        cout << "\nPatient ID already exists!\n";
        return;
    }


    patients.push_back(patient);

    savePatients(patients);
}


// ============================================================
// SEARCH PATIENT
// ============================================================

void searchPatient(
    const vector<Patient>& patients)
{
    cout << "\n";
    cout << "=============================================\n";
    cout << "              SEARCH PATIENT\n";
    cout << "=============================================\n";

    int id =
        getInt("Enter Patient ID: ");


    int index =
        findPatientIndex(patients, id);


    if (index == -1)
    {
        cout << "\nPatient not found!\n";
        return;
    }


    patients[index].displayPatient();
}


// ============================================================
// UPDATE PATIENT
// ============================================================

void updatePatient(
    vector<Patient>& patients)
{
    int id =
        getInt("Enter Patient ID to update: ");


    int index =
        findPatientIndex(patients, id);


    if (index == -1)
    {
        cout << "\nPatient not found!\n";
        return;
    }


    patients[index].updatePatient();

    savePatients(patients);
}


// ============================================================
// ADD MEDICATION TO PATIENT
// ============================================================

void addMedicationToPatient(
    vector<Patient>& patients)
{
    int id =
        getInt("Enter Patient ID: ");


    int index =
        findPatientIndex(patients, id);


    if (index == -1)
    {
        cout << "\nPatient not found!\n";
        return;
    }


    patients[index].addMedication();

    savePatients(patients);
}


// ============================================================
// ADD TEST TO PATIENT
// ============================================================

void addTestToPatient(
    vector<Patient>& patients)
{
    int id =
        getInt("Enter Patient ID: ");


    int index =
        findPatientIndex(patients, id);


    if (index == -1)
    {
        cout << "\nPatient not found!\n";
        return;
    }


    patients[index].addTest();

    savePatients(patients);
}


// ============================================================
// DISCHARGE PATIENT
// ============================================================

void dischargePatient(
    vector<Patient>& patients)
{
    int id =
        getInt("Enter Patient ID: ");


    int index =
        findPatientIndex(patients, id);


    if (index == -1)
    {
        cout << "\nPatient not found!\n";
        return;
    }


    cout << "\n";
    cout << "=============================================\n";
    cout << "            DISCHARGE PATIENT\n";
    cout << "=============================================\n";


    patients[index].dischargeDate =
        getLine("Discharge Date: ");


    patients[index].dischargeCondition =
        getLine("Condition at Discharge: ");


    patients[index].followUpDate =
        getLine("Follow-up Date: ");


    patients[index].instructions =
