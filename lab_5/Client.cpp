#include "employee.h"
#include <windows.h>

using std::cin;
using std::string;
using std::cerr;
using std::endl;

HANDLE namedPipe;
int const MAX_MES_SIZE = 40;

BOOL SendMessageToServer(char mode, int id) {
    char mes[MAX_MES_SIZE];
    sprintf(mes, "%c %d", mode, id);
    if (!WriteFile(
            namedPipe,
            mes,
            MAX_MES_SIZE,
            NULL,
            NULL
    )) {
        cerr << "Writing to the named pipe failed: " << endl
             << "The last error code: " << GetLastError() << endl;
        return FALSE;
    }
    return TRUE;
}

int main() {
    const char* pipeName = "\\\\.\\pipe\\my_pipe";

    if(!WaitNamedPipeA(pipeName, NMPWAIT_WAIT_FOREVER)){
        cerr << "Wait pipe failed" << endl
            << "Last error: " << GetLastError() << endl;
    }

    namedPipe = CreateFile(
            pipeName,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

    while (true) {
        cout << "\nSelect option: \n 1 - modify entry\n 2 - read entry\n 3 - exit\n";
        int option;
        cin >> option;
        if (option == 3) {
            SendMessageToServer('c', -1);
            break;
        }
        cout << "\nEnter employee's ID: \n";
        int id;
        cin >> id;

        if (option == 2)
            SendMessageToServer('r', id);

        if (option == 1)
            SendMessageToServer('w', id);

        employee emp;

        if (!ReadFile(namedPipe, &emp, sizeof(employee), NULL, NULL)){
            cerr << "Reading to the named pipe failed\n" << endl
            << "The last error code: " << GetLastError() << endl;
        }
        cout << "\n Got answer from server: \n" << emp;
        if (option == 1) {
            cout << "\nEnter new entry (id, name, work hours)\n";

            employee emp_changed;
            cin >> emp_changed;

            if (!WriteFile(namedPipe, &emp_changed, sizeof(employee), NULL, NULL)){
                cerr << "Sending changed record failed\n"
                    << "Last error code: " << GetLastError() << endl;
            }

            char notification[MAX_MES_SIZE];
            if (!ReadFile(namedPipe, notification, MAX_MES_SIZE, NULL, NULL)) {
                cerr << "Reading to the named pipe failed\n" << endl
                     << "The last error code: " << GetLastError() << endl;
            }

            cout << notification << endl;
        }
        if (option == 2) {
            char c;
            cout << "\n Press any key to continue: \n";
            cin >> c;
            SendMessageToServer('g', id);
        }
    }
}
