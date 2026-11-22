#ifndef WAITING_QUEUE_CPP
#define WAITING_QUEUE_CPP

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace WaitingQueueModule {

struct QueueEntry {
    int tokenNumber = 0;          // 1-based position in current file/queue
    std::string patientId;        // e.g., "1001", "AG-1001", or patient ref
    std::string department;       // e.g., "General", "Emergency", "Cardiology"
    int requiredBedType = 1;      // 1 = General, 2 = Semi-Private, 3 = ICU
    int priority = 2;             // 1 = Critical/Urgent, 2 = Moderate, 3 = Routine
    std::string status = "Waiting"; // Waiting, Called, Assigned
};

struct QueueStats {
    int totalWaiting = 0;
    int highPriorityCount = 0;    // priority == 1
    int mediumPriorityCount = 0;  // priority == 2
    int lowPriorityCount = 0;     // priority >= 3
    int generalBedReq = 0;        // requiredBedType == 1
    int icuBedReq = 0;            // requiredBedType >= 2
};

// Trim leading and trailing whitespace
inline std::string trim(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        start++;
    }
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
        end--;
    }
    return s.substr(start, end - start);
}

// Lowercase helper for case-insensitive matching
inline std::string toLower(const std::string& s) {
    std::string out = s;
    for (char& ch : out) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return out;
}

// Strip non-digit characters so "AG-1001" and "1001" compare cleanly
inline std::string digitsOnly(const std::string& s) {
    std::string out;
    for (char ch : s) {
        if (std::isdigit(static_cast<unsigned char>(ch))) {
            out.push_back(ch);
        }
    }
    return out;
}

// Prevent pipe delimiter or newline corruption in flat-file storage
inline std::string sanitizeField(const std::string& input) {
    std::string clean = trim(input);
    for (char& ch : clean) {
        if (ch == '|' || ch == '\r' || ch == '\n') {
            ch = ' ';
        }
    }
    return clean;
}

// Normalize and validate required bed type (1..3)
inline int normalizeBedType(int bedType) {
    if (bedType < 1 || bedType > 3) {
        return 1;
    }
    return bedType;
}

// Normalize and validate priority level (1..3; 1 = highest priority)
inline int normalizePriority(int priority) {
    if (priority < 1) return 1;
    if (priority > 3) return 3;
    return priority;
}

// Human-readable labels
inline std::string bedTypeLabel(int bedType) {
    switch (normalizeBedType(bedType)) {
        case 1:  return "General Ward";
        case 2:  return "Semi-Private";
        case 3:  return "ICU";
        default: return "General Ward";
    }
}

inline std::string priorityLabel(int priority) {
    switch (normalizePriority(priority)) {
        case 1:  return "High (Critical)";
        case 2:  return "Medium (Urgent)";
        case 3:  return "Low (Routine)";
        default: return "Medium (Urgent)";
    }
}

// Resolve waitlist.txt path (matches crow-project layout and local working directory)
inline std::string resolveWaitlistPath() {
    std::ifstream primary("txt-files/waitlist.txt");
    if (primary.is_open()) {
        primary.close();
        return "txt-files/waitlist.txt";
    }
    std::ifstream alt("crow-project/txt-files/waitlist.txt");
    if (alt.is_open()) {
        alt.close();
        return "crow-project/txt-files/waitlist.txt";
    }
    return "txt-files/waitlist.txt";
}

inline std::string resolveWritePath() {
    std::ofstream test("txt-files/waitlist.txt", std::ios::app);
    if (test.is_open()) {
        test.close();
        return "txt-files/waitlist.txt";
    }
    return "waitlist.txt";
}

// Check whether two patient ID strings refer to the same patient
inline bool samePatientId(const std::string& a, const std::string& b) {
    std::string la = toLower(trim(a));
    std::string lb = toLower(trim(b));
    if (!la.empty() && la == lb) {
        return true;
    }
    std::string da = digitsOnly(a);
    std::string db = digitsOnly(b);
    return (!da.empty() && da == db);
}

