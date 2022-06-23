#pragma once
#ifndef MESSAGE_H
#define MESSAGE_H
#include<iostream>
#include<fstream>
#include<Windows.h>

using namespace std;

wstring ConvertIntToWstring(int i){
    wchar_t* buf = new wchar_t[20];
    swprintf(buf, 20, L"%d", i);
    return (wstring)buf;
}

struct Message{
    Message(){}

    Message(const string& s){
        strcpy(mes, s.c_str());
    }

    char* GetMes(){
        return mes;
    }

    friend fstream& operator <<(fstream& file, const Message& msg) {
        file.write((const char*)& msg, sizeof(struct Message));
        return file;
    }

    friend fstream& operator >>(fstream& file, Message& msg){
        file.read((char*)& msg, sizeof(struct Message));
        return file;
    }

private:
    char mes[20];
};
#endif
