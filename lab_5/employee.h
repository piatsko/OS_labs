#ifndef LAB_5_EMPLOYEE_H
#define LAB_5_EMPLOYEE_H

#include <cstring>
#include <iostream>
#include <fstream>

using std::cout;
using std::fstream;
using std::ostream;
using std::istream;
using std::string;

struct employee {

private:
    int num;
    char name[10];
    double hours;

public:
    employee() : num(), name(), hours(){}

    employee(int num, const char name[], double hours) : num(num), hours(hours) {
        if (strlen(name) > 10) {
            cout << "Name is too long";
        } else strcpy(this->name, name);
    }

    operator char*() const {
        char buf[80];
        sprintf(buf, "%d %s %f", &num, &name, &hours);
        return buf;
    }

    friend fstream &operator>>(fstream &in, employee &emp) {
        in.read((char *) &emp, sizeof(emp));
        return in;
    }

    friend fstream &operator<<(fstream &out, const employee &emp) {
        out.write((const char *) &emp, sizeof(emp));
        return out;
    }

    friend ostream& operator<<(ostream& os, const employee& emp){
        os << emp.num << " " << emp.name << " " << emp.hours;
        return os;
    }

    friend istream& operator>>(istream& is, employee& emp){
        is >> emp.num >> emp.name >> emp.hours;
        return is;
    }
};

#endif //LAB_5_EMPLOYEE_H
