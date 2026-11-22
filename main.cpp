#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "crow_all.h"

// Helper function to read standalone HTML/CSS/JS files with explicit paths
std::string read_file(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::ifstream altFile("crow-project/" + filepath);
        std::stringstream altBuffer;
        altBuffer << altFile.rdbuf();
        return altBuffer.str();
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Track current logged-in user session globally for the web app
std::string current_user_email = "user@arogya.com";
std::string current_user_role = "Patient";

// Include all backend modules directly into the translation unit
#include "modules/user.cpp"
#include "modules/dr.cpp"
#include "modules/staff.cpp"
#include "modules/bed_ward_management.cpp"
#include "modules/patient.cpp"
#include "modules/appoint.cpp"
#include "modules/billing_management.cpp"
#include "modules/emergency.cpp"
#include "modules/medical_records.cpp"

struct BillingRecord {
    std::string billId;
    std::string patientId;
    std::string patientName;
    std::string billDate;
    std::string billTime;
    double consultationFee = 0.0;
    double testCharges = 0.0;
    double medicineCharges = 0.0;
    double roomCharges = 0.0;
    double otherCharges = 0.0;
    double discount = 0.0;
    double totalAmount = 0.0;
    double paidAmount = 0.0;
    double balance = 0.0;
    std::string paymentMode = "Cash";
};

inline void computeBillTotals(BillingRecord& b) {
    double subtotal = b.consultationFee + b.testCharges + b.medicineCharges + b.roomCharges + b.otherCharges;
    if (b.discount < 0.0) b.discount = 0.0;
    if (b.discount > subtotal) b.discount = subtotal;
    b.totalAmount = subtotal - b.discount;
    if (b.paidAmount < 0.0) b.paidAmount = 0.0;
    if (b.paidAmount > b.totalAmount) b.paidAmount = b.totalAmount;
    b.balance = b.totalAmount - b.paidAmount;
}

inline std::vector<BillingRecord> loadBillingRecords() {
    std::vector<BillingRecord> list;
    std::ifstream fin("txt-files/billing.txt");
    if (!fin.is_open()) fin.open("billing.txt");

    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string fields[13];
        bool ok = true;
        for (int i = 0; i < 13; i++) {
            if (!std::getline(ss, fields[i], '|')) {
                ok = false;
                break;
            }
        }
        if (!ok) continue;

        try {
            BillingRecord b;
            b.billId = fields[0];
            b.patientId = fields[1];
            b.patientName = fields[2];
            b.billDate = fields[3];
            b.billTime = fields[4];
            b.consultationFee = std::stod(fields[5]);
            b.testCharges = std::stod(fields[6]);
            b.medicineCharges = std::stod(fields[7]);
            b.roomCharges = std::stod(fields[8]);
            b.otherCharges = std::stod(fields[9]);
            b.discount = std::stod(fields[10]);
            b.paidAmount = std::stod(fields[11]);
            b.paymentMode = fields[12];
            computeBillTotals(b);
            list.push_back(b);
        } catch (...) {
            continue;
        }
    }
    return list;
}

inline void saveBillingRecords(const std::vector<BillingRecord>& list) {
    std::ofstream fout("txt-files/billing.txt");
    if (!fout.is_open()) fout.open("billing.txt");
    for (const auto& b : list) {
        fout << b.billId << "|" << b.patientId << "|" << b.patientName << "|"
             << b.billDate << "|" << b.billTime << "|"
             << b.consultationFee << "|" << b.testCharges << "|"
             << b.medicineCharges << "|" << b.roomCharges << "|"
             << b.otherCharges << "|" << b.discount << "|"
             << b.paidAmount << "|" << b.paymentMode << "\n";
    }
}

