#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<cstdlib>
#include<cctype>
using namespace std;

struct Doctor
{
    int id;
    string name;
    string specialization;
    string department;
    string shift;
    string status;
};

struct Appointment
{
    int appointmentID;
    int patientID;
    string patientName;
    int doctorID;
    string doctorName;
    string department;
    string date;
    string time;
    string status;
};

struct Node
{
    Appointment data;
    Node *next;
};

class AppointmentManager
{
    Node *head;
    int nextID;

    Doctor doctors[100];
    int doctorCount;

    string trim(string s)
    {
        int start=0;
        int end=s.length()-1;

        while(start<=end && isspace((unsigned char)s[start]))
            start++;

        while(end>=start && isspace((unsigned char)s[end]))
            end--;

        return s.substr(start,end-start+1);
    }

    bool isAvailable(string s)
    {
        for(int i=0;i<s.length();i++)
            s[i]=tolower((unsigned char)s[i]);

        return s=="available";
    }

public:

    AppointmentManager()
    {
        head=NULL;
        nextID=1001;
        doctorCount=0;

        loadDoctors();
        loadAppointments();
    }

    void loadDoctors()
    {
        ifstream in("doctors.txt");
        string line;

        if(!in)
        {
            cout<<"Unable to open doctors.txt\n";
            return;
        }

        while(getline(in,line) && doctorCount<100)
        {
            stringstream ss(line);
            string temp;
            Doctor d;

            if(!getline(ss,temp,'|'))
                continue;

            d.id=atoi(trim(temp).c_str());

            if(!getline(ss,d.name,'|'))
                continue;

            if(!getline(ss,d.specialization,'|'))
                continue;

            if(!getline(ss,d.department,'|'))
                continue;

            if(!getline(ss,d.shift,'|'))
                continue;

            if(!getline(ss,d.status))
                continue;

            d.name=trim(d.name);
            d.specialization=trim(d.specialization);
            d.department=trim(d.department);
            d.shift=trim(d.shift);
            d.status=trim(d.status);

            doctors[doctorCount]=d;
            doctorCount++;
        }

        in.close();
    }

    void insert(Appointment a)
    {
        Node *temp=new Node;

        temp->data=a;
        temp->next=NULL;

        if(head==NULL)
        {
            head=temp;
        }
        else
        {
            Node *t=head;

            while(t->next!=NULL)
                t=t->next;

            t->next=temp;
        }
    }

    void loadAppointments()
    {
        ifstream in("appointments.txt");
        string line;

        while(getline(in,line))
        {
            stringstream ss(line);
            string temp;
            Appointment a;

            if(!getline(ss,temp,'|'))
                continue;

            a.appointmentID=atoi(temp.c_str());

            if(!getline(ss,temp,'|'))
                continue;

            a.patientID=atoi(temp.c_str());

            if(!getline(ss,a.patientName,'|'))
                continue;

            if(!getline(ss,temp,'|'))
                continue;

            a.doctorID=atoi(temp.c_str());

            if(!getline(ss,a.doctorName,'|'))
                continue;

            if(!getline(ss,a.department,'|'))
                continue;

            if(!getline(ss,a.date,'|'))
                continue;

            if(!getline(ss,a.time,'|'))
                continue;

            if(!getline(ss,a.status))
                continue;

            insert(a);

            if(a.appointmentID>=nextID)
                nextID=a.appointmentID+1;
        }

        in.close();
    }

    void saveAppointments()
    {
        ofstream out("appointments.txt");

        if(!out)
        {
            cout<<"Unable to save appointment records.\n";
            return;
        }

        Node *t=head;

        while(t!=NULL)
        {
            Appointment a=t->data;

            out<<a.appointmentID<<"|"
               <<a.patientID<<"|"
               <<a.patientName<<"|"
               <<a.doctorID<<"|"
               <<a.doctorName<<"|"
               <<a.department<<"|"
               <<a.date<<"|"
               <<a.time<<"|"
               <<a.status<<endl;

            t=t->next;
        }

        out.close();
    }

