#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

#define SIZE 100
#define PORT 8080

struct User {
    string username;
    int password;
    string role;
};

struct Node {
    User data;
    Node* next;
};

Node* table[SIZE];

string urlDecode(string s) {
    string result;

    for (int i = 0; i < (int)s.length(); i++) {
        if (s[i] == '+') {
            result += ' ';
        } else if (s[i] == '%' && i + 2 < (int)s.length()) {
            string hex = s.substr(i + 1, 2);
            result += (char)strtol(hex.c_str(), NULL, 16);
            i += 2;
        } else {
            result += s[i];
        }
    }

    return result;
}

string getField(string data, string key) {
    string search = key + "=";
    size_t pos = data.find(search);

    if (pos == string::npos)
        return "";

    pos += search.length();

    size_t end = data.find('&', pos);

    if (end == string::npos)
        end = data.length();

    return urlDecode(data.substr(pos, end - pos));
}

string jsonEscape(string s) {
    string result;

    for (int i = 0; i < (int)s.length(); i++) {
        if (s[i] == '"' || s[i] == '\\')
            result += '\\';

        if (s[i] == '\n')
            result += "\\n";
        else if (s[i] == '\r')
            result += "\\r";
        else
            result += s[i];
    }

    return result;
}

int hashFunction(string username) {
    unsigned int hash = 0;

    for (int i = 0; i < (int)username.length(); i++)
        hash += (unsigned char)username[i];

    return hash % SIZE;
}

int hashPassword(string password) {
    unsigned int hash = 0;

    for (int i = 0; i < (int)password.length(); i++)
        hash = hash * 31 + (unsigned char)password[i];

    return hash & 0x7fffffff;
}

bool validPassword(string password) {
    if (password.length() < 6)
        return false;

    bool upper = false;
    bool lower = false;
    bool digit = false;
    bool special = false;

    for (int i = 0; i < (int)password.length(); i++) {
        unsigned char ch = password[i];

        if (isupper(ch))
            upper = true;
        else if (islower(ch))
            lower = true;
        else if (isdigit(ch))
            digit = true;
        else
            special = true;
    }

    return upper && lower && digit && special;
}

void initializeTable() {
    for (int i = 0; i < SIZE; i++)
        table[i] = NULL;
}

bool usernameExists(string username) {
    int index = hashFunction(username);
    Node* temp = table[index];

    while (temp != NULL) {
        if (temp->data.username == username)
            return true;

        temp = temp->next;
    }

    return false;
}

void insertUser(string username, int password, string role) {
    int index = hashFunction(username);

    Node* temp = new Node;

    temp->data.username = username;
    temp->data.password = password;
    temp->data.role = role;
    temp->next = table[index];

    table[index] = temp;
}

void saveUser(string username, int password, string role) {
    ofstream file("users.txt", ios::app);

    file << username << "|" << password << "|" << role << endl;

    file.close();
}

void loadUsers() {
    ifstream file("users.txt");

    string line;

    while (getline(file, line)) {
        size_t first = line.find('|');
        size_t second = line.find('|', first == string::npos ? 0 : first + 1);

        if (first == string::npos || second == string::npos)
            continue;

        string username = line.substr(0, first);
        string passText = line.substr(first + 1, second - first - 1);
        string role = line.substr(second + 1);

        if (username.empty() || usernameExists(username))
            continue;

        int password = atoi(passText.c_str());

        insertUser(username, password, role);
    }

    file.close();
}

string makeResponse(string status, string message, string role = "") {
    return "{\"status\":\"" + jsonEscape(status) +
           "\",\"message\":\"" + jsonEscape(message) +
           "\",\"role\":\"" + jsonEscape(role) + "\"}";
}

string contentType(string path) {
    if (path.find(".html") != string::npos)
        return "text/html; charset=utf-8";

    if (path.find(".css") != string::npos)
        return "text/css; charset=utf-8";

    if (path.find(".js") != string::npos)
        return "application/javascript";

    if (path.find(".png") != string::npos)
        return "image/png";

    if (path.find(".jpg") != string::npos ||
        path.find(".jpeg") != string::npos)
        return "image/jpeg";

    return "text/plain";
}

void sendResponse(SOCKET client, string body, string type = "application/json",
                  int status = 200) {
    string statusText = status == 200 ? "OK" :
                        status == 404 ? "Not Found" : "Bad Request";

    string response =
        "HTTP/1.1 " + to_string(status) + " " + statusText + "\r\n" +
        "Content-Type: " + type + "\r\n" +
        "Content-Length: " + to_string(body.length()) + "\r\n" +
        "Access-Control-Allow-Origin: *\r\n"
        "Connection: close\r\n\r\n" + body;

    send(client, response.c_str(), (int)response.length(), 0);
}

