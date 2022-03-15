#include <Windows.h>
#include <iostream>
#include <fstream>
#include <direct.h>
#include <exception>

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
		throw exception();
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}

char* CreateCommandLine(const char* name, string params[], int size)
{
    string commandLine(name);
    for (int i = 0; i < size; i++)
        commandLine.append(" ").append(params[i]);
    return const_cast<char*>(commandLine.c_str());
}

int main(int argc, char* argv[])
{
	TCHAR creatorExe[256];
	TCHAR reporterExe[256];
	GetCurrentDirectory(256, creatorExe);
	GetCurrentDirectory(256, reporterExe);
	strcat(creatorExe, "\\Creator.exe");
	strcat(reporterExe, "\\Reporter.exe");

	
	cout << "Enter binary file name\n";
	string name;
	cin >> name;
	cout << "Enter number of entries\n";
	string entries;
	cin >> entries;


	string creatorParams[2]; creatorParams[0] = name; creatorParams[1] = entries;
	char* CreatorCommandLine = CreateCommandLine(creatorExe, creatorParams, 2);

	try{
		CallProcess(CreatorCommandLine);
	}
	catch(exception& e){
		return 1;
	}
	
	ifstream creator;
    try{
        creator.open(name.c_str());
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

	string reporterParams[3];
	reporterParams[0] = name; 
	cout << "Enter report file name\n";
	string file; 
	cin >> file;
	reporterParams[1] = file;
	cout << "Enter pay per hour\n";
	cin >> reporterParams[2];

	char* ReporterCommandLine = CreateCommandLine(reporterExe, reporterParams, 3);
	
	try{
		CallProcess(ReporterCommandLine);
	}
	catch(exception& e){
		return 1;
	}
		
	ifstream report;
    try{
        report.open(file.c_str());
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