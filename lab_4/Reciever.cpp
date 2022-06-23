#include<vector>
#include "message.h"

using namespace std;

HANDLE StartNewProcess(wstring commandLine)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);

    if (!CreateProcessW(NULL,
                       (LPWSTR)commandLine.c_str(),
                  NULL,
                  NULL,
                  FALSE,
                  CREATE_NEW_CONSOLE,
                  NULL,
                  NULL,
                        reinterpret_cast<LPSTARTUPINFOW>(&si),
                  &pi))
    {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
    }

    CloseHandle(pi.hThread);

    return pi.hProcess;
}

vector<HANDLE> StartSenderProcesses(int processesCount, wstring binaryFileName)
{
    vector<HANDLE> processesHandles;

    for (int i = 0; i < processesCount; i++){
        wstring commandLine = L"Sender.exe " + binaryFileName + L" " + ConvertIntToWstring(i);
        processesHandles.push_back(StartNewProcess(commandLine));
    }

    return processesHandles;
}

HANDLE* CreateSenderStartEvents(int sendersCount)
{
    HANDLE* startEvents = new HANDLE[sendersCount];
    for (int i = 0; i < sendersCount; i++){
        startEvents[i] = CreateEvent(
                NULL,
                TRUE,
                FALSE,
                (LPSTR)(L"Sender " + ConvertIntToWstring(i) + L"Event").c_str());
    }
    return startEvents;
}

void FreeSenderStartEvents(HANDLE* senderStartEvents)
{
	delete[] senderStartEvents;
}

int main()
{
    cout << "Enter binary file name:\n";
    wstring fileName; wcin >> fileName;

    cout << "\nEnter number of entries:\n";
    int entriesCount; cin >> entriesCount;

    fstream file;

    try{
        file.open(fileName.c_str(), ios::out | ios::binary | ios::trunc);
    }
    catch(ios_base::failure){
        cerr << "Failed to create file" << '\n';
        return 1;
    }
    file.close();

    cout << "\nEnter number of Sender processes:\n";
    int sendersCount; cin >> sendersCount;

    vector<HANDLE> processes = StartSenderProcesses(sendersCount, fileName);
    HANDLE* senderStartEvents = CreateSenderStartEvents(sendersCount);
    HANDLE currentProcess = CreateMutex(NULL, FALSE, "Current Process Mutex");
    HANDLE fileReadSemaphore = CreateSemaphore(NULL, 0, entriesCount, "Read Semaphore");
    HANDLE fileWriteSemaphore = CreateSemaphore(NULL, entriesCount, entriesCount, "Write Semaphore");

    WaitForMultipleObjects(sendersCount, senderStartEvents, TRUE, INFINITE);

    int alreadyRead = 0;

    while(true){
        cout << "Select option: \n 1: Read from file \n 2: Exit\n";
        int option; cin >> option;
        if (option == 2)
            break;

        WaitForSingleObject(fileReadSemaphore, INFINITE);
        WaitForSingleObject(currentProcess, INFINITE);

        file.open(fileName.c_str(), ios::binary | ios::in | ios::out);
        file.seekg(alreadyRead * sizeof(Message), ios::beg);

        Message mes;
        file >> mes;
        while(!file.eof()){
            cout << "Got message " << ++alreadyRead << " : " << mes.GetMes() << endl;
            file >> mes;
        }
        file.close();

        ReleaseMutex(currentProcess);
        ReleaseSemaphore(fileWriteSemaphore, entriesCount, NULL);
    }

    for (int i = 0; i < processes.size(); i++){
        CloseHandle(processes[i]);
        CloseHandle(senderStartEvents[i]);
    }

	FreeSenderStartEvents(senderStartEvents);
    CloseHandle(fileWriteSemaphore);
    CloseHandle(fileReadSemaphore);
}