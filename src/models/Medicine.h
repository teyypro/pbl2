#ifndef MEDICINE_H
#define MEDICINE_H

#include <string>
using namespace std;

struct Medicine {
    int id = 0;
    string code;
    string name;
    string active_ingredient;
    string unit;
    double price = 0;
    int stock_quantity = 0;
    int min_stock = 10;
    string expiry_date;
    string manufacturer;
    bool requires_prescription = true;
    string status;     // available, out_of_stock, discontinued
};

#endif