inline bool isAppointmentSlotFree(int doctorId, const std::string& date, const std::string& time) {
    std::ifstream in("txt-files/appointments.txt");
    if (!in.is_open()) in.open("appointments.txt");
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string c1, c2, c3, dId, c5, c6, dDate, dTime, dStatus;
        if (std::getline(ss, c1, '|') && std::getline(ss, c2, '|') &&
            std::getline(ss, c3, '|') && std::getline(ss, dId, '|') &&
            std::getline(ss, c5, '|') && std::getline(ss, c6, '|') &&
            std::getline(ss, dDate, '|') && std::getline(ss, dTime, '|') &&
            std::getline(ss, dStatus)) {
            if (std::atoi(dId.c_str()) == doctorId && dDate == date && dTime == time && dStatus != "Cancelled") {
                return false;
            }
        }
    }
    return true;
}

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/ambulence.html")([](){
        return crow::mustache::load("ambulence.html").render();
    });

    CROW_ROUTE(app, "/api/medical-records/list").methods("GET"_method)
    ([](){
        MedicalRecords med;
        std::ifstream file("txt-files/medical_records.txt");
        if (!file.is_open()) file.open("medical_records.txt");

        crow::json::wvalue arr = crow::json::wvalue::list();
        std::string line;
        size_t idx = 0;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string f[17];
            bool ok = true;
            for (int i = 0; i < 17; i++) {
                if (!std::getline(ss, f[i], '|')) {
                    ok = false;
                    break;
                }
            }
            if (!ok) continue;
            arr[idx]["recordId"] = f[0];
            arr[idx]["patientId"] = f[1];
            arr[idx]["patientName"] = f[2];
            arr[idx]["age"] = std::atoi(f[3].c_str());
            arr[idx]["gender"] = f[4];
            arr[idx]["department"] = f[5];
            arr[idx]["bloodGroup"] = f[6];
            arr[idx]["diagnosis"] = f[7];
            arr[idx]["symptoms"] = f[8];
            arr[idx]["doctorName"] = f[9];
            arr[idx]["treatment"] = f[10];
            arr[idx]["medicines"] = f[11];
            arr[idx]["dosage"] = f[12];
            arr[idx]["duration"] = f[13];
            arr[idx]["allergies"] = f[14];
            arr[idx]["recordDate"] = f[15];
            arr[idx]["recordTime"] = f[16];
            idx++;
        }

        crow::response res(200, arr.dump());
        res.set_header("Content-Type", "application/json");
        return res;
    });

    CROW_ROUTE(app, "/api/emergency/cases").methods("GET"_method)
    ([](){
        std::ifstream file("txt-files/emergency.txt");
        if (!file.is_open()) file.open("emergency.txt");

        crow::json::wvalue arr = crow::json::wvalue::list();
        std::string line;
        size_t idx = 0;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string f[10];
            bool ok = true;
            for (int i = 0; i < 10; i++) {
                if (!std::getline(ss, f[i], '|')) {
                    ok = false;
                    break;
                }
            }
            if (!ok) continue;
            arr[idx]["caseID"] = std::atoi(f[0].c_str());
            arr[idx]["patientID"] = std::atoi(f[1].c_str());
            arr[idx]["patientName"] = f[2];
            arr[idx]["age"] = std::atoi(f[3].c_str());
            arr[idx]["department"] = f[4];
            arr[idx]["priority"] = std::atoi(f[5].c_str());
            arr[idx]["doctorID"] = std::atoi(f[6].c_str());
            arr[idx]["doctorName"] = f[7];
            arr[idx]["status"] = f[8];
            arr[idx]["arrival"] = std::atoi(f[9].c_str());
            idx++;
        }

        crow::response res(200, arr.dump());
        res.set_header("Content-Type", "application/json");
        return res;
    });

    CROW_ROUTE(app, "/api/billing/list").methods("GET"_method)
    ([](){
        auto bills = loadBillingRecords();
        crow::json::wvalue arr = crow::json::wvalue::list();
        for (size_t i = 0; i < bills.size(); i++) {
            arr[i]["billId"] = bills[i].billId;
            arr[i]["patientId"] = bills[i].patientId;
            arr[i]["patientName"] = bills[i].patientName;
            arr[i]["billDate"] = bills[i].billDate;
            arr[i]["consultationFee"] = bills[i].consultationFee;
            arr[i]["testCharges"] = bills[i].testCharges;
            arr[i]["medicineCharges"] = bills[i].medicineCharges;
            arr[i]["roomCharges"] = bills[i].roomCharges;
            arr[i]["otherCharges"] = bills[i].otherCharges;
            arr[i]["discount"] = bills[i].discount;
            arr[i]["totalAmount"] = bills[i].totalAmount;
            arr[i]["paidAmount"] = bills[i].paidAmount;
            arr[i]["balance"] = bills[i].balance;
            arr[i]["paymentMode"] = bills[i].paymentMode;
        }
        crow::response res(200, arr.dump());
        res.set_header("Content-Type", "application/json");
        return res;
    });

    CROW_ROUTE(app, "/api/billing/generate").methods("POST"_method)
    ([](const crow::request& req){
        crow::response res;
        res.set_header("Content-Type", "application/json");
        auto x = crow::json::load(req.body);
        if (!x || !x.has("billId") || !x.has("patientId") || !x.has("patientName")) {
            res.code = 400;
            res.write("{\"message\":\"Missing required billing fields\"}");
            return res;
        }

        auto bills = loadBillingRecords();
        BillingRecord b;
        b.billId = std::string(x["billId"].s());
        b.patientId = std::string(x["patientId"].s());
        b.patientName = std::string(x["patientName"].s());
        b.billDate = x.has("billDate") ? std::string(x["billDate"].s()) : "22-11-2026";
        b.billTime = x.has("billTime") ? std::string(x["billTime"].s()) : "00:00:00";
        b.consultationFee = x.has("consultationFee") ? x["consultationFee"].d() : 0.0;
        b.testCharges = x.has("testCharges") ? x["testCharges"].d() : 0.0;
        b.medicineCharges = x.has("medicineCharges") ? x["medicineCharges"].d() : 0.0;
        b.roomCharges = x.has("roomCharges") ? x["roomCharges"].d() : 0.0;
        b.otherCharges = x.has("otherCharges") ? x["otherCharges"].d() : 0.0;
        b.discount = x.has("discount") ? x["discount"].d() : 0.0;
        b.paidAmount = x.has("paidAmount") ? x["paidAmount"].d() : 0.0;
        b.paymentMode = x.has("paymentMode") ? std::string(x["paymentMode"].s()) : "Cash";

        computeBillTotals(b);
        bills.push_back(b);
        saveBillingRecords(bills);

        res.code = 200;
        res.write("{\"message\":\"Bill generated successfully\"}");
        return res;
    });

