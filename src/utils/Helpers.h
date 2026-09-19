#ifndef HELPERS_H
#define HELPERS_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <limits>
#include <random>

using namespace std;

class Helpers {
public:
    static void printLine(int length = 60) {
        cout << string(length, '-') << endl;
    }

    static void printTitle(const string& title) {
        cout << endl;
        printLine(60);
        cout << "  " << title << endl;
        printLine(60);
    }

    static int getInputInt(const string& prompt) {
        int val;
        while (true) {
            cout << prompt;
            if (cin >> val) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return val;
            }
            if (cin.eof()) return 0;
            cout << "Gia tri khong hop le, vui long nhap so nguyen!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    static double getInputDouble(const string& prompt) {
        double val;
        while (true) {
            cout << prompt;
            if (cin >> val) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return val;
            }
            if (cin.eof()) return 0.0;
            cout << "Gia tri khong hop le, vui long nhap so thuc!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    static string getInputString(const string& prompt) {
        string s;
        cout << prompt;
        if (!(cin >> s)) return "";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return s;
    }

    static string getLine(const string& prompt) {
        string s;
        cout << prompt;
        if (!getline(cin, s)) return "";
        return s;
    }

    static void pauseScreen() {
        if (cin.eof()) return;
        cout << "\nNhan Enter de tiep tuc...";
        cin.get();
    }

    static string formatMoney(double amount) {
        stringstream ss;
        ss << fixed << setprecision(0) << amount;
        string numStr = ss.str();
        string res = "";
        int count = 0;
        for (int i = (int)numStr.length() - 1; i >= 0; --i) {
            res = numStr[i] + res;
            count++;
            if (count % 3 == 0 && i > 0) {
                res = "." + res;
            }
        }
        return res + " VND";
    }

    static string generateCode(const string& prefix) {
        time_t t = time(nullptr);
        tm* now = localtime(&t);
        stringstream ss;
        ss << prefix;
        if (now) {
            ss << setfill('0') << setw(2) << (now->tm_year % 100)
               << setw(2) << (now->tm_mon + 1)
               << setw(2) << now->tm_mday;
        }
        int r = rand() % 900 + 100;
        ss << r;
        return ss.str();
    }

    static string getCurrentDateTime() {
        time_t t = time(nullptr);
        tm* now = localtime(&t);
        char buf[32];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", now);
        return string(buf);
    }
};

#endif