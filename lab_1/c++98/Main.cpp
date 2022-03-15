#include<Windows.h>
#include<iostream>
#include<fstream>
#include <direct.h>

using namespace std;

void CallProcess(char* commandLine)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    // Start the child process. 
    if( !CreateProcess( NULL,   // No module name (use command line)
        commandLine,    // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}

char* CreateCommandLine(const char* name, char** params, int size)
{
    string commandLine(name);
    for (int i = 0; i < size; i++)
        commandLine.append(" ").append(params[i]);
    return const_cast<char*>(commandLine.c_str());
}

int main(int argc, char* argv[])
{
	cout << "Enter binary file name\n";
	char name[20];
	char entries[20];
	try{
        cin.getline(name, 20);
	}
	catch(...){
		cerr << "Exception in name\n";
		return 1;
	}
	cout << "Enter amount of entries\n";
	try{
        cin >> entries;
	}
	catch(...){
		cerr << "Exception in entries\n";
		return 1;
	}

	char** creatorParams = new char*[2]; creatorParams[0] = name; creatorParams[1] = entries;
	char* CreatorCommandLine = CreateCommandLine("Creator.exe", creatorParams, 2);

	CallProcess(CreatorCommandLine);
	
	ifstream creator;
    try{
        creator.open(name);
    }
    catch(...){
        cerr << "Something went wrong with opening creator file" << '\n';
        return 1;
    }

    cout << "Content of created binary file \n";
    while (!creator.eof()){
        cout << creator.get();
    }
    cout << '\n';
	creator.close();

	char** reporterParams = new char*[3]; 
	reporterParams[0] = name; 
	char perHour[10];
	char file[20];
	try {
		cout << "Enter report file name\n";
        cin >> file;
		reporterParams[1] = file;
		cout << "Enter pay per hour\n";
		cin >> perHour;
		reporterParams[2] = perHour;
	}
	catch(...){
        cout << "Exception in reporter params\n";
		return 1;
	}
	char* ReporterCommandLine = CreateCommandLine("Reporter.exe", reporterParams, 3);
	
	CallProcess(ReporterCommandLine);
		
	ifstream report;
    try{
        report.open(file);
    }
    catch(...){
        cerr << "Something went wrong with opening reporter file\n";
    }
    while (!report.eof()){
        char buf[256];
        report.getline(buf, 256);
        cout << buf << '\n';
    }
    report.close();
	return 0;
}