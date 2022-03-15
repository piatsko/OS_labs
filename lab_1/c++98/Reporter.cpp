#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include<fstream>

using namespace std;

struct employee{
	int num;
	char name[10];
	double hours;
};

int main(int argc, char* argv[])
{
	employee emp;
	double perHour = atof(argv[3]);
	ofstream out;
	ifstream in;
	
	try {
		out.open(argv[2]);
	}
	catch (ios_base::failure) {
		cerr << "Failed while opening " << argv[2];
		return 1;
	}
	
	try {
		in.open(argv[1], ios::binary);
	}
	catch(ios_base::failure) {
		cerr << "Failed while opening " << argv[1];
		return 1;
	}

	out << argv[1] << '\n';
	
	int length = 0;
	try{
		in.seekg(0, ios::end);
		length = in.tellg() / sizeof(struct employee);
		in.seekg(0, ios::beg);
	}
	catch(...){
		cerr << "Failed getting length";
	}

	for (int i = 0; i < length; i++) {
		in.seekg(i * sizeof(struct employee));
		in.read((char*)& emp, sizeof(struct employee));
		out << "Number: " << emp.num << '\t' << "Name: " << emp.name << '\t' << "Hours: "
		<< emp.hours << '\t' << "Salary: " << emp.hours * perHour << '\n';
	}
	
	in.close();
	out.close();
	return 0;
}
