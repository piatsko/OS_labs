#include<iostream>
#include<fstream>
#include<sstream>

using namespace std;

struct employee{
	int num;
	char name[10];
	double hours;
};

int main(int argc, char* argv[])
{
	string name = argv[1];
	stringstream ss(argv[2]);
	int n; 
	ss >> n;
	ofstream out;
	
	try{
		out.open(name.c_str(), ofstream::binary);
	}
	catch(ios_base::failure){
		cerr << "Failed to create file" << '\n';
	}
	
	struct employee emp;
	emp.num = 0;
	emp.hours = 0.0;
	
	printf("%s\n", "Enter employee's number, name and working hours");
	
	for (int i = 0; i < n; i++){
		cin >> emp.num >> emp.name >> emp.hours;
		out.write((const char*)&emp, sizeof(struct employee));
	}
	out.close();
	return 0;
}