string readRequest(SOCKET client) {
    string request;
    char buffer[4096];

    int received;

    while ((received = recv(client, buffer, sizeof(buffer), 0)) > 0) {
        request.append(buffer, received);

        size_t headerEnd = request.find("\r\n\r\n");

        if (headerEnd != string::npos) {
            size_t pos = request.find("Content-Length:");

            if (pos == string::npos)
                pos = request.find("content-length:");

            int contentLength = 0;

            if (pos != string::npos) {
                pos += 15;

                while (pos < request.length() &&
                       (request[pos] == ' ' || request[pos] == '\t'))
                    pos++;

                contentLength = atoi(request.c_str() + pos);
            }

            size_t bodyStart = headerEnd + 4;

            if (request.length() >= bodyStart + contentLength)
                break;
        } else if (request.length() > 100000) {
            break;
        }
    }

    return request;
}

void handleClient(SOCKET client) {
    string request = readRequest(client);

    size_t firstSpace = request.find(' ');

    if (firstSpace == string::npos) {
        sendResponse(client, "Invalid request", "text/plain", 400);
        closesocket(client);
        return;
    }

    string method = request.substr(0, firstSpace);

    size_t secondSpace = request.find(' ', firstSpace + 1);

    string path = request.substr(firstSpace + 1,
                                 secondSpace - firstSpace - 1);

    size_t query = path.find('?');

    if (query != string::npos)
        path = path.substr(0, query);

    if (method == "POST") {
        size_t headerEnd = request.find("\r\n\r\n");

        string body = "";

        if (headerEnd != string::npos)
            body = request.substr(headerEnd + 4);

        if (path == "/register") {
            string username = getField(body, "username");
            string password = getField(body, "password");
            string role = getField(body, "role");

            if (username.empty() || password.empty() || role.empty()) {
                sendResponse(client, makeResponse("error", "Please fill all fields."));
            } else if (!validPassword(password)) {
                sendResponse(client, makeResponse(
                    "error",
                    "Password must have at least 6 characters, uppercase, lowercase, a digit and a special character."
                ));
            } else if (usernameExists(username)) {
                sendResponse(client, makeResponse("error", "Username already exists."));
            } else if (role != "Patient" && role != "Doctor" &&
                       role != "Nurse" && role != "Receptionist" &&
                       role != "Admin") {
                sendResponse(client, makeResponse("error", "Invalid role."));
            } else {
                int passHash = hashPassword(password);

                insertUser(username, passHash, role);
                saveUser(username, passHash, role);

                sendResponse(client, makeResponse(
                    "success", "Registration successful. You can now sign in.", role
                ));
            }
        } else if (path == "/login") {
            string username = getField(body, "username");
            string password = getField(body, "password");

            int index = hashFunction(username);
            Node* temp = table[index];

            bool found = false;

            while (temp != NULL) {
                if (temp->data.username == username &&
                    temp->data.password == hashPassword(password)) {
                    sendResponse(client, makeResponse(
                        "success", "Login successful.", temp->data.role
                    ));

                    found = true;
                    break;
                }

                temp = temp->next;
            }

            if (!found)
                sendResponse(client, makeResponse(
                    "error", "Incorrect username or password."
                ));
        } else {
            sendResponse(client, makeResponse("error", "Request not found."), "application/json", 404);
        }
    } else if (method == "GET") {
        if (path == "/")
            path = "/welcome.html";

        if (path == "/register.html")
            path = "/login.html";

        string filename = path.substr(1);

        if (filename.find("..") != string::npos) {
            sendResponse(client, "Invalid path", "text/plain", 400);
            closesocket(client);
            return;
        }

        ifstream file(filename.c_str(), ios::binary);

        if (!file) {
            sendResponse(client, "File not found: " + filename,
                         "text/plain", 404);
        } else {
            ostringstream content;
            content << file.rdbuf();

            sendResponse(client, content.str(), contentType(filename));
        }
    } else {
        sendResponse(client, "Method not allowed", "text/plain", 400);
    }

    closesocket(client);
}

int main() {
    initializeTable();
    loadUsers();

    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "Winsock initialization failed." << endl;
        return 1;
    }

    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);

    if (server == INVALID_SOCKET) {
        cout << "Could not create server socket." << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(server, (sockaddr*)&serverAddress,
             sizeof(serverAddress)) == SOCKET_ERROR) {
        cout << "Could not use port 8080. Close any previous AROGYA server." << endl;
        closesocket(server);
        WSACleanup();
        system("pause");
        return 1;
    }

    if (listen(server, 5) == SOCKET_ERROR) {
        cout << "Server could not start listening." << endl;
        closesocket(server);
        WSACleanup();
        return 1;
    }

    cout << "AROGYA server is running." << endl;
    cout << "Open http://127.0.0.1:8080 in your browser." << endl;

    while (true) {
        SOCKET client = accept(server, NULL, NULL);

        if (client != INVALID_SOCKET)
            handleClient(client);
    }

    closesocket(server);
    WSACleanup();

    return 0;
}