// Load all entries from waitlist.txt in arrival order
inline std::vector<QueueEntry> loadQueue() {
    std::vector<QueueEntry> queue;
    std::ifstream in(resolveWaitlistPath());
    if (!in.is_open()) {
        in.open("waitlist.txt");
    }
    if (!in.is_open()) {
        return queue;
    }

    std::string line;
    int token = 1;
    while (std::getline(in, line)) {
        std::string cleanLine = trim(line);
        if (cleanLine.empty()) {
            continue;
        }

        std::stringstream ss(cleanLine);
        std::string pid, dept, sReq, sPri;
        if (std::getline(ss, pid, '|') &&
            std::getline(ss, dept, '|') &&
            std::getline(ss, sReq, '|') &&
            std::getline(ss, sPri)) {

            QueueEntry entry;
            entry.tokenNumber = token++;
            entry.patientId = sanitizeField(pid);
            entry.department = sanitizeField(dept);
            if (entry.department.empty()) {
                entry.department = "General";
            }
            entry.requiredBedType = normalizeBedType(std::atoi(trim(sReq).c_str()));
            entry.priority = normalizePriority(std::atoi(trim(sPri).c_str()));
            entry.status = "Waiting";

            if (!entry.patientId.empty()) {
                queue.push_back(entry);
            }
        }
    }

    in.close();
    return queue;
}

// Rewrite entire queue to waitlist.txt
inline bool saveQueue(const std::vector<QueueEntry>& entries) {
    std::string path = resolveWritePath();
    std::ofstream out(path, std::ios::trunc);
    if (!out.is_open()) {
        out.open("waitlist.txt", std::ios::trunc);
    }
    if (!out.is_open()) {
        return false;
    }

    for (const auto& e : entries) {
        std::string pid = sanitizeField(e.patientId);
        std::string dept = sanitizeField(e.department);
        if (pid.empty()) continue;
        if (dept.empty()) dept = "General";

        out << pid << "|"
            << dept << "|"
            << normalizeBedType(e.requiredBedType) << "|"
            << normalizePriority(e.priority) << "\n";
    }

    out.close();
    return true;
}

// Append a new patient to the waiting queue
inline bool enqueuePatient(const std::string& patientId,
                           const std::string& department,
                           int requiredBedType = 1,
                           int priority = 2,
                           std::string* outMessage = nullptr) {
    std::string pid = sanitizeField(patientId);
    std::string dept = sanitizeField(department);
    int bedType = normalizeBedType(requiredBedType);
    int prio = normalizePriority(priority);

    if (pid.empty()) {
        if (outMessage) *outMessage = "Patient ID cannot be empty.";
        return false;
    }
    if (dept.empty()) {
        dept = "General";
    }

    std::string path = resolveWritePath();
    std::ofstream out(path, std::ios::app);
    if (!out.is_open()) {
        out.open("waitlist.txt", std::ios::app);
    }
    if (!out.is_open()) {
        if (outMessage) *outMessage = "Unable to open waitlist file for writing.";
        return false;
    }

    out << pid << "|" << dept << "|" << bedType << "|" << prio << "\n";
    out.close();

    if (outMessage) {
        *outMessage = "Patient added to waiting queue.";
    }
    return true;
}

// Return queue ordered by priority (1 -> 2 -> 3), preserving arrival order within same priority
inline std::vector<QueueEntry> getPriorityOrderedQueue() {
    std::vector<QueueEntry> entries = loadQueue();
    std::stable_sort(entries.begin(), entries.end(),
                     [](const QueueEntry& a, const QueueEntry& b) {
                         if (a.priority != b.priority) {
                             return a.priority < b.priority;
                         }
                         return a.tokenNumber < b.tokenNumber;
                     });

    for (size_t i = 0; i < entries.size(); i++) {
        entries[i].tokenNumber = static_cast<int>(i + 1);
    }
    return entries;
}

