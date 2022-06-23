#include "employee.h"
#include <map>
#include <vector>
#include <windows.h>

using namespace std;

int activeClientsCount;
int empCount;
int clientsCount;
int const MAX_MES_SIZE = 40;

int* currentRecordReaders;
vector<HANDLE> serverThreads;
map<int, int> employeesRecordsInFileById;

HANDLE* writeSemaphores;
string fileName;
fstream file;

HANDLE StartClientProcess() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    if (!CreateProcess(NULL,
                       "Client.exe",
                       NULL,
                       NULL,
                       FALSE,
                       CREATE_NEW_CONSOLE,
                       NULL,
                       NULL,
                       &si,
                       &pi)) {
        printf("CreateProcess failed (%d).\n", GetLastError());
    }

    CloseHandle(pi.hThread);

    return pi.hProcess;
}

employee GetRecordFromFile(int recNumber)
{
    file.open(fileName.c_str(), ios::in | ios::binary | ios::app);
    file.seekg(recNumber * sizeof(employee), ios::beg);
    employee emp;
    file >> emp;
    file.close();
    return emp;
}

DWORD WINAPI ProcessClientMessage(LPVOID params){
    HANDLE namedPipe = params;
    char* clientMes = new char[MAX_MES_SIZE];
    HANDLE writingMutex = CreateMutexA(NULL, FALSE, NULL);
    employee emp;

    while(true) {

        if (!ReadFile(namedPipe, clientMes, MAX_MES_SIZE * sizeof(char), NULL, NULL)) {
            cerr << "Reading client message failed: " << endl
                 << "The last error code: " << GetLastError() << endl;
        }

        char mode = clientMes[0];

        int id = strtol(clientMes + 2, NULL, 10);

        int recordNumber = employeesRecordsInFileById[id];

        if (mode == 'r') {
            currentRecordReaders[recordNumber]++;

            emp = GetRecordFromFile(recordNumber);
            if (!WriteFile(namedPipe, &emp, sizeof(employee), NULL, NULL)) {
                cerr << "Sending entry to client failed: " << endl
                     << "The last error code: " << GetLastError() << endl;
            }

            WaitForSingleObject(writeSemaphores[recordNumber], INFINITE);
        } else if (mode == 'w') {

            emp = GetRecordFromFile(recordNumber);
            if (!WriteFile(namedPipe, &emp, sizeof(employee), NULL, NULL)) {
                cerr << "Sending entry to client failed: " << endl
                     << "The last error code: " << GetLastError() << endl;
            }

            if (!ReadFile(namedPipe, &emp, sizeof(employee), NULL, NULL)){
                cerr << "Couldn't read new entry" << endl
                     << "The last error code: " << GetLastError() << endl;
            }

            if (currentRecordReaders[recordNumber] == 0){
                WaitForSingleObject(writingMutex, INFINITE);

                file.open(fileName.c_str(), ios::binary | ios::out | ios::in);
                file.seekp(recordNumber * sizeof(employee), ios::beg);
                file << emp;
                file.close();

                if(!WriteFile(namedPipe, "CHANGED SUCCESSFULLY!", MAX_MES_SIZE, NULL, NULL)){
                    cerr << "Couldn't tell about modifying" << endl
                         << "The last error code: " << GetLastError() << endl;
                }

                ReleaseMutex(writingMutex);
            }
            else{
                if(!WriteFile(namedPipe, "CAN'T CHANGE CONTENT AT THE MOMENT!", MAX_MES_SIZE, NULL, NULL)){
                    cerr << "Couldn't tell about modifying" << endl
                         << "The last error code: " << GetLastError() << endl;
                }
            }
        } else if (mode == 'g') {
            currentRecordReaders[recordNumber]--;
            ReleaseSemaphore(writeSemaphores[recordNumber], 1, NULL);
        } else {
            activeClientsCount--;
            break;
        }
    }

    FlushFileBuffers(namedPipe);
    DisconnectNamedPipe(namedPipe);
    CloseHandle(namedPipe);
}

void PrepareToWork()
{
    writeSemaphores = new HANDLE[empCount];
    for (int i = 0; i < empCount; i++){
        writeSemaphores[i] = CreateSemaphoreA(NULL, 0, clientsCount, NULL);
    }

    serverThreads.resize(clientsCount);

    currentRecordReaders = new int[empCount];
    for (int i = 0; i < empCount; i++){
        currentRecordReaders[i] = 0;
    }
}

void FreeResources()
{
    for (int i = 0; i < empCount; i++)
        CloseHandle(writeSemaphores[i]);

    serverThreads.clear();
    delete[] currentRecordReaders;
}

void printFile()
{
    file.open(fileName.c_str(), ios::in | ios::binary | ios::app);
    for (int i = 0; i < empCount; i++) {
        employee emp;
        file >> emp;
        cout << emp << '\n';
    }
    file.close();
}

bool EstablishConnection()
{
    HANDLE namedPipe;
    for (int i = 0; i < clientsCount; i++) {
        StartClientProcess();

        namedPipe = CreateNamedPipeA(
                "\\\\.\\pipe\\my_pipe",
                PIPE_ACCESS_DUPLEX,
                PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                clientsCount,
                0,
                0,
                INFINITE,
                NULL);

        if (!ConnectNamedPipe(namedPipe,(LPOVERLAPPED) NULL))
        {
            cerr << "The connection failed." << endl
                 << "The last error code: " << GetLastError() << endl;
            CloseHandle(namedPipe);
            return false;
        }
        serverThreads[i] = CreateThread(NULL, 0, ProcessClientMessage, namedPipe, 0, NULL);
    }
    return true;
}

void CreateBinatyFile()
{
    cout << "Enter file name: \n";
    cin >> fileName;

    try {
        file.open(fileName.c_str(), ios::out | ios::binary | ios::trunc);
        file.close();
    }
    catch (ios_base::failure) {
        cerr << "Failed to create file" << '\n';
    }
}

void FillFile()
{
    cout << "Enter number of employees: \n";
    cin >> empCount;

    file.open(fileName.c_str(), ios::binary | ios::out | ios::app);

    for (int i = 0; i < empCount; i++) {
        cout << "\nEnter employee ID, name and working hours: \n";
        int id;
        char name[10];
        double workingHours;
        cin >> id >> name >> workingHours;
        file << employee(id, name, workingHours);
        employeesRecordsInFileById[id] = i;
    }

    file.close();
}

int main() {
    CreateBinatyFile();
    FillFile();

    cout << "\nCreated file\n";
    printFile();

    cout << "Enter number of clients: \n";
    cin >> clientsCount;

    PrepareToWork();
    EstablishConnection();
    WaitForMultipleObjects(clientsCount, serverThreads.data(), TRUE, INFINITE);

    cout << "\nFINAL FILE\n";
    printFile();

    FreeResources();
}