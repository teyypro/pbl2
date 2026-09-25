#ifndef HELPERS_H
#define HELPERS_H

#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <limits>
#include <random>

using namespace std;

class Helpers {
public:
    static void clearScreen();
    static void printLine(int length = 60);
    static void printTitle(const string& title);
    static int getInputInt(const string& prompt);
    static double getInputDouble(const string& prompt);
    static string getInputString(const string& prompt);
    static string getLine(const string& prompt);
    static void pauseScreen();
    static string formatMoney(double amount);
    static string generateCode(const string& prefix);
    static string getCurrentDateTime();
};

#endif