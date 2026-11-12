#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <cctype>
#include <limits>

using namespace std;

#define MAX_BILLS 100

struct Bill
{
    string billId;
    string patientId;
    string patientName;
    string billDate;
    string billTime;
    double consultationFee;
    double testCharges;
    double medicineCharges;
    double roomCharges;
    double otherCharges;
    double discount;
    double totalAmount;
    double paidAmount;
    double balance;
    string paymentMode;
};

class BillingManagement
{
private:
    Bill bills[MAX_BILLS];
    int count;

    
void getDateTime(string &date, string &billTime)
{
    time_t now = std::time(0);
    tm *local = std::localtime(&now);

    char d[20], t[20];

    strftime(d, sizeof(d), "%d-%m-%Y", local);
    strftime(t, sizeof(t), "%H:%M:%S", local);

    date = d;
    billTime = t;
}

void clearScreen()
{
    system("cls");
}

    void calculateBill(Bill &b)
    {
        double subtotal = b.consultationFee + b.testCharges +b.medicineCharges + b.roomCharges + b.otherCharges;
        b.totalAmount = subtotal - b.discount;
        b.balance = b.totalAmount - b.paidAmount;
    }

    int searchBill(string id)
    {
        for (int i = 0; i < count; i++)
        {
            if (bills[i].billId == id)
               return i;
        }

        return -1;
    }

    double getAmount(string message)
    {
        double amount;
        while (true)
        {
            cout << message;
            if (cin >> amount && amount >= 0)
                return amount;

            cout << "Enter a valid non-negative amount.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    string getPaymentMode()
    {
        int choice;
        while (true)
        {
            cout << "\nSelect Payment Mode\n";
            cout << "1. Cash\n";
            cout << "2. UPI\n";
            cout << "3. Card\n";
            cout << "Enter choice: ";

            if (cin >> choice)
            {
                if (choice == 1) return "Cash";
                if (choice == 2) return "UPI";
                if (choice == 3) return "Card";
            }

            cout << "Invalid choice. Try again.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    
    bool isValidID(string id)
    {
        if (id.empty())
           return false;

        for (int i = 0; i < id.length(); i++)
        {
             if (!isalnum(static_cast<unsigned char>(id[i])) && id[i] != '-')
                     return false;
        }

        return true;
    } 

    void saveBills()
    {
        ofstream fout("billing.txt");
        if (!fout)
        {
            cout << "Unable to save billing records.\n";
            return;
        }

        for (int i = 0; i < count; i++)
        {
            fout << bills[i].billId << "|"<< bills[i].patientId << "|" << bills[i].patientName << "|"<< bills[i].billDate << "|"
                 << bills[i].billTime << "|"<< bills[i].consultationFee << "|"<< bills[i].testCharges << "|"
                 << bills[i].medicineCharges << "|"<< bills[i].roomCharges << "|"<< bills[i].otherCharges << "|"
                 << bills[i].discount << "|"<< bills[i].paidAmount << "|"<< bills[i].paymentMode << "\n";
        }

        fout.close();
    }

    void loadBills()
    {
        ifstream fin("billing.txt");
        string line;

        while (getline(fin, line) && count < MAX_BILLS)
        {
            stringstream ss(line);
            string fields[13];
            bool valid = true;

            for (int i = 0; i < 13; i++)
            {
                if (!getline(ss, fields[i], '|'))
                {
                    valid = false;
                    break;
                }
            }

            if (!valid)
                continue;

            try
            {
                Bill b;

                b.billId = fields[0];
                b.patientId = fields[1];
                b.patientName = fields[2];
                b.billDate = fields[3];
                b.billTime = fields[4];
                b.consultationFee = stod(fields[5]);
                b.testCharges = stod(fields[6]);
                b.medicineCharges = stod(fields[7]);
                b.roomCharges = stod(fields[8]);
                b.otherCharges = stod(fields[9]);
                b.discount = stod(fields[10]);
                b.paidAmount = stod(fields[11]);
                b.paymentMode = fields[12];

                calculateBill(b);

                if (!isValidID(b.billId) ||!isValidID(b.patientId) || b.discount < 0 || b.discount > b.consultationFee + b.testCharges +
                 b.medicineCharges + b.roomCharges + b.otherCharges || b.paidAmount > b.totalAmount || b.paymentMode.empty() ||searchBill(b.billId) != -1)
                      continue;

                bills[count++] = b;
            }
            catch (...)
            {
                continue;
            }
        }

        fin.close();
    }

    void displayBill(Bill b)
    {
        cout << fixed << setprecision(2);

        cout << "\n============================================\n";
        cout << "             AROGYA HOSPITAL\n";
        cout << "              PATIENT BILL\n";
        cout << "============================================\n";

        cout << "Bill ID       : " << b.billId << endl;
        cout << "Patient ID    : " << b.patientId << endl;
        cout << "Patient Name  : " << b.patientName << endl;
        cout << "Bill Date     : " << b.billDate << endl;
        cout << "Bill Time     : " << b.billTime << endl;

        cout << "--------------------------------------------\n";
        cout << "Consultation Fee : Rs. " << b.consultationFee << endl;
        cout << "Test Charges     : Rs. " << b.testCharges << endl;
        cout << "Medicine Charges : Rs. " << b.medicineCharges << endl;
        cout << "Room Charges     : Rs. " << b.roomCharges << endl;
        cout << "Other Charges    : Rs. " << b.otherCharges << endl;
        cout << "Discount         : Rs. " << b.discount << endl;

        cout << "--------------------------------------------\n";
        cout << "Total Amount     : Rs. " << b.totalAmount << endl;
        cout << "Amount Paid      : Rs. " << b.paidAmount << endl;
        cout << "Balance Due      : Rs. " << b.balance << endl;
        cout << "Payment Mode     : " << b.paymentMode << endl;

        if (b.balance == 0)
            cout << "Payment Status   : PAID\n";
        else if (b.paidAmount > 0)
            cout << "Payment Status   : PARTIALLY PAID\n";
        else
            cout << "Payment Status   : UNPAID\n";

        cout << "============================================\n";
        cout << "          Thank you for choosing AROGYA!\n";
        cout << "============================================\n";
    }

public:
    BillingManagement()
    {
        count = 0;
        loadBills();
    }

    void generateBill()
    {
        if (count >= MAX_BILLS)
        {
            cout << "Maximum bill limit reached.\n";
            return;
        }

        Bill b;

        cout << "Enter Bill ID: ";
        cin >> b.billId;

        if (!isValidID(b.billId))
        {
            cout << "Invalid Bill ID. Use letters, numbers or hyphens only.\n";
            return;
        }

        if (searchBill(b.billId) != -1)
        {
           cout << "Bill ID already exists.\n";
           return;
        }

        if (searchBill(b.billId) != -1)
        {
            cout << "Bill ID already exists.\n";
            return;
        }

        cout << "Enter Patient ID: ";
        cin >> b.patientId;

        if (!isValidID(b.patientId))
        {
           cout << "Invalid Patient ID. Use letters, numbers or hyphens only.\n";
           return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter Patient Name: ";
        getline(cin, b.patientName);

        if (b.patientName.empty() || b.patientName.find('|') != string::npos)
        {
            cout << "Invalid patient name.\n";
            return;
        }

        getDateTime(b.billDate, b.billTime);

        cout << "\nEnter Billing Details\n";

        b.consultationFee = getAmount("Consultation Fee: Rs. ");
        b.testCharges = getAmount("Test Charges: Rs. ");
        b.medicineCharges = getAmount("Medicine Charges: Rs. ");
        b.roomCharges = getAmount("Room Charges: Rs. ");
        b.otherCharges = getAmount("Other Charges: Rs. ");

        double subtotal = b.consultationFee + b.testCharges +b.medicineCharges + b.roomCharges +b.otherCharges;

        b.discount = getAmount("Discount: Rs. ");

        if (b.discount > subtotal)
        {
            cout << "Discount cannot exceed the subtotal.\n";
            return;
        }

        calculateBill(b);

        cout << "\nTotal Amount: Rs. " << b.totalAmount << endl;

        b.paidAmount = getAmount("Enter Initial Payment: Rs. ");

        if (b.paidAmount > b.totalAmount)
        {
            cout << "Payment cannot exceed the total bill.\n";
            return;
        }

        if (b.paidAmount > 0)
            b.paymentMode = getPaymentMode();
        else
            b.paymentMode = "Not Paid";

        calculateBill(b);

        bills[count++] = b;
        saveBills();

        cout << "\nBill generated successfully!\n";
        displayBill(b);
    }

    void displayAllBills()
    {
        if (count == 0)
        {
            cout << "No billing records found.\n";
            return;
        }

        for (int i = 0; i < count; i++)
            displayBill(bills[i]);
    }

    void searchAndDisplayBill()
{
    string id;

    cout << "Enter Bill ID: ";
    cin >> id;

    if (!isValidID(id))
    {
        cout << "Invalid Bill ID.\n";
        return;
    }

    int index = searchBill(id);

    if (index == -1)
        cout << "Bill not found.\n";
    else
        displayBill(bills[index]);
}

    void updatePayment()
{
    string id;

    cout << "Enter Bill ID: ";
    cin >> id;

    if (!isValidID(id))
    {
        cout << "Invalid Bill ID.\n";
        return;
    }

    int index = searchBill(id);

    if (index == -1)
    {
        cout << "Bill not found.\n";
        return;
    }

    if (bills[index].balance == 0)
    {
        cout << "This bill is already fully paid.\n";
        return;
    }

    double payment = getAmount("Enter Additional Payment: Rs. ");

    if (payment <= 0 || payment > bills[index].balance)
    {
        cout << "Invalid payment amount.\n";
        return;
    }

    bills[index].paidAmount += payment;
    bills[index].paymentMode = getPaymentMode();

    calculateBill(bills[index]);
    saveBills();

    cout << "\nPayment updated successfully!\n";
    displayBill(bills[index]);
}

    void billingSummary()
    {
        double totalBilled = 0;
        double totalCollected = 0;
        double totalPending = 0;
        int paid = 0, unpaid = 0, partial = 0;

        for (int i = 0; i < count; i++)
        {
            totalBilled += bills[i].totalAmount;
            totalCollected += bills[i].paidAmount;
            totalPending += bills[i].balance;

            if (bills[i].balance == 0)
                paid++;
            else if (bills[i].paidAmount > 0)
                partial++;
            else
                unpaid++;
        }

        cout << fixed << setprecision(2);

        cout << "\n========== BILLING SUMMARY ==========\n";
        cout << "Total Bills       : " << count << endl;
        cout << "Total Billed      : Rs. " << totalBilled << endl;
        cout << "Total Collected   : Rs. " << totalCollected << endl;
        cout << "Outstanding Amount: Rs. " << totalPending << endl;
        cout << "Fully Paid Bills  : " << paid << endl;
        cout << "Partially Paid    : " << partial << endl;
        cout << "Unpaid Bills      : " << unpaid << endl;
        cout << "=====================================\n";
    }


void menu()
{
    int choice;

    do
    {
        clearScreen();

        cout << "\n========================================\n";
        cout << "             AROGYA HOSPITAL\n";
        cout << "           BILLING MANAGEMENT\n";
        cout << "========================================\n";
        cout << "1. Generate New Bill\n";
        cout << "2. Display All Bills\n";
        cout << "3. Search Bill\n";
        cout << "4. Update Payment\n";
        cout << "5. Billing Summary\n";
        cout << "0. Exit\n";
        cout << "========================================\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice))
        {
            cout << "\nInvalid input!";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else
        {
            switch (choice)
            {
                case 1:
                    clearScreen();
                    generateBill();
                    break;

                case 2:
                    clearScreen();
                    displayAllBills();
                    break;

                case 3:
                    clearScreen();
                    searchAndDisplayBill();
                    break;

                case 4:
                    clearScreen();
                    updatePayment();
                    break;

                case 5:
                    clearScreen();
                    billingSummary();
                    break;

                case 0:
                    saveBills();
                    cout << "\nThank you for using AROGYA!\n";
                    break;

                default:
                    cout << "\nInvalid choice!";
            }
        }

        if (choice != 0)
        {
            cout << "\n\nPress Enter to return to the menu...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
        }

    } while (choice != 0);
}

    
};

int main()
{
    BillingManagement billing;
    billing.menu();

    return 0;
}