// Filter waiting queue by department (case-insensitive)
inline std::vector<QueueEntry> getQueueByDepartment(const std::string& department) {
    std::string target = toLower(trim(department));
    std::vector<QueueEntry> ordered = getPriorityOrderedQueue();
    if (target.empty() || target == "all") {
        return ordered;
    }

    std::vector<QueueEntry> result;
    for (const auto& e : ordered) {
        if (toLower(e.department) == target || toLower(e.department).find(target) != std::string::npos) {
            result.push_back(e);
        }
    }
    return result;
}

// Find all entries matching a given patient ID
inline std::vector<QueueEntry> findEntriesByPatient(const std::string& patientId) {
    std::vector<QueueEntry> ordered = getPriorityOrderedQueue();
    std::vector<QueueEntry> matches;
    for (const auto& e : ordered) {
        if (samePatientId(e.patientId, patientId)) {
            matches.push_back(e);
        }
    }
    return matches;
}

// Peek next patient to be served (optionally filtered by department)
inline bool peekNextPatient(QueueEntry& outEntry, const std::string& department = "") {
    std::vector<QueueEntry> list = department.empty()
        ? getPriorityOrderedQueue()
        : getQueueByDepartment(department);

    if (list.empty()) {
        return false;
    }
    outEntry = list.front();
    return true;
}

// Dequeue (call/remove) the next patient in priority order and persist updated waitlist.txt
inline bool dequeueNextPatient(QueueEntry& removedEntry,
                               const std::string& department = "",
                               std::string* outMessage = nullptr) {
    std::vector<QueueEntry> entries = loadQueue();
    if (entries.empty()) {
        if (outMessage) *outMessage = "Waiting queue is empty.";
        return false;
    }

    std::string deptFilter = toLower(trim(department));
    int bestIndex = -1;
    int bestPriority = 999;
    int bestToken = 999999;

    for (size_t i = 0; i < entries.size(); i++) {
        bool deptOk = deptFilter.empty() || deptFilter == "all" ||
                      toLower(entries[i].department) == deptFilter ||
                      toLower(entries[i].department).find(deptFilter) != std::string::npos;
        if (!deptOk) continue;

        if (entries[i].priority < bestPriority ||
            (entries[i].priority == bestPriority && entries[i].tokenNumber < bestToken)) {
            bestPriority = entries[i].priority;
            bestToken = entries[i].tokenNumber;
            bestIndex = static_cast<int>(i);
        }
    }

    if (bestIndex == -1) {
        if (outMessage) *outMessage = "No matching patient found in queue.";
        return false;
    }

    removedEntry = entries[bestIndex];
    entries.erase(entries.begin() + bestIndex);

    if (!saveQueue(entries)) {
        if (outMessage) *outMessage = "Failed to save updated waiting queue.";
        return false;
    }

    if (outMessage) {
        *outMessage = "Dequeued patient " + removedEntry.patientId + ".";
    }
    return true;
}

// Remove a specific patient from the waiting queue by ID
inline bool removePatientFromQueue(const std::string& patientId,
                                   std::string* outMessage = nullptr) {
    if (trim(patientId).empty()) {
        if (outMessage) *outMessage = "Patient ID is required.";
        return false;
    }

    std::vector<QueueEntry> entries = loadQueue();
    size_t before = entries.size();

    entries.erase(
        std::remove_if(entries.begin(), entries.end(),
                       [&](const QueueEntry& e) { return samePatientId(e.patientId, patientId); }),
        entries.end()
    );

    if (entries.size() == before) {
        if (outMessage) *outMessage = "Patient not found in waiting queue.";
        return false;
    }

    if (!saveQueue(entries)) {
        if (outMessage) *outMessage = "Failed to update waitlist file.";
        return false;
    }

    if (outMessage) {
        *outMessage = "Removed patient from waiting queue.";
    }
    return true;
}

