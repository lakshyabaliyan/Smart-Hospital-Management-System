#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "crow_all.h"

// Track current logged-in user session globally for the web app
std::string current_user_email = "user@arogya.com";
std::string current_user_role = "Patient";

// Declare backend functions from user.cpp
extern void initialize();
extern void loadUsers();
extern bool registerUserWeb(const std::string& fullName, const std::string& email, const std::string& password, const std::string& roleStr, std::string& errorMsg);
extern bool loginUserWeb(const std::string& username, const std::string& password, std::string& outRole, std::string& errorMsg);

// Helper function to read standalone HTML/CSS/JS files if needed
std::string read_file(const std::string& filepath) {
    std::ifstream file(filepath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    crow::SimpleApp app;
    
    // Initialize user hash table and load existing users from users.txt
    initialize();
    loadUsers();
    
    crow::mustache::set_global_base("templates");

    // ==========================================
    // 1. STATIC FILE ROUTE (CSS / JS / Images)
    // ==========================================
    CROW_ROUTE(app, "/static/<string>")
    ([](const crow::request& req, crow::response& res, std::string filename) {
        res.set_static_file_info("static/" + filename);
        res.end();
    });

    // Dedicated static routes for direct file assets if requested
    CROW_ROUTE(app, "/admin-dashboard.css")
    ([](){
        crow::response res;
        res.body = read_file("admin-dashboard.css");
        res.set_header("Content-Type", "text/css");
        return res;
    });

    CROW_ROUTE(app, "/admin-dashboard.js")
    ([](){
        crow::response res;
        res.body = read_file("admin-dashboard.js");
        res.set_header("Content-Type", "application/javascript");
        return res;
    });

    // ==========================================
    // 2. WEBPAGE ROUTES
    // ==========================================

    // Welcome Page
    CROW_ROUTE(app, "/")([](){
        return crow::mustache::load("index.html").render();
    });

    // Sign-in Page (supports both /signin and /signin.html)
    CROW_ROUTE(app, "/signin")([](){
        return crow::mustache::load("signin.html").render();
    });

    CROW_ROUTE(app, "/signin.html")([](){
        return crow::mustache::load("signin.html").render();
    });

    // Patient Dashboard Page
    CROW_ROUTE(app, "/patient-dashboard.html")([](){
        return crow::mustache::load("patient-dashboard.html").render();
    });

    // Appointments Page
    CROW_ROUTE(app, "/appointments.html")([](){
        return crow::mustache::load("appointments.html").render();
    });

    // Queue Status Page
    CROW_ROUTE(app, "/queue-status.html")([](){
        return crow::mustache::load("queue-status.html").render();
    });

    // Medical Records Page
    CROW_ROUTE(app, "/medical-records.html")([](){
        return crow::mustache::load("medical-records.html").render();
    });

    // Billing Page
    CROW_ROUTE(app, "/billing.html")([](){
        return crow::mustache::load("billing.html").render();
    });

    // --- Additional Feature & Role Dashboard Pages ---

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

    CROW_ROUTE(app, "/staff-dashboard.html")([](){
        return crow::mustache::load("staff-dashboard.html").render();
    });

    // --- Standalone Admin UI Pages via raw file loader ---
    CROW_ROUTE(app, "/staff.html")
    ([](){
        crow::response res;
        res.body = read_file("staff.html");
        res.set_header("Content-Type", "text/html");
        return res;
    });

    CROW_ROUTE(app, "/patients.html")
    ([](){
        crow::response res;
        res.body = read_file("patients.html");
        res.set_header("Content-Type", "text/html");
        return res;
    });

    CROW_ROUTE(app, "/triage.html")
    ([](){
        crow::response res;
        res.body = read_file("triage.html");
        res.set_header("Content-Type", "text/html");
        return res;
    });

    // ==========================================
    // 3. AUTHENTICATION & SESSION APIs
    // ==========================================

    // Register Form Submission Handler
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

    // Login Form Submission Handler
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

    // Session Check Route
    CROW_ROUTE(app, "/api/session").methods("GET"_method)
    ([](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        res.code = 200;
        res.write("{\"role\": \""+ current_user_role + "\", \"email\": \"" + current_user_email + "\"}");
        res.end();
    });

    // Logout Endpoint
    CROW_ROUTE(app, "/api/logout").methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");
        current_user_email = ""; // clear on logout
        current_user_role = "guest";
        res.code = 200;
        res.write("{\"message\": \"Logged out successfully\"}");
        res.end();
    });

    // ==========================================
    // 4. BACKEND MODULE API HOOKS
    // ==========================================

    // Staff List Placeholder
    CROW_ROUTE(app, "/api/staff/list")([](){
        return crow::response(200, "Doctor & Staff records loaded successfully.");
    });

    // Bed & Ward Management Status API (Reads beds.txt & waitlist.txt)
    CROW_ROUTE(app, "/api/beds/status").methods("GET"_method)
    ([](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "application/json");

        int totalBeds = 0;
        int availableBeds = 0;
        int occupiedBeds = 0;
        int maintenanceBeds = 0;

        // Read from beds.txt generated by bed_ward_management.cpp
        std::ifstream bedFile("beds.txt");
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

        // Count waiting list from waitlist.txt
        int waitCount = 0;
        std::ifstream waitFile("waitlist.txt");
        while (getline(waitFile, line)) {
            if (!line.empty()) waitCount++;
        }
        waitFile.close();

        // Construct JSON response
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

    // Admin Endpoint to Provision Staff/Doctor Accounts
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
        std::string role = x["role"].s(); // e.g., "Doctor" or "Staff"

        std::string errorMsg;
        if (registerUserWeb(fullName, email, password, role, errorMsg)) {
            res.code = 200;
            res.write("{\"message\": \"Staff/Doctor account created successfully!\"}");
            res.end();
        } else {
            res.code = 400;
            res.write("{\"message\": \"" + errorMsg + "\"}");
            res.end();
        }
    });

    // Start server on port 18080
    app.port(18080).multithreaded().run();
    return 0;
}