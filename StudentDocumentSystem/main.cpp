#include "views/Application.h"
#include "utils/FileHelper.h"
#include <iostream>

using namespace std;
int main(int argc, char* argv[]) {
    try {
        string directory;
        bool dashboardOnly = false;
        for (int i = 1; i < argc; ++i) {
            string argument = argv[i];
            if (argument == "--dashboard") dashboardOnly = true;
            else if (argument == "--data" && i + 1 < argc)
                directory = argv[++i];
            else {
                cerr << "Cach dung: StudentDocumentSystem.exe [--data thu_muc] [--dashboard]\n";
                return 1;
            }
        }
        if (directory.empty()) directory = FileHelper::DefaultDataDirectory();
        cout << "Du lieu: " << directory << '\n';
        Application app(directory);
        app.Run(dashboardOnly);
        return 0;
    } catch (const exception& error) {
        cerr << "Khong the chay: " << error.what() << '\n';
        return 1;
    }
}