CROW_ROUTE(app, "/api/billing/pay").methods("POST"_method)
    ([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("billId") || !body.has("amount")) {
            return crow::response(400, "{\"message\":\"billId and amount are required\"}");
        }

        string billId = body["billId"].s();
        double amount = body["amount"].d();
        string paymentMode = body.has("paymentMode") ? string(body["paymentMode"].s()) : "UPI";

        if (amount <= 0) {
            return crow::response(400, "{\"message\":\"Payment amount must be greater than zero\"}");
        }

        vector<BillingRecord> bills = loadBillingRecords();
        bool found = false;

        for (auto& b : bills) {
            if (b.billId == billId) {
                found = true;
                double gross = b.consultationFee + b.testCharges + b.medicineCharges + b.roomCharges + b.otherCharges;
                double netTotal = gross - b.discount;
                if (netTotal < 0) netTotal = 0;

                double currentBalance = netTotal - b.paidAmount;
                if (currentBalance <= 0) {
                    return crow::response(400, "{\"message\":\"This bill is already fully paid\"}");
                }
                if (amount > currentBalance) {
                    amount = currentBalance;
                }

                b.paidAmount += amount;
                b.paymentMode = paymentMode;
                break;
            }
        }

        if (!found) {
            return crow::response(404, "{\"message\":\"Bill ID not found\"}");
        }

        saveBillingRecords(bills);
        return crow::response(200, "{\"message\":\"Payment recorded successfully\"}");
    });

    CROW_ROUTE(app, "/api/appointments/check-slot").methods("GET"_method)
    ([](const crow::request& req){
        int docId = req.url_params.get("doctorId") ? std::atoi(req.url_params.get("doctorId")) : 0;
        std::string date = req.url_params.get("date") ? req.url_params.get("date") : "";
        std::string time = req.url_params.get("time") ? req.url_params.get("time") : "";

        bool freeSlot = isAppointmentSlotFree(docId, date, time);
        crow::json::wvalue out;
        out["available"] = freeSlot;
        crow::response res(200, out.dump());
        res.set_header("Content-Type", "application/json");
        return res;
    });

    // --- Emergency Cases (Read & Write -> txt-files/emergency.txt) ---
    CROW_ROUTE(app, "/api/emergency/add").methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        auto x = crow::json::load(req.body);
        if (!x || !x.has("patientName") || !x.has("department")) {
            res.code = 400;
            res.write("{\"message\":\"Patient name and department are required\"}");
            res.end();
            return;
        }

        int nextCaseId = 1;
        int nextArrival = 1;
        std::ifstream in("txt-files/emergency.txt");
        if (!in.is_open()) in.open("emergency.txt");
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string f[10];
            bool ok = true;
            for (int i = 0; i < 10; i++) {
                if (!std::getline(ss, f[i], '|')) { ok = false; break; }
            }
            if (!ok) continue;
            try {
                int cid = std::stoi(f[0]);
                int arr = std::stoi(f[9]);
                if (cid >= nextCaseId) nextCaseId = cid + 1;
                if (arr >= nextArrival) nextArrival = arr + 1;
            } catch (...) {}
        }
        in.close();

        int patientId = x.has("patientID") ? std::atoi(std::string(x["patientID"].s()).c_str()) : (1000 + nextCaseId);
        if (patientId <= 0) patientId = 1000 + nextCaseId;
        std::string patientName = sanitize(std::string(x["patientName"].s()));
        int age = x.has("age") ? std::atoi(std::string(x["age"].s()).c_str()) : 30;
        if (age <= 0) age = 30;
        std::string dept = sanitize(std::string(x["department"].s()));
        int priority = x.has("priority") ? std::atoi(std::string(x["priority"].s()).c_str()) : 1;
        if (priority < 1 || priority > 3) priority = 1;

        std::ofstream out("txt-files/emergency.txt", std::ios::app);
        if (!out.is_open()) out.open("emergency.txt", std::ios::app);
        out << nextCaseId << "|" << patientId << "|" << patientName << "|"
            << age << "|" << dept << "|" << priority << "|-1|Not Assigned|Waiting|"
            << nextArrival << "\n";
        out.close();

        res.code = 200;
        res.write("{\"message\":\"Emergency case logged to emergency.txt\"}");
        res.end();
    });

    CROW_ROUTE(app, "/api/emergency/update").methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        auto x = crow::json::load(req.body);
        if (!x || !x.has("caseID")) {
            res.code = 400;
            res.write("{\"message\":\"caseID is required\"}");
            res.end();
            return;
        }

        int targetId = x["caseID"].i();
        std::string newStatus = x.has("status") ? sanitize(std::string(x["status"].s())) : "";
        int docId = x.has("doctorID") ? std::atoi(std::string(x["doctorID"].s()).c_str()) : -1;
        std::string docName = x.has("doctorName") ? sanitize(std::string(x["doctorName"].s())) : "";

        std::ifstream in("txt-files/emergency.txt");
        if (!in.is_open()) in.open("emergency.txt");
        std::vector<std::string> lines;
        std::string line;
        bool found = false;

        while (std::getline(in, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string f[10];
            bool ok = true;
            for (int i = 0; i < 10; i++) {
                if (!std::getline(ss, f[i], '|')) { ok = false; break; }
            }
            if (!ok) { lines.push_back(line); continue; }

            if (std::atoi(f[0].c_str()) == targetId) {
                found = true;
                if (docId > 0) f[6] = std::to_string(docId);
                if (!docName.empty()) f[7] = docName;
                if (!newStatus.empty()) {
                    f[8] = newStatus;
                } else if (!docName.empty() && f[8] == "Waiting") {
                    f[8] = "Assigned";
                }
                lines.push_back(
                    f[0] + "|" + f[1] + "|" + f[2] + "|" + f[3] + "|" +
                    f[4] + "|" + f[5] + "|" + f[6] + "|" + f[7] + "|" +
                    f[8] + "|" + f[9]
                );
            } else {
                lines.push_back(line);
            }
        }
        in.close();

        if (!found) {
            res.code = 404;
            res.write("{\"message\":\"Emergency Case ID not found\"}");
            res.end();
            return;
        }

        std::ofstream out("txt-files/emergency.txt");
        if (!out.is_open()) out.open("emergency.txt");
        for (const auto& l : lines) out << l << "\n";
        out.close();

        res.code = 200;
        res.write("{\"message\":\"Emergency case updated\"}");
        res.end();
    });

    // --- Investigations (Read & Write -> txt-files/investigations.txt) ---
    CROW_ROUTE(app, "/api/investigations/list").methods("GET"_method)
    ([](){
        std::ifstream in("txt-files/investigations.txt");
        if (!in.is_open()) in.open("investigations.txt");
        crow::json::wvalue arr = crow::json::wvalue::list();
        std::string line;
        size_t idx = 0;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string f[9];
            bool ok = true;
            for (int i = 0; i < 9; i++) {
                if (!std::getline(ss, f[i], '|')) { ok = false; break; }
            }
            if (!ok) continue;
            arr[idx]["investigationId"] = f[0];
            arr[idx]["recordId"] = f[1];
            arr[idx]["patientId"] = f[2];
            arr[idx]["testName"] = f[3];
            arr[idx]["bodyPart"] = f[4];
            arr[idx]["testDate"] = f[5];
            arr[idx]["findings"] = f[6];
            arr[idx]["remarks"] = f[7];
            arr[idx]["status"] = f[8];
            idx++;
        }
        in.close();
        crow::response res(200, arr.dump());
        res.set_header("Content-Type", "application/json");
        return res;
    });

    CROW_ROUTE(app, "/api/investigations/add").methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        auto x = crow::json::load(req.body);
        if (!x || !x.has("investigationId") || !x.has("patientId") || !x.has("testName")) {
            res.code = 400;
            res.write("{\"message\":\"Investigation ID, Patient ID, and Test Name are required\"}");
            res.end();
            return;
        }

        std::string invId = sanitize(std::string(x["investigationId"].s()));
        std::string recId = x.has("recordId") ? sanitize(std::string(x["recordId"].s())) : "R-01";
        if (recId.empty()) recId = "R-01";
        std::string patId = sanitize(std::string(x["patientId"].s()));
        std::string testName = sanitize(std::string(x["testName"].s()));
        std::string bodyPart = x.has("bodyPart") ? sanitize(std::string(x["bodyPart"].s())) : "General";
        if (bodyPart.empty()) bodyPart = "General";
        std::string testDate = x.has("testDate") ? sanitize(std::string(x["testDate"].s())) : "22-11-2026";
        if (testDate.empty()) testDate = "22-11-2026";
        std::string findings = x.has("findings") ? sanitize(std::string(x["findings"].s())) : "Pending";
        if (findings.empty()) findings = "Pending";
        std::string remarks = x.has("remarks") ? sanitize(std::string(x["remarks"].s())) : "Pending";
        if (remarks.empty()) remarks = "Pending";
        std::string status = x.has("status") ? sanitize(std::string(x["status"].s())) : "Pending";
        if (status.empty()) status = "Pending";

        std::ofstream out("txt-files/investigations.txt", std::ios::app);
        if (!out.is_open()) out.open("investigations.txt", std::ios::app);
        out << invId << "|" << recId << "|" << patId << "|"
            << testName << "|" << bodyPart << "|" << testDate << "|"
            << findings << "|" << remarks << "|" << status << "\n";
        out.close();

        res.code = 200;
        res.write("{\"message\":\"Investigation saved to investigations.txt\"}");
        res.end();
    });

    CROW_ROUTE(app, "/api/investigations/update").methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        auto x = crow::json::load(req.body);
        if (!x || !x.has("investigationId")) {
            res.code = 400;
            res.write("{\"message\":\"investigationId is required\"}");
            res.end();
            return;
        }

        std::string targetId = std::string(x["investigationId"].s());
        std::string status = x.has("status") ? sanitize(std::string(x["status"].s())) : "Completed";
        std::string findings = x.has("findings") ? sanitize(std::string(x["findings"].s())) : "";
        std::string remarks = x.has("remarks") ? sanitize(std::string(x["remarks"].s())) : "";

        std::ifstream in("txt-files/investigations.txt");
        if (!in.is_open()) in.open("investigations.txt");
        std::vector<std::string> lines;
        std::string line;
        bool found = false;

        while (std::getline(in, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string f[9];
            bool ok = true;
            for (int i = 0; i < 9; i++) {
                if (!std::getline(ss, f[i], '|')) { ok = false; break; }
            }
            if (!ok) { lines.push_back(line); continue; }

            if (f[0] == targetId) {
                found = true;
                if (!status.empty()) f[8] = status;
                if (!findings.empty()) f[6] = findings;
                if (!remarks.empty()) f[7] = remarks;
                lines.push_back(
                    f[0] + "|" + f[1] + "|" + f[2] + "|" + f[3] + "|" +
                    f[4] + "|" + f[5] + "|" + f[6] + "|" + f[7] + "|" + f[8]
                );
            } else {
                lines.push_back(line);
            }
        }
        in.close();

        if (!found) {
            res.code = 404;
            res.write("{\"message\":\"Investigation ID not found\"}");
            res.end();
            return;
        }

        std::ofstream out("txt-files/investigations.txt");
        if (!out.is_open()) out.open("investigations.txt");
        for (const auto& l : lines) out << l << "\n";
        out.close();

        res.code = 200;
        res.write("{\"message\":\"Investigation updated\"}");
        res.end();
    });

    // --- Staff Appointment Booking (Write -> txt-files/appointments.txt) ---
    CROW_ROUTE(app, "/api/staff/book-appointment").methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        auto x = crow::json::load(req.body);
        if (!x || !x.has("patientEmail") || !x.has("doctor") || !x.has("date") || !x.has("time")) {
            res.code = 400;
            res.write("{\"message\":\"Patient, doctor, date, and time are required\"}");
            res.end();
            return;
        }

        std::string pEmail = sanitize(std::string(x["patientEmail"].s()));
        std::string doctor = sanitize(std::string(x["doctor"].s()));
        std::string dept = x.has("department") ? sanitize(std::string(x["department"].s())) : "General";
        std::string date = sanitize(std::string(x["date"].s()));
        std::string time = sanitize(std::string(x["time"].s()));
        std::string reason = x.has("reason") ? sanitize(std::string(x["reason"].s())) : "Consultation";

        std::ofstream out("txt-files/appointments.txt", std::ios::app);
        if (!out.is_open()) out.open("appointments.txt", std::ios::app);
        out << pEmail << "|" << doctor << "|" << dept << "|" << date << "|" << time << "|" << reason << "\n";
        out.close();

        res.code = 200;
        res.write("{\"message\":\"Appointment scheduled in appointments.txt\"}");
        res.end();
    });
    
    // Initialize user hash table and load existing users from users.txt
    initialize();
    loadUsers();
    
    // Set global mustache base directory for HTML templates
    crow::mustache::set_global_base("crow-project/templates");

    // ==========================================
    // 1. STATIC FILE ROUTES (CSS / JS / Images)
    // ==========================================
    CROW_ROUTE(app, "/static/<string>")
    ([](const crow::request& req, crow::response& res, std::string filename) {
        std::string fullPath = "crow-project/static/" + filename;
        std::ifstream testFile(fullPath);
        if (!testFile.is_open()) {
            fullPath = "static/" + filename;
        }
        res.set_static_file_info(fullPath);
        res.end();
    });

    CROW_ROUTE(app, "/admin-dashboard.css")
    ([](){
        crow::response res;
        res.body = read_file("crow-project/static/admin-dashboard.css");
        res.set_header("Content-Type", "text/css");
        return res;
    });

    CROW_ROUTE(app, "/admin-dashboard.js")
    ([](){
        crow::response res;
        res.body = read_file("crow-project/static/admin-dashboard.js");
        res.set_header("Content-Type", "application/javascript");
        return res;
    });

    // ==========================================
    // 2. WEBPAGE ROUTES (Templates)
    // ==========================================

    CROW_ROUTE(app, "/")([](){
        return crow::mustache::load("index.html").render();
    });

    CROW_ROUTE(app, "/signin")([](){
        return crow::mustache::load("signin.html").render();
    });

    CROW_ROUTE(app, "/signin.html")([](){
        return crow::mustache::load("signin.html").render();
    });

    // Patient Portal Pages
    CROW_ROUTE(app, "/patient-dashboard.html")([](){
        return crow::mustache::load("patient-dashboard.html").render();
    });

    CROW_ROUTE(app, "/appointments.html")([](){
        return crow::mustache::load("appointments.html").render();
    });

    CROW_ROUTE(app, "/queue-status.html")([](){
        return crow::mustache::load("queue-status.html").render();
    });

    CROW_ROUTE(app, "/medical-records.html")([](){
        return crow::mustache::load("medical-records.html").render();
    });

    CROW_ROUTE(app, "/billing.html")([](){
        return crow::mustache::load("billing.html").render();
    });

    // Admin & Staff Dashboard Pages
    CROW_ROUTE(app, "/emergency-triage.html")([](){
        return crow::mustache::load("emergency-triage.html").render();
    });

    CROW_ROUTE(app, "/bed-management.html")([](){
        return crow::mustache::load("bed-management.html").render();
    });

    CROW_ROUTE(app, "/reports.html")([](){
        return crow::mustache::load("reports.html").render();
    });

    CROW_ROUTE(app, "/admin-dashboard.html")([](){
        return crow::mustache::load("admin-dashboard.html").render();
    });

    CROW_ROUTE(app, "/doctor-dashboard.html")([](){
        return crow::mustache::load("doctor-dashboard.html").render();
    });

    CROW_ROUTE(app, "/doctor-appointments.html")([](){
        return crow::mustache::load("doctor-appointments.html").render();
    });

    CROW_ROUTE(app, "/doctor-patients.html")([](){
        return crow::mustache::load("doctor-patients.html").render();
    });

    CROW_ROUTE(app, "/doctor-triage.html")([](){
        return crow::mustache::load("doctor-triage.html").render();
    });

    CROW_ROUTE(app, "/staff-dashboard.html")([](){
        return crow::mustache::load("staff-dashboard.html").render();
    });

    CROW_ROUTE(app, "/staff.html")([](){
        return crow::mustache::load("staff.html").render();
    });

    CROW_ROUTE(app, "/patients.html")([](){
        return crow::mustache::load("patients.html").render();
    });

    CROW_ROUTE(app, "/triage.html")([](){
        return crow::mustache::load("triage.html").render();
    });

    CROW_ROUTE(app, "/billing-staff.html")([](){
        return crow::mustache::load("billing-staff.html").render();
    });

    // ==========================================
    // 3. AUTHENTICATION & SESSION APIS (Role-Aware)
    // ==========================================

    CROW_ROUTE(app, "/api/doctors/add").methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        auto x = crow::json::load(req.body);
        if (!x || !x.has("name")) {
            res.code = 400;
            res.write("{\"message\":\"Missing doctor name\"}");
            res.end();
            return;
        }

        int newId = 2001 + static_cast<int>(loadDoctors().size());
        while (!doctorIdValidForNewRecord(newId)) newId++;

        Doctor d;
        d.id = newId;
        d.name = sanitize(std::string(x["name"].s()));
        d.specialization = x.has("specialization") ? sanitize(std::string(x["specialization"].s())) : "General Physician";
        d.department = x.has("department") ? sanitize(std::string(x["department"].s())) : "General";
        d.shift = x.has("shift") ? sanitize(std::string(x["shift"].s())) : "Morning";
        d.status = x.has("status") ? sanitize(std::string(x["status"].s())) : "Available";

        saveDoctor(d);
        res.code = 200;
        res.write("{\"message\":\"Doctor added to doctors.txt\"}");
        res.end();
    });

    CROW_ROUTE(app, "/api/staff/add").methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        auto x = crow::json::load(req.body);
        if (!x || !x.has("name")) {
            res.code = 400;
            res.write("{\"message\":\"Missing staff name\"}");
            res.end();
            return;
        }

        int newId = 3001 + static_cast<int>(loadStaff().size());
        while (!staffIdValidForNewRecord(newId)) newId++;

        Staff s;
        s.id = newId;
        s.name = sanitize(std::string(x["name"].s()));
        s.role = x.has("role") ? sanitize(std::string(x["role"].s())) : "Staff";
        s.department = x.has("department") ? sanitize(std::string(x["department"].s())) : "Operations";
        s.shift = x.has("shift") ? sanitize(std::string(x["shift"].s())) : "Morning";
        s.status = x.has("status") ? sanitize(std::string(x["status"].s())) : "Available";

        saveStaff(s);
        res.code = 200;
        res.write("{\"message\":\"Staff added to staff.txt\"}");
        res.end();
    });
    
    CROW_ROUTE(app, "/api/register").methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        auto x = crow::json::load(req.body);
        res.set_header("Content-Type", "application/json");
        
        if (!x) {
            res.code = 400;
            res.write("{\"message\": \"Invalid JSON payload\"}");
            res.end();
            return;
        }
        
        std::string fullName = x["name"].s();
        std::string email = x["email"].s();
        std::string password = x["password"].s();
        
        std::string role = "Patient";
        if (x.has("role")) {
            role = x["role"].s();
        }

        std::string errorMsg;
        if (registerUserWeb(fullName, email, password, role, errorMsg)) {
            res.code = 200;
            res.write("{\"message\": \"Registration successful!\"}");
            res.end();
        } else {
            res.code = 400;
            res.write("{\"message\": \"" + errorMsg + "\"}");
            res.end();
        }
    });

    CROW_ROUTE(app, "/api/login").methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        auto x = crow::json::load(req.body);
        res.set_header("Content-Type", "application/json");

        if (!x) {
            res.code = 400;
            res.write("{\"message\": \"Invalid JSON payload\"}");
            res.end();
            return;
        }
        
        std::string username = x["email"].s();
        std::string password = x["password"].s();

        std::string role, errorMsg;
        if (loginUserWeb(username, password, role, errorMsg)) {
            current_user_email = username;
            current_user_role = role;
            res.code = 200;
            res.write("{\"message\": \"Login successful!\", \"role\": \"" + role + "\"}");
            res.end();
        } else {
            res.code = 401;
            res.write("{\"message\": \"" + errorMsg + "\"}");
            res.end();
        }
    });

    CROW_ROUTE(app, "/api/session").methods("GET"_method)
    ([](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        res.code = 200;
        res.write("{\"role\": \"" + current_user_role + "\", \"email\": \"" + current_user_email + "\"}");
        res.end();
    });

    CROW_ROUTE(app, "/api/logout").methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        current_user_email = "";
        current_user_role = "guest";
        res.code = 200;
        res.write("{\"message\": \"Logged out successfully\"}");
        res.end();
    });

    // ==========================================
    // 4. BACKEND MODULE API HOOKS (Linked to txt-files/)
    // ==========================================

    CROW_ROUTE(app, "/api/doctors/list").methods("GET"_method)
    ([](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        vector<Doctor> docs = loadDoctors();
        
        crow::json::wvalue jsonRes = crow::json::wvalue::list();
        for(size_t i = 0; i < docs.size(); i++) {
            jsonRes[i]["id"] = docs[i].id;
            jsonRes[i]["name"] = docs[i].name;
            jsonRes[i]["specialization"] = docs[i].specialization;
            jsonRes[i]["department"] = docs[i].department;
            jsonRes[i]["shift"] = docs[i].shift;
            jsonRes[i]["status"] = docs[i].status;
        }
        res.code = 200;
        res.write(jsonRes.dump());
        res.end();
    });

    CROW_ROUTE(app, "/api/staff/list").methods("GET"_method)
    ([](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        vector<Staff> staffList = loadStaff();
        
        crow::json::wvalue jsonRes = crow::json::wvalue::list();
        for(size_t i = 0; i < staffList.size(); i++) {
            jsonRes[i]["id"] = staffList[i].id;
            jsonRes[i]["name"] = staffList[i].name;
            jsonRes[i]["role"] = staffList[i].role;
            jsonRes[i]["department"] = staffList[i].department;
            jsonRes[i]["shift"] = staffList[i].shift;
            jsonRes[i]["status"] = staffList[i].status;
        }
        res.code = 200;
        res.write(jsonRes.dump());
        res.end();
    });

    CROW_ROUTE(app, "/api/patients/list").methods("GET"_method)
    ([](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        vector<Patient> patients = loadPatients();
        
        crow::json::wvalue jsonRes = crow::json::wvalue::list();
        for(size_t i = 0; i < patients.size(); i++) {
            jsonRes[i]["patientID"] = patients[i].patientID;
            jsonRes[i]["name"] = patients[i].name;
            jsonRes[i]["age"] = patients[i].age;
            jsonRes[i]["gender"] = patients[i].gender;
            jsonRes[i]["bloodGroup"] = patients[i].bloodGroup;
            jsonRes[i]["department"] = patients[i].department;
            jsonRes[i]["doctor"] = patients[i].doctor;
            jsonRes[i]["diagnosis"] = patients[i].diagnosis;
            jsonRes[i]["admissionDate"] = patients[i].admissionDate;
            jsonRes[i]["dischargeDate"] = patients[i].dischargeDate;
        }
        res.code = 200;
        res.write(jsonRes.dump());
        res.end();
    });