    void displayDoctors()
    {
        int found=0;

        cout<<"\nAVAILABLE DOCTORS\n";
        cout<<"-----------------------------\n";

        for(int i=0;i<doctorCount;i++)
        {
            if(isAvailable(doctors[i].status))
            {
                cout<<"Doctor ID: "<<doctors[i].id<<endl;
                cout<<"Name: "<<doctors[i].name<<endl;
                cout<<"Specialization: "<<doctors[i].specialization<<endl;
                cout<<"Department: "<<doctors[i].department<<endl;
                cout<<"Shift: "<<doctors[i].shift<<endl;
                cout<<"-----------------------------\n";

                found=1;
            }
        }

        if(found==0)
            cout<<"No available doctors found.\n";
    }

    Doctor* findDoctor(int id)
    {
        for(int i=0;i<doctorCount;i++)
        {
            if(doctors[i].id==id && isAvailable(doctors[i].status))
                return &doctors[i];
        }

        return NULL;
    }

    bool checkSlot(int doctorID,string date,string time,int currentID=-1)
    {
        Node *t=head;

        while(t!=NULL)
        {
            if(t->data.doctorID==doctorID &&
               t->data.date==date &&
               t->data.time==time &&
               t->data.status=="Booked" &&
               t->data.appointmentID!=currentID)
            {
                return false;
            }

            t=t->next;
        }

        return true;
    }

    void displayAppointment(Appointment a)
    {
        cout<<"\nAppointment ID: "<<a.appointmentID<<endl;
        cout<<"Patient ID: "<<a.patientID<<endl;
        cout<<"Patient Name: "<<a.patientName<<endl;
        cout<<"Doctor ID: "<<a.doctorID<<endl;
        cout<<"Doctor Name: "<<a.doctorName<<endl;
        cout<<"Department: "<<a.department<<endl;
        cout<<"Date: "<<a.date<<endl;
        cout<<"Time: "<<a.time<<endl;
        cout<<"Status: "<<a.status<<endl;
        cout<<"-----------------------------\n";
    }

    void bookAppointment(int patientID,string patientName)
    {
        if(doctorCount==0)
        {
            cout<<"No doctor records found in doctors.txt.\n";
            return;
        }

        displayDoctors();

        int id;

        cout<<"\nEnter Doctor ID: ";
        cin>>id;

        Doctor *d=findDoctor(id);

        if(d==NULL)
        {
            cout<<"Invalid Doctor ID or doctor unavailable.\n";
            return;
        }

        string date,time;

        cout<<"Enter Date (YYYY-MM-DD): ";
        cin>>date;

        cout<<"Enter Time (HH:MM): ";
        cin>>time;

        if(!checkSlot(id,date,time))
        {
            cout<<"This time slot is already booked.\n";
            return;
        }

        Appointment a;

        a.appointmentID=nextID++;
        a.patientID=patientID;
        a.patientName=patientName;
        a.doctorID=d->id;
        a.doctorName=d->name;
        a.department=d->department;
        a.date=date;
        a.time=time;
        a.status="Booked";

        insert(a);
        saveAppointments();

        cout<<"\nAppointment booked successfully!\n";
        displayAppointment(a);
    }

    void viewAppointments(int patientID)
    {
        Node *t=head;
        int found=0;

        cout<<"\nYOUR APPOINTMENTS\n";

        while(t!=NULL)
        {
            if(t->data.patientID==patientID)
            {
                displayAppointment(t->data);
                found=1;
            }

            t=t->next;
        }

        if(found==0)
            cout<<"No appointments found.\n";
    }

    void searchAppointment(int patientID)
    {
        int id;

        cout<<"Enter Appointment ID: ";
        cin>>id;

        Node *t=head;

        while(t!=NULL)
        {
            if(t->data.appointmentID==id &&
               t->data.patientID==patientID)
            {
                displayAppointment(t->data);
                return;
            }

            t=t->next;
        }

        cout<<"Appointment not found.\n";
    }

    void cancelAppointment(int patientID)
    {
        int id;

        cout<<"Enter Appointment ID to cancel: ";
        cin>>id;

        Node *t=head;

        while(t!=NULL)
        {
            if(t->data.appointmentID==id &&
               t->data.patientID==patientID)
            {
                if(t->data.status=="Cancelled")
                {
                    cout<<"Appointment is already cancelled.\n";
                    return;
                }

                t->data.status="Cancelled";

                saveAppointments();

                cout<<"Appointment cancelled successfully.\n";
                return;
            }

            t=t->next;
        }

        cout<<"Appointment not found.\n";
    }

    void rescheduleAppointment(int patientID)
    {
        int id;

        cout<<"Enter Appointment ID: ";
        cin>>id;

        Node *t=head;

        while(t!=NULL)
        {
            if(t->data.appointmentID==id &&
               t->data.patientID==patientID)
            {
                if(t->data.status=="Cancelled")
                {
                    cout<<"Cancelled appointment cannot be rescheduled.\n";
                    return;
                }

                string date,time;

                cout<<"Enter New Date (YYYY-MM-DD): ";
                cin>>date;

                cout<<"Enter New Time (HH:MM): ";
                cin>>time;

                if(!checkSlot(t->data.doctorID,date,time,id))
                {
                    cout<<"This time slot is already booked.\n";
                    return;
                }

                t->data.date=date;
                t->data.time=time;

                saveAppointments();

                cout<<"Appointment rescheduled successfully.\n";
                displayAppointment(t->data);

                return;
            }

            t=t->next;
        }

        cout<<"Appointment not found.\n";
    }

    void sortAppointments(int patientID)
    {
        Node *i,*j;

        for(i=head;i!=NULL;i=i->next)
        {
            if(i->data.patientID==patientID)
            {
                for(j=i->next;j!=NULL;j=j->next)
                {
                    if(j->data.patientID==patientID)
                    {
                        if(i->data.date>j->data.date ||
                          (i->data.date==j->data.date &&
                           i->data.time>j->data.time))
                        {
                            Appointment temp=i->data;
                            i->data=j->data;
                            j->data=temp;
                        }
                    }
                }
            }
        }

        saveAppointments();

        cout<<"\nAppointments sorted successfully.\n";
        viewAppointments(patientID);
    }

    void menu(int patientID,string patientName)
    {
        int choice;

        do
        {
            cout<<"\n===== APPOINTMENT MANAGEMENT =====\n";
            cout<<"1. Book Appointment\n";
            cout<<"2. View My Appointments\n";
            cout<<"3. Search Appointment\n";
            cout<<"4. Cancel Appointment\n";
            cout<<"5. Reschedule Appointment\n";
            cout<<"6. Sort Appointments\n";
            cout<<"0. Back to Dashboard\n";

            cout<<"\nEnter your choice: ";
            cin>>choice;

            switch(choice)
            {
                case 1:
                    bookAppointment(patientID,patientName);
                    break;

                case 2:
                    viewAppointments(patientID);
                    break;

                case 3:
                    searchAppointment(patientID);
                    break;

                case 4:
                    cancelAppointment(patientID);
                    break;

                case 5:
                    rescheduleAppointment(patientID);
                    break;

                case 6:
                    sortAppointments(patientID);
                    break;

                case 0:
                    cout<<"Returning to dashboard.\n";
                    break;

                default:
                    cout<<"Invalid choice.\n";
            }

        }while(choice!=0);
    }

    ~AppointmentManager()
    {
        Node *t;

        while(head!=NULL)
        {
            t=head;
            head=head->next;
            delete t;
        }
    }
};

void appointmentMenu(int patientID,string patientName)
{
    AppointmentManager obj;
    obj.menu(patientID,patientName);
}

int main()
{
    int patientID;
    string patientName;

    cout<<"===== AROGYA APPOINTMENT MANAGEMENT =====\n";

    cout<<"Enter Patient ID: ";
    cin>>patientID;

    cin.ignore(10000,'\n');

    cout<<"Enter Patient Name: ";
    getline(cin,patientName);

    appointmentMenu(patientID,patientName);

    return 0;
}