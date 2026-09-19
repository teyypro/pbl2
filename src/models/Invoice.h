#ifndef INVOICE_H
#define INVOICE_H

#include <string>
#include <vector>
using namespace std;

struct InvoiceItem {
    int id = 0;
    int invoice_id = 0;
    string item_type;   // service, medicine
    int item_id = 0;
    string item_name;
    int quantity = 0;
    double unit_price = 0;
    double discount = 0;
    double amount = 0;
    string note;
};

struct Invoice {
    int id = 0;
    string invoice_code;
    int patient_id = 0;
    int medical_record_id = 0;
    int prescription_id = 0;
    double total_amount = 0;
    double bhyt_discount = 0;
    double other_discount = 0;
    double patient_pay = 0;
    double paid_amount = 0;
    string payment_method;
    string payment_status; // pending, paid, partial
    string invoice_date;
    int created_by = 0;
    string note;

    // Join fields
    string patient_name;

    // Chi tiet hoa don
    vector<InvoiceItem> items;
};

#endif