CROW_ROUTE(app, "/api/patient/me").methods("GET"_method)
    ([](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        vector<Patient> patients = loadPatients();

        crow::json::wvalue jsonRes;
        jsonRes["email"] = current_user_email;
        jsonRes["role"] = current_user_role;
        jsonRes["found"] = false;

        // Match logged-in user against patients file (by name/email prefix or first record if testing)
        std::string emailPrefix = current_user_email.substr(0, current_user_email.find('@'));
        const Patient* matched = nullptr;

        for (const auto& p : patients) {
            std::string lowerName = p.name;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
            std::string lowerPrefix = emailPrefix;
            std::transform(lowerPrefix.begin(), lowerPrefix.end(), lowerPrefix.begin(), ::tolower);

            if (!lowerPrefix.empty() && lowerName.find(lowerPrefix) != std::string::npos) {
                matched = &p;
                break;
            }
        }

        // If no exact name match yet, use the first patient record if available
        if (!matched && !patients.empty()) {
            matched = &patients[0];
        }

        if (matched) {
            jsonRes["found"] = true;
            jsonRes["patientID"] = matched->patientID;
            jsonRes["name"] = matched->name;
            jsonRes["age"] = matched->age;
            jsonRes["gender"] = matched->gender;
            jsonRes["bloodGroup"] = matched->bloodGroup;
            jsonRes["department"] = matched->department;
            jsonRes["doctor"] = matched->doctor;
            jsonRes["diagnosis"] = matched->diagnosis;
            jsonRes["admissionDate"] = matched->admissionDate;
            jsonRes["dischargeDate"] = matched->dischargeDate;
            jsonRes["followUpDate"] = matched->followUpDate;
            jsonRes["instructions"] = matched->instructions;

            crow::json::wvalue medsList = crow::json::wvalue::list();
            for (size_t i = 0; i < matched->medications.size(); i++) {
                medsList[i]["name"] = matched->medications[i].name;
                medsList[i]["dosage"] = matched->medications[i].dosage;
                medsList[i]["frequency"] = matched->medications[i].frequency;
                medsList[i]["duration"] = matched->medications[i].duration;
            }
            jsonRes["medications"] = std::move(medsList);

            crow::json::wvalue testsList = crow::json::wvalue::list();
            for (size_t i = 0; i < matched->tests.size(); i++) {
                testsList[i]["name"] = matched->tests[i].name;
                testsList[i]["date"] = matched->tests[i].date;
                testsList[i]["result"] = matched->tests[i].result;
            }
            jsonRes["tests"] = std::move(testsList);
        }

        res.code = 200;
        res.write(jsonRes.dump());
        res.end();
    });
    
