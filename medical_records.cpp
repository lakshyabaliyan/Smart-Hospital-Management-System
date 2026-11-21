
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <limits>
#include <cctype>
#include <cstdlib>

using namespace std;

struct MedicalRecord
{
    string recordId;
    string patientId;
    string patientName;
    int age;
    string gender;
    string department;
    string bloodGroup;
    string diagnosis;
    string symptoms;
    string doctorName;
    string treatment;
    string medicines;
    string dosage;
    string duration;
    string allergies;
    string recordDate;
    string recordTime;
};

struct Investigation
{
    string investigationId;
    string recordId;
    string patientId;
    string testName;
    string bodyPart;
    string testDate;
    string findings;
    string remarks;
    string status;
};

class MedicalRecords
{
private:
    vector<MedicalRecord> records;
    vector<Investigation> investigations;
    const string recordFile = "medical_records.txt";
    const string investigationFile = "investigations.txt";

    void clearScreen()
    {
        system("cls");
    }

    string toLowerCase(string text)
    {
        for (int i = 0; i < text.length(); i++)
            text[i] = tolower(static_cast<unsigned char>(text[i]));
        return text;
    }

    string getDateTime(string &recordTime)
    {
        time_t now = time(0);
        tm *local = localtime(&now);
        char dateBuffer[20];
        char timeBuffer[20];
        strftime(dateBuffer, sizeof(dateBuffer), "%d-%m-%Y", local);
        strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", local);
        recordTime = timeBuffer;
        return dateBuffer;
    }

    string getCurrentDate()
    {
        time_t now = time(0);
        tm *local = localtime(&now);
        char dateBuffer[20];
        strftime(dateBuffer, sizeof(dateBuffer), "%d-%m-%Y", local);
        return dateBuffer;
    }

    bool isValidID(string id)
    {
        if (id.empty())
            return false;
        for (int i = 0; i < id.length(); i++)
        {
            unsigned char ch = id[i];
            if (!isalnum(ch) && ch != '-')
                return false;
        }
        return true;
    }

    bool isValidText(string value)
    {
        return !value.empty() && value.find('|') == string::npos;
    }

    string getText(string message)
    {
        string value;
        while (true)
        {
            cout << message;
            getline(cin, value);
            if (isValidText(value))
                return value;
            cout << "Invalid input. Enter a non-empty value without the | character.\n";
        }
    }

    string getID(string message)
    {
        string id;
        while (true)
        {
            cout << message;
            getline(cin, id);
            if (isValidID(id))
                return id;
            cout << "Invalid ID. Use letters, numbers or hyphens only.\n";
        }
    }

    int getAge()
    {
        int age;
        while (true)
        {
            cout << "Enter Patient Age: ";
            if (cin >> age && age >= 1 && age <= 120)
            {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return age;
            }
            cout << "Invalid age. Enter a value between 1 and 120.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    int searchRecord(string id)
    {
        for (int i = 0; i < records.size(); i++)
        {
            if (toLowerCase(records[i].recordId) == toLowerCase(id))
                return i;
        }
        return -1;
    }

    int searchInvestigation(string id)
    {
        for (int i = 0; i < investigations.size(); i++)
        {
            if (toLowerCase(investigations[i].investigationId) == toLowerCase(id))
                return i;
        }
        return -1;
    }

    void saveRecords()
    {
        ofstream file(recordFile.c_str());
        if (!file)
        {
            cout << "Error: Unable to save medical records.\n";
            return;
        }

        for (int i = 0; i < records.size(); i++)
        {
            MedicalRecord r = records[i];
            file << r.recordId << "|" << r.patientId << "|" << r.patientName << "|"<< r.age << "|" << r.gender << "|" << r.department << "|"
                 << r.bloodGroup << "|" << r.diagnosis << "|" << r.symptoms << "|"<< r.doctorName << "|" << r.treatment << "|" << r.medicines << "|"
                 << r.dosage << "|" << r.duration << "|" << r.allergies << "|" << r.recordDate << "|" << r.recordTime << "\n";
        }
        file.close();
    }

    void saveInvestigations()
    {
        ofstream file(investigationFile.c_str());
        if (!file)
        {
            cout << "Error: Unable to save investigations.\n";
            return;
        }

        for (int i = 0; i < investigations.size(); i++)
        {
            Investigation t = investigations[i];
            file << t.investigationId << "|" << t.recordId << "|" << t.patientId << "|" << t.testName << "|" << t.bodyPart << "|"
                 << t.testDate << "|" << t.findings << "|" << t.remarks << "|"<< t.status << "\n";
        }
        file.close();
    }

    void loadRecords()
    {
        ifstream file(recordFile.c_str());
        if (!file)
            return;

        string line;
        while (getline(file, line))
        {
            stringstream ss(line);
            string f[17];
            bool valid = true;

            for (int i = 0; i < 17; i++)
            {
                if (!getline(ss, f[i], '|'))
                {
                    valid = false;
                    break;
                }
            }

            if (!valid)
                continue;

            string extra;
            if (getline(ss, extra, '|'))
                continue;

            try
            {
                MedicalRecord r;
                r.recordId = f[0];
                r.patientId = f[1];
                r.patientName = f[2];
                r.age = stoi(f[3]);
                r.gender = f[4];
                r.department = f[5];
                r.bloodGroup = f[6];
                r.diagnosis = f[7];
                r.symptoms = f[8];
                r.doctorName = f[9];
                r.treatment = f[10];
                r.medicines = f[11];
                r.dosage = f[12];
                r.duration = f[13];
                r.allergies = f[14];
                r.recordDate = f[15];
                r.recordTime = f[16];

                if (isValidID(r.recordId) && isValidID(r.patientId) &&r.age >= 1 && r.age <= 120 && searchRecord(r.recordId) == -1)
                    records.push_back(r);
            }
            catch (...)
            {
                continue;
            }
        }
        file.close();
    }

    void loadInvestigations()
    {
        ifstream file(investigationFile.c_str());
        if (!file)
            return;

        string line;
        while (getline(file, line))
        {
            stringstream ss(line);
            string f[9];
            bool valid = true;

            for (int i = 0; i < 9; i++)
            {
                if (!getline(ss, f[i], '|'))
                {
                    valid = false;
                    break;
                }
            }

            if (!valid)
                continue;

            string extra;
            if (getline(ss, extra, '|'))
                continue;

            Investigation t;
            t.investigationId = f[0];
            t.recordId = f[1];
            t.patientId = f[2];
            t.testName = f[3];
            t.bodyPart = f[4];
            t.testDate = f[5];
            t.findings = f[6];
            t.remarks = f[7];
            t.status = f[8];

            int index = searchRecord(t.recordId);

            if (isValidID(t.investigationId) &&
                index != -1 &&
                toLowerCase(records[index].patientId) == toLowerCase(t.patientId) &&
                searchInvestigation(t.investigationId) == -1)
                investigations.push_back(t);
        }
        file.close();
    }

    void displayRecord(MedicalRecord r)
    {
        cout << "\n============================================\n";
        cout << "               MEDICAL RECORD\n";
        cout << "============================================\n";
        cout << left << setw(20) << "Record ID:" << r.recordId << "\n";
        cout << left << setw(20) << "Patient ID:" << r.patientId << "\n";
        cout << left << setw(20) << "Patient Name:" << r.patientName << "\n";
        cout << left << setw(20) << "Age:" << r.age << "\n";
        cout << left << setw(20) << "Gender:" << r.gender << "\n";
        cout << left << setw(20) << "Department:" << r.department << "\n";
        cout << left << setw(20) << "Blood Group:" << r.bloodGroup << "\n";
        cout << "--------------------------------------------\n";
        cout << left << setw(20) << "Diagnosis:" << r.diagnosis << "\n";
        cout << left << setw(20) << "Symptoms:" << r.symptoms << "\n";
        cout << left << setw(20) << "Doctor:" << r.doctorName << "\n";
        cout << left << setw(20) << "Treatment:" << r.treatment << "\n";
        cout << left << setw(20) << "Medicines:" << r.medicines << "\n";
        cout << left << setw(20) << "Dosage:" << r.dosage << "\n";
        cout << left << setw(20) << "Duration:" << r.duration << "\n";
        cout << left << setw(20) << "Allergies:" << r.allergies << "\n";
        cout << "--------------------------------------------\n";
        cout << left << setw(20) << "Record Date:" << r.recordDate << "\n";
        cout << left << setw(20) << "Record Time:" << r.recordTime << "\n";
        cout << "============================================\n";
    }

    void displayInvestigation(Investigation t)
    {
        cout << "\n============================================\n";
        cout << "           DIAGNOSTIC INVESTIGATION\n";
        cout << "============================================\n";
        cout << left << setw(22) << "Investigation ID:" << t.investigationId << "\n";
        cout << left << setw(22) << "Record ID:" << t.recordId << "\n";
        cout << left << setw(22) << "Patient ID:" << t.patientId << "\n";
        cout << left << setw(22) << "Test Name:" << t.testName << "\n";
        cout << left << setw(22) << "Body Part:" << t.bodyPart << "\n";
        cout << left << setw(22) << "Test Date:" << t.testDate << "\n";
        cout << left << setw(22) << "Findings:" << t.findings << "\n";
        cout << left << setw(22) << "Doctor's Remarks:" << t.remarks << "\n";
        cout << left << setw(22) << "Status:" << t.status << "\n";
        cout << "============================================\n";
    }

    void updateMedicalDetails(MedicalRecord &r)
    {
        cout << "\nEnter Updated Medical Details\n";
        r.department = getText("Department: ");
        r.diagnosis = getText("Diagnosis: ");
        r.symptoms = getText("Symptoms: ");
        r.doctorName = getText("Doctor Name: ");
        r.treatment = getText("Treatment: ");
        r.medicines = getText("Prescribed Medicines: ");
        r.dosage = getText("Dosage: ");
        r.duration = getText("Medicine Duration: ");
        r.allergies = getText("Allergies (enter None if no allergies): ");
        r.recordDate = getDateTime(r.recordTime);
    }

public:
    MedicalRecords()
    {
        loadRecords();
        loadInvestigations();
    }

    void addRecord()
    {
        clearScreen();
        MedicalRecord r;

        cout << "============================================\n";
        cout << "           ADD NEW MEDICAL RECORD\n";
        cout << "============================================\n";

        r.recordId = getID("Enter Unique Record ID: ");
        if (searchRecord(r.recordId) != -1)
        {
            cout << "\nRecord ID already exists!\n";
            return;
        }

        r.patientId = getID("Enter Patient ID: ");
        r.patientName = getText("Enter Patient Name: ");
        r.age = getAge();
        r.gender = getText("Enter Gender: ");
        r.department = getText("Enter Department: ");
        r.bloodGroup = getText("Enter Blood Group: ");

        cout << "\n---------- MEDICAL INFORMATION ----------\n";
        r.diagnosis = getText("Enter Diagnosis: ");
        r.symptoms = getText("Enter Symptoms: ");
        r.doctorName = getText("Enter Doctor Name: ");
        r.treatment = getText("Enter Treatment: ");
        r.medicines = getText("Enter Prescribed Medicines: ");
        r.dosage = getText("Enter Dosage: ");
        r.duration = getText("Enter Medicine Duration: ");
        r.allergies = getText("Enter Allergies (enter None if no allergies): ");
        r.recordDate = getDateTime(r.recordTime);

        records.push_back(r);
        saveRecords();
        cout << "\nMedical record added successfully!\n";
        cout << "Total Medical Records: " << records.size() << "\n";
    }

    void displayAllRecords()
    {
        clearScreen();
        cout << "============================================\n";
        cout << "              ALL MEDICAL RECORDS\n";
        cout << "============================================\n";

        if (records.empty())
        {
            cout << "\nNo medical records available.\n";
            return;
        }

        for (int i = 0; i < records.size(); i++)
            displayRecord(records[i]);

        cout << "\nTotal Records: " << records.size() << "\n";
    }

    void searchByRecordID()
    {
        clearScreen();
        cout << "============================================\n";
        cout << "            SEARCH BY RECORD ID\n";
        cout << "============================================\n";

        string id = getID("Enter Record ID: ");
        int index = searchRecord(id);

        if (index == -1)
        {
            cout << "\nMedical record not found.\n";
            return;
        }
        displayRecord(records[index]);
    }

    void searchByPatientID()
    {
        clearScreen();
        cout << "============================================\n";
        cout << "           PATIENT MEDICAL HISTORY\n";
        cout << "============================================\n";

        string id = getID("Enter Patient ID: ");
        bool found = false;
        int count = 0;

        for (int i = 0; i < records.size(); i++)
        {
            if (toLowerCase(records[i].patientId) == toLowerCase(id))
            {
                displayRecord(records[i]);
                found = true;
                count++;
            }
        }

        if (!found)
            cout << "\nNo medical history found for this Patient ID.\n";
        else
            cout << "\nTotal Visits / Records: " << count << "\n";
    }

    void updateRecord()
    {
        clearScreen();
        cout << "============================================\n";
        cout << "            UPDATE MEDICAL RECORD\n";
        cout << "============================================\n";

        string id = getID("Enter Record ID to Update: ");
        int index = searchRecord(id);

        if (index == -1)
        {
            cout << "\nMedical record not found.\n";
            return;
        }

        displayRecord(records[index]);
        cout << "\nUpdate the medical details? (1 = Yes, 0 = No): ";

        int choice;
        if (!(cin >> choice) || (choice != 0 && choice != 1))
        {
            cout << "\nInvalid choice.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1)
        {
            updateMedicalDetails(records[index]);
            saveRecords();
            cout << "\nMedical record updated successfully!\n";
        }
        else
            cout << "\nUpdate cancelled.\n";
    }

    void searchByDepartment()
    {
        clearScreen();
        cout << "============================================\n";
        cout << "            SEARCH BY DEPARTMENT\n";
        cout << "============================================\n";

        string department = getText("Enter Department Name: ");
        bool found = false;
        int count = 0;

        for (int i = 0; i < records.size(); i++)
        {
            if (toLowerCase(records[i].department) == toLowerCase(department))
            {
                displayRecord(records[i]);
                found = true;
                count++;
            }
        }

        if (!found)
            cout << "\nNo records found in this department.\n";
        else
            cout << "\nTotal Records in " << department << ": " << count << "\n";
    }

    void departmentStatistics()
    {
        clearScreen();
        cout << "============================================\n";
        cout << "          DEPARTMENT-WISE STATISTICS\n";
        cout << "============================================\n";

        if (records.empty())
        {
            cout << "\nNo medical records available.\n";
            return;
        }

        int totalPatients = 0;

        for (int i = 0; i < records.size(); i++)
        {
            bool alreadyCounted = false;
            for (int j = 0; j < i; j++)
            {
                if (toLowerCase(records[i].patientId) ==
                    toLowerCase(records[j].patientId))
                {
                    alreadyCounted = true;
                    break;
                }
            }
            if (!alreadyCounted)
                totalPatients++;
        }

        cout << "\nTotal Medical Records: " << records.size() << "\n";
        cout << "Total Unique Patients: " << totalPatients << "\n";
        cout << "\n--------------------------------------------\n";
        cout << left << setw(25) << "Department" << "Number of Records\n";
        cout << "--------------------------------------------\n";

        for (int i = 0; i < records.size(); i++)
        {
            bool alreadyCounted = false;
            for (int j = 0; j < i; j++)
            {
                if (toLowerCase(records[i].department) ==
                    toLowerCase(records[j].department))
                {
                    alreadyCounted = true;
                    break;
                }
            }

            if (!alreadyCounted)
            {
                int count = 0;
                for (int j = 0; j < records.size(); j++)
                {
                    if (toLowerCase(records[i].department) ==
                        toLowerCase(records[j].department))
                        count++;
                }
                cout << left << setw(25) << records[i].department << count << "\n";
            }
        }
        cout << "--------------------------------------------\n";
    }

    void addInvestigation()
    {
        clearScreen();
        Investigation t;

        cout << "============================================\n";
        cout << "         ADD DIAGNOSTIC INVESTIGATION\n";
        cout << "============================================\n";

        t.investigationId = getID("Enter Unique Investigation ID: ");
        if (searchInvestigation(t.investigationId) != -1)
        {
            cout << "\nInvestigation ID already exists!\n";
            return;
        }

        t.recordId = getID("Enter Related Medical Record ID: ");
        int index = searchRecord(t.recordId);

        if (index == -1)
        {
            cout << "\nMedical record not found. Add the medical record first.\n";
            return;
        }

        t.patientId = records[index].patientId;
        cout << "\nPatient ID: " << t.patientId << "\n";
        cout << "Patient Name: " << records[index].patientName << "\n";

        t.testName = getText("Enter Test Name (X-ray, MRI, CT Scan, Ultrasound, ECG, Blood Test etc.): ");
        t.bodyPart = getText("Enter Body Part / Sample: ");
        t.testDate = getCurrentDate();
        cout << "Investigation Date: " << t.testDate << "\n";
        t.status = "Pending";
        t.findings = "Pending";
        t.remarks = "Pending";

        investigations.push_back(t);
        saveInvestigations();

        cout << "\nInvestigation added successfully!\n";
        cout << "Initial Status: Pending\n";
    }

    void viewInvestigations()
    {
        clearScreen();
        cout << "============================================\n";
        cout << "         ALL DIAGNOSTIC INVESTIGATIONS\n";
        cout << "============================================\n";

        if (investigations.empty())
        {
            cout << "\nNo investigations available.\n";
            return;
        }

        for (int i = 0; i < investigations.size(); i++)
            displayInvestigation(investigations[i]);

        cout << "\nTotal Investigations: " << investigations.size() << "\n";
    }

    void patientInvestigationHistory()
    {
        clearScreen();
        cout << "============================================\n";
        cout << "       PATIENT INVESTIGATION HISTORY\n";
        cout << "============================================\n";

        string id = getID("Enter Patient ID: ");
        bool found = false;
        int count = 0;

        for (int i = 0; i < investigations.size(); i++)
        {
            if (toLowerCase(investigations[i].patientId) == toLowerCase(id))
            {
                displayInvestigation(investigations[i]);
                found = true;
                count++;
            }
        }

        if (!found)
            cout << "\nNo investigations found for this patient.\n";
        else
            cout << "\nTotal Investigations: " << count << "\n";
    }

    void updateInvestigationStatus()
    {
        clearScreen();
        cout << "============================================\n";
        cout << "       UPDATE INVESTIGATION DETAILS\n";
        cout << "============================================\n";

        string id = getID("Enter Investigation ID: ");
        int index = searchInvestigation(id);

        if (index == -1)
        {
            cout << "\nInvestigation not found.\n";
            return;
        }

        displayInvestigation(investigations[index]);

        cout << "\nSelect New Status:\n";
        cout << "1. Pending\n";
        cout << "2. Completed\n";
        cout << "Enter choice: ";

        int choice;
        if (!(cin >> choice) || (choice != 1 && choice != 2))
        {
            cout << "\nInvalid choice.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1)
        {
            investigations[index].status = "Pending";
            investigations[index].findings = "Pending";
            investigations[index].remarks = "Pending";
        }
        else
        {
            investigations[index].status = "Completed";
            investigations[index].findings = getText("Enter Test Findings / Result: ");
            investigations[index].remarks = getText("Enter Doctor's Remarks: ");
        }

        saveInvestigations();
        cout << "\nInvestigation details updated successfully!\n";
    }
};

int main()
{
    MedicalRecords medical;
    int choice;

    do
    {
        system("cls");
        cout << "\n============================================\n";
        cout << "       AROGYA HOSPITAL MANAGEMENT SYSTEM\n";
        cout << "           MEDICAL RECORDS MODULE\n";
        cout << "============================================\n";
        cout << "\n1. Add New Medical Record";
        cout << "\n2. Display All Medical Records";
        cout << "\n3. Search by Record ID";
        cout << "\n4. View Patient Medical History";
        cout << "\n5. Update Medical Record";
        cout << "\n6. Search Records by Department";
        cout << "\n7. Department-wise Statistics";
        cout << "\n8. Add Diagnostic Investigation";
        cout << "\n9. View All Investigations";
        cout << "\n10. View Patient Investigation History";
        cout << "\n11. Update Investigation Status";
        cout << "\n12. Exit";
        cout << "\n\nEnter your choice: ";

        if (!(cin >> choice))
        {
            cout << "\nInvalid input. Enter a number.";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nPress Enter to continue...";
            cin.get();
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice)
        {
            case 1:
                medical.addRecord();
                break;
            case 2:
                medical.displayAllRecords();
                break;
            case 3:
                medical.searchByRecordID();
                break;
            case 4:
                medical.searchByPatientID();
                break;
            case 5:
                medical.updateRecord();
                break;
            case 6:
                medical.searchByDepartment();
                break;
            case 7:
                medical.departmentStatistics();
                break;
            case 8:
                medical.addInvestigation();
                break;
            case 9:
                medical.viewInvestigations();
                break;
            case 10:
                medical.patientInvestigationHistory();
                break;
            case 11:
                medical.updateInvestigationStatus();
                break;
            case 12:
                cout << "\nExiting....\n";
                break;
            default:
                cout << "\nInvalid choice. Try again.\n";
        }

        if (choice != 12)
        {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    } while (choice != 12);

    return 0;
}