// Update priority and/or required bed type for an existing patient in the queue
inline bool updatePatientQueueDetails(const std::string& patientId,
                                      int newPriority,
                                      int newBedType,
                                      std::string* outMessage = nullptr) {
    if (trim(patientId).empty()) {
        if (outMessage) *outMessage = "Patient ID is required.";
        return false;
    }

    std::vector<QueueEntry> entries = loadQueue();
    bool found = false;

    for (auto& e : entries) {
        if (samePatientId(e.patientId, patientId)) {
            e.priority = normalizePriority(newPriority);
            e.requiredBedType = normalizeBedType(newBedType);
            found = true;
            break;
        }
    }

    if (!found) {
        if (outMessage) *outMessage = "Patient not found in waiting queue.";
        return false;
    }

    if (!saveQueue(entries)) {
        if (outMessage) *outMessage = "Failed to save updated queue entry.";
        return false;
    }

    if (outMessage) {
        *outMessage = "Queue entry updated.";
    }
    return true;
}

// Clear all waiting entries
inline bool clearWaitingQueue(std::string* outMessage = nullptr) {
    std::vector<QueueEntry> emptyList;
    if (!saveQueue(emptyList)) {
        if (outMessage) *outMessage = "Failed to clear waiting queue.";
        return false;
    }
    if (outMessage) {
        *outMessage = "Waiting queue cleared.";
    }
    return true;
}

// Compute summary statistics for dashboards and reports
inline QueueStats computeQueueStats() {
    std::vector<QueueEntry> entries = loadQueue();
    QueueStats stats;
    stats.totalWaiting = static_cast<int>(entries.size());

    for (const auto& e : entries) {
        int prio = normalizePriority(e.priority);
        int bed = normalizeBedType(e.requiredBedType);

        if (prio == 1) stats.highPriorityCount++;
        else if (prio == 2) stats.mediumPriorityCount++;
        else stats.lowPriorityCount++;

        if (bed == 1) stats.generalBedReq++;
        else stats.icuBedReq++;
    }

    return stats;
}

inline int getQueueCount() {
    return static_cast<int>(loadQueue().size());
}

// Pretty-print waiting queue in console
inline void displayQueueTable(bool prioritySorted = true) {
    std::vector<QueueEntry> list = prioritySorted ? getPriorityOrderedQueue() : loadQueue();
    if (list.empty()) {
        std::cout << "\n[Waiting Queue is currently empty]\n";
        return;
    }

    std::cout << "\n====================================================================\n";
    std::cout << std::left
              << std::setw(6)  << "Pos"
              << std::setw(14) << "Patient ID"
              << std::setw(18) << "Department"
              << std::setw(15) << "Bed Type"
              << std::setw(15) << "Priority" << "\n";
    std::cout << "====================================================================\n";

    for (size_t i = 0; i < list.size(); i++) {
        std::cout << std::left
                  << std::setw(6)  << (i + 1)
                  << std::setw(14) << list[i].patientId
                  << std::setw(18) << list[i].department
                  << std::setw(15) << bedTypeLabel(list[i].requiredBedType)
                  << std::setw(15) << priorityLabel(list[i].priority) << "\n";
    }
    std::cout << "====================================================================\n";
}

inline void displayQueueStatsConsole() {
    QueueStats s = computeQueueStats();
    std::cout << "\n--- Waiting Queue Summary ---\n"
              << "Total Waiting      : " << s.totalWaiting << "\n"
              << "High Priority (1)  : " << s.highPriorityCount << "\n"
              << "Medium Priority (2): " << s.mediumPriorityCount << "\n"
              << "Low Priority (3)   : " << s.lowPriorityCount << "\n"
              << "General Bed Req    : " << s.generalBedReq << "\n"
              << "ICU / Higher Req   : " << s.icuBedReq << "\n";
}

// Interactive console menu if compiled standalone for module testing
//if 0
inline void runWaitingQueueConsoleMenu() {
    int choice = -1;
    while (true) {
        std::cout << "\n=== AROGYA Waiting Queue Module ===\n"
                  << "1. Add Patient to Queue\n"
                  << "2. View Queue (Priority Sorted)\n"
                  << "3. View Queue (Arrival Order)\n"
                  << "4. Peek Next Patient\n"
                  << "5. Dequeue Next Patient\n"
                  << "6. Remove Patient by ID\n"
                  << "7. Update Priority / Bed Type\n"
                  << "8. Filter Queue by Department\n"
                  << "9. View Queue Statistics\n"
                  << "0. Exit\n"
                  << "Enter choice: ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input. Try again.\n";
            continue;
        }
        std::cin.ignore(10000, '\n');

        if (choice == 0) {
            break;
        } else if (choice == 1) {
            std::string pid, dept, msg;
            int bedType = 1, prio = 2;
            std::cout << "Patient ID: ";
            std::getline(std::cin, pid);
            std::cout << "Department: ";
            std::getline(std::cin, dept);
            std::cout << "Required Bed Type (1=General, 2=Semi-Private, 3=ICU): ";
            std::cin >> bedType;
            std::cout << "Priority (1=High, 2=Medium, 3=Low): ";
            std::cin >> prio;
            std::cin.ignore(10000, '\n');
            enqueuePatient(pid, dept, bedType, prio, &msg);
            std::cout << msg << "\n";
        } else if (choice == 2) {
            displayQueueTable(true);
        } else if (choice == 3) {
            displayQueueTable(false);
        } else if (choice == 4) {
            QueueEntry next;
            if (peekNextPatient(next)) {
                std::cout << "Next Patient: " << next.patientId
                          << " | Dept: " << next.department
                          << " | Bed: " << bedTypeLabel(next.requiredBedType)
                          << " | Priority: " << priorityLabel(next.priority) << "\n";
            } else {
                std::cout << "Queue is empty.\n";
            }
        } else if (choice == 5) {
            QueueEntry removed;
            std::string msg;
            if (dequeueNextPatient(removed, "", &msg)) {
                std::cout << msg << " (Dept: " << removed.department << ")\n";
            } else {
                std::cout << msg << "\n";
            }
        } else if (choice == 6) {
            std::string pid, msg;
            std::cout << "Enter Patient ID to remove: ";
            std::getline(std::cin, pid);
            removePatientFromQueue(pid, &msg);
            std::cout << msg << "\n";
        } else if (choice == 7) {
            std::string pid, msg;
            int prio = 2, bedType = 1;
            std::cout << "Enter Patient ID: ";
            std::getline(std::cin, pid);
            std::cout << "New Priority (1=High, 2=Medium, 3=Low): ";
            std::cin >> prio;
            std::cout << "New Bed Type (1=General, 2=Semi-Private, 3=ICU): ";
            std::cin >> bedType;
            std::cin.ignore(10000, '\n');
            updatePatientQueueDetails(pid, prio, bedType, &msg);
            std::cout << msg << "\n";
        } else if (choice == 8) {
            std::string dept;
            std::cout << "Enter Department: ";
            std::getline(std::cin, dept);
            std::vector<QueueEntry> filtered = getQueueByDepartment(dept);
            if (filtered.empty()) {
                std::cout << "No waiting patients for department: " << dept << "\n";
            } else {
                for (size_t i = 0; i < filtered.size(); i++) {
                    std::cout << (i + 1) << ". " << filtered[i].patientId
                              << " | " << filtered[i].department
                              << " | " << bedTypeLabel(filtered[i].requiredBedType)
                              << " | " << priorityLabel(filtered[i].priority) << "\n";
                }
            }
        } else if (choice == 9) {
            displayQueueStatsConsole();
        } else {
            std::cout << "Invalid choice.\n";
        }
    }
}

} 
#endif