CROW_ROUTE(app, "/api/appointments/book").methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        auto x = crow::json::load(req.body);
        res.set_header("Content-Type", "application/json");
        if (!x) {
            res.code = 400;
            res.write("{\"message\": \"Invalid JSON\"}");
            res.end();
            return;
        }

        std::string doctor = x["doctor"].s();
        std::string dept = x["department"].s();
        std::string date = x["date"].s();
        std::string time = x["time"].s();
        std::string reason = x["reason"].s();

        std::ofstream file("txt-files/appointments.txt", std::ios::app);
        if (!file.is_open()) file.open("appointments.txt", std::ios::app);
        file << current_user_email << "|" << doctor << "|" << dept << "|" << date << "|" << time << "|" << reason << "\n";
        file.close();

        res.code = 200;
        res.write("{\"message\": \"Appointment scheduled!\"}");
        res.end();
    });

// Fetch all booked appointments from appointments.txt
    CROW_ROUTE(app, "/api/appointments/list").methods("GET"_method)
    ([](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        crow::json::wvalue jsonRes = crow::json::wvalue::list();

        std::ifstream file("txt-files/appointments.txt");
        if (!file.is_open()) file.open("appointments.txt");

        std::string line;
        int idx = 0;
        while (getline(file, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string patientEmail, doctor, dept, date, time, reason;
            if (getline(ss, patientEmail, '|') && getline(ss, doctor, '|') &&
                getline(ss, dept, '|') && getline(ss, date, '|') &&
                getline(ss, time, '|') && getline(ss, reason)) {
                jsonRes[idx]["patientEmail"] = patientEmail;
                jsonRes[idx]["doctor"] = doctor;
                jsonRes[idx]["department"] = dept;
                jsonRes[idx]["date"] = date;
                jsonRes[idx]["time"] = time;
                jsonRes[idx]["reason"] = reason;
                idx++;
            }
        }
        file.close();

        res.code = 200;
        res.write(jsonRes.dump());
        res.end();
    });

    // Doctor action: Update patient diagnosis/instructions and add prescription to patients.txt
    CROW_ROUTE(app, "/api/doctor/update-patient").methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        auto x = crow::json::load(req.body);
        res.set_header("Content-Type", "application/json");
        if (!x || !x.has("patientID")) {
            res.code = 400;
            res.write("{\"message\": \"Invalid request payload\"}");
            res.end();
            return;
        }

        int targetId = x["patientID"].i();
        std::string diagnosis = x.has("diagnosis") ? std::string(x["diagnosis"].s()) : "";
        std::string followUp = x.has("followUpDate") ? std::string(x["followUpDate"].s()) : "";
        std::string instructions = x.has("instructions") ? std::string(x["instructions"].s()) : "";
        std::string medName = x.has("medName") ? std::string(x["medName"].s()) : "";
        std::string medDosage = x.has("medDosage") ? std::string(x["medDosage"].s()) : "";
        std::string medFreq = x.has("medFreq") ? std::string(x["medFreq"].s()) : "";
        std::string medDur = x.has("medDur") ? std::string(x["medDur"].s()) : "";

        vector<Patient> patients = loadPatients();
        int idx = findPatientIndex(patients, targetId);
        if (idx == -1) {
            res.code = 404;
            res.write("{\"message\": \"Patient ID not found in patients.txt\"}");
            res.end();
            return;
        }

        if (!diagnosis.empty()) patients[idx].diagnosis = sanitize(diagnosis);
        if (!followUp.empty()) patients[idx].followUpDate = sanitize(followUp);
        if (!instructions.empty()) patients[idx].instructions = sanitize(instructions);

        if (!medName.empty()) {
            Medication m;
            m.name = sanitize(medName);
            m.dosage = medDosage.empty() ? "As directed" : sanitize(medDosage);
            m.frequency = medFreq.empty() ? "Once daily" : sanitize(medFreq);
            m.duration = medDur.empty() ? "5 days" : sanitize(medDur);
            patients[idx].medications.push_back(m);
        }

        savePatients(patients);

        res.code = 200;
        res.write("{\"message\": \"Patient chart and prescription saved!\"}");
        res.end();
    });

    // 1. List all beds from beds.txt
    CROW_ROUTE(app, "/api/beds/list").methods("GET"_method)
    ([](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        crow::json::wvalue jsonRes = crow::json::wvalue::list();

        std::ifstream bedFile("txt-files/beds.txt");
        if (!bedFile.is_open()) bedFile.open("beds.txt");

        std::string line;
        int idx = 0;
        while (getline(bedFile, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string sId, dept, sType, sStatus, pid;
            if (getline(ss, sId, '|') && getline(ss, dept, '|') &&
                getline(ss, sType, '|') && getline(ss, sStatus, '|') && getline(ss, pid)) {
                jsonRes[idx]["id"] = sId;
                jsonRes[idx]["department"] = dept;
                jsonRes[idx]["type"] = std::stoi(sType);
                jsonRes[idx]["status"] = std::stoi(sStatus);
                jsonRes[idx]["patientId"] = pid;
                idx++;
            }
        }
        bedFile.close();

        res.code = 200;
        res.write(jsonRes.dump());
        res.end();
    });

    // 2. Assign or release a bed in beds.txt
    CROW_ROUTE(app, "/api/beds/update").methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        auto x = crow::json::load(req.body);
        res.set_header("Content-Type", "application/json");
        if (!x || !x.has("id") || !x.has("status")) {
            res.code = 400;
            res.write("{\"message\": \"Invalid request\"}");
            res.end();
            return;
        }

        std::string targetId = x["id"].s();
        int newStatus = x["status"].i();
        std::string patientId = x.has("patientId") ? std::string(x["patientId"].s()) : "None";
        if (newStatus != 2 || patientId.empty()) patientId = "None";

        std::ifstream inFile("txt-files/beds.txt");
        if (!inFile.is_open()) inFile.open("beds.txt");

        std::vector<std::string> updatedLines;
        std::string line;
        bool found = false;

        while (getline(inFile, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string sId, dept, sType, sStatus, pid;
            if (getline(ss, sId, '|') && getline(ss, dept, '|') &&
                getline(ss, sType, '|') && getline(ss, sStatus, '|') && getline(ss, pid)) {
                if (sId == targetId) {
                    updatedLines.push_back(sId + "|" + dept + "|" + sType + "|" + std::to_string(newStatus) + "|" + patientId);
                    found = true;
                } else {
                    updatedLines.push_back(line);
                }
            }
        }
        inFile.close();

        if (!found) {
            res.code = 404;
            res.write("{\"message\": \"Bed ID not found\"}");
            res.end();
            return;
        }

        std::ofstream outFile("txt-files/beds.txt");
        if (!outFile.is_open()) outFile.open("beds.txt");
        for (const auto& l : updatedLines) {
            outFile << l << "\n";
        }
        outFile.close();

        res.code = 200;
        res.write("{\"message\": \"Bed status updated in beds.txt!\"}");
        res.end();
    });

    // Add a new bed into beds.txt
    CROW_ROUTE(app, "/api/beds/add").methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        auto x = crow::json::load(req.body);
        res.set_header("Content-Type", "application/json");
        if (!x || !x.has("id") || !x.has("department")) {
            res.code = 400;
            res.write("{\"message\": \"Invalid bed details\"}");
            res.end();
            return;
        }

        std::string bedId = sanitize(x["id"].s());
        std::string dept = sanitize(x["department"].s());
        std::string type = x.has("type") ? std::string(x["type"].s()) : "1";

        std::ofstream bedFile("txt-files/beds.txt", std::ios::app);
        if (!bedFile.is_open()) bedFile.open("beds.txt", std::ios::app);
        bedFile << bedId << "|" << dept << "|" << type << "|1|None\n";
        bedFile.close();

        res.code = 200;
        res.write("{\"message\": \"Bed #" + bedId + " added to beds.txt!\"}");
        res.end();
    });

    // 3. Admit / Register a new patient into patients.txt
    CROW_ROUTE(app, "/api/staff/admit-patient").methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        auto x = crow::json::load(req.body);
        res.set_header("Content-Type", "application/json");
        if (!x || !x.has("name")) {
            res.code = 400;
            res.write("{\"message\": \"Missing patient details\"}");
            res.end();
            return;
        }

        int newId = 1001 + static_cast<int>(loadPatients().size());
        std::string name = sanitize(x["name"].s());
        std::string age = x.has("age") ? std::string(x["age"].s()) : "30";
        std::string gender = x.has("gender") ? sanitize(x["gender"].s()) : "Other";
        std::string blood = x.has("bloodGroup") ? sanitize(x["bloodGroup"].s()) : "O+";
        std::string phone = x.has("phone") ? sanitize(x["phone"].s()) : "9999999999";
        std::string dept = x.has("department") ? sanitize(x["department"].s()) : "General";
        std::string doctor = x.has("doctor") ? sanitize(x["doctor"].s()) : "On-Duty Doctor";
        std::string diagnosis = x.has("diagnosis") ? sanitize(x["diagnosis"].s()) : "Under Observation";
        std::string admDate = x.has("admissionDate") ? sanitize(x["admissionDate"].s()) : "12/11/2026";

        std::ofstream file("txt-files/patients.txt", std::ios::app);
        if (!file.is_open()) file.open("patients.txt", std::ios::app);

        file << newId << "|" << name << "|" << age << "|" << gender << "|" << blood << "|"
             << phone << "|Dehradun|" << phone << "|" << admDate << "||"
             << dept << "|" << doctor << "|" << diagnosis
             << "|Stable|None|None|None|None|None|Stable||Follow instructions|MEDS||TESTS|\n";
        file.close();

        res.code = 200;
        res.write("{\"message\": \"Patient admitted with ID #AG-" + std::to_string(newId) + "!\"}");
        res.end();
    });

    // 4. Log a new emergency triage case into waitlist.txt
    CROW_ROUTE(app, "/api/staff/log-triage").methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        auto x = crow::json::load(req.body);
        res.set_header("Content-Type", "application/json");
        if (!x || !x.has("patientId") || !x.has("department")) {
            res.code = 400;
            res.write("{\"message\": \"Invalid triage payload\"}");
            res.end();
            return;
        }

        std::string pid = sanitize(x["patientId"].s());
        std::string dept = sanitize(x["department"].s());
        std::string reqBed = x.has("required") ? std::string(x["required"].s()) : "1";
        std::string priority = x.has("priority") ? std::string(x["priority"].s()) : "2";

        std::ofstream waitFile("txt-files/waitlist.txt", std::ios::app);
        if (!waitFile.is_open()) waitFile.open("waitlist.txt", std::ios::app);
        waitFile << pid << "|" << dept << "|" << reqBed << "|" << priority << "\n";
        waitFile.close();

        res.code = 200;
        res.write("{\"message\": \"Emergency case added to waitlist.txt!\"}");
        res.end();
    });

    CROW_ROUTE(app, "/api/beds/status").methods("GET"_method)
    ([](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");

        int totalBeds = 0;
        int availableBeds = 0;
        int occupiedBeds = 0;
        int maintenanceBeds = 0;

        std::ifstream bedFile("txt-files/beds.txt");
        if (!bedFile.is_open()) bedFile.open("beds.txt");
        std::string line;
        while (getline(bedFile, line)) {
            std::stringstream ss(line);
            std::string sId, dept, sType, sStatus, pid;
            if (getline(ss, sId, '|') && getline(ss, dept, '|') &&
                getline(ss, sType, '|') && getline(ss, sStatus, '|') && getline(ss, pid, '|')) {
                totalBeds++;
                int status = std::stoi(sStatus);
                if (status == 1) availableBeds++;
                else if (status == 2) occupiedBeds++;
                else if (status == 3) maintenanceBeds++;
            }
        }
        bedFile.close();

        int waitCount = 0;
        std::ifstream waitFile("txt-files/waitlist.txt");
        if (!waitFile.is_open()) waitFile.open("waitlist.txt");
        while (getline(waitFile, line)) {
            if (!line.empty()) waitCount++;
        }
        waitFile.close();

        crow::json::wvalue jsonRes;
        jsonRes["total_beds"] = totalBeds;
        jsonRes["available_beds"] = availableBeds;
        jsonRes["occupied_beds"] = occupiedBeds;
        jsonRes["maintenance_beds"] = maintenanceBeds;
        jsonRes["waiting_count"] = waitCount;

        res.code = 200;
        res.write(jsonRes.dump());
        res.end();
    });

    CROW_ROUTE(app, "/api/triage/list").methods("GET"_method)
    ([](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        crow::json::wvalue jsonRes = crow::json::wvalue::list();

        std::ifstream waitFile("txt-files/waitlist.txt");
        if (!waitFile.is_open()) waitFile.open("waitlist.txt");

        std::string line;
        int idx = 0;
        while (getline(waitFile, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string pid, dept, sReq, sPri;
            if (getline(ss, pid, '|') && getline(ss, dept, '|') &&
                getline(ss, sReq, '|') && getline(ss, sPri)) {
                jsonRes[idx]["patientId"] = pid;
                jsonRes[idx]["department"] = dept;
                jsonRes[idx]["required"] = std::stoi(sReq);
                jsonRes[idx]["priority"] = std::stoi(sPri);
                idx++;
            }
        }
        waitFile.close();

        res.code = 200;
        res.write(jsonRes.dump());
        res.end();
    });

    // Admin endpoint to provision accounts with custom roles
    CROW_ROUTE(app, "/api/admin/create-user").methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        auto x = crow::json::load(req.body);
        res.set_header("Content-Type", "application/json");
        
        if (!x) {
            res.code = 400;
            res.write("{\"message\": \"Invalid JSON payload\"}");
            res.end();
            return;
        }
        
        std::string fullName = x["name"].s();
        std::string email = x["email"].s();
        std::string password = x["password"].s();
        std::string role = x["role"].s();

        std::string errorMsg;
        if (registerUserWeb(fullName, email, password, role, errorMsg)) {
            res.code = 200;
            res.write("{\"message\": \"Account created successfully!\"}");
            res.end();
        } else {
            res.code = 400;
            res.write("{\"message\": \"" + errorMsg + "\"}");
            res.end();
        }
    });

    // Start server locally on port 18080
    app.port(18080).multithreaded().run();
    return 0;
}