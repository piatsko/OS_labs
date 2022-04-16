#include <iostream>
#include <vector>
#include "marker.h"

vector<HANDLE> start_threads(int count){
    vector<HANDLE> threads_handles(count);
    for (int i = 0; i < count; i++){
        HANDLE hThread;
        DWORD IDThread;
        hThread = CreateThread(
                NULL,
                0,
                marker,
                (void*)new marker_params(i + 1),
                0,
                &IDThread);
        if(hThread != NULL){
            cout << "Marker thread #" << i + 1 << " created successfully" << endl;
            threads_handles[i] = hThread;
        }
        else{
            cout << "Something went wrong. Error code: " << GetLastError();
        }
    }
    return threads_handles;
}

bool getBoolArrValue(vector<bool>& v){
    bool answ = 1;
    for (int i = 0; i < v.size(); i++)
        if (isActive[i])
            answ &= v[i];
    return answ;
}

void setIsLocked(vector<bool>& v){
    for (int i = 0; i < v.size(); i++)
        if (isActive[i])
            v[i] = 0;
}

void print(vector<int>& v){
    for (int i = 0; i < v.size(); i++)
        cout << v[i] << " ";
    cout << endl;
}

int main() {

    cout << "Enter array size: ";
    int arr_size; cin >> arr_size;
    array = vector<int>(arr_size, 0);

    cout << "Enter markers count: ";
    int marker_count; cin >> marker_count;

    isLocked = vector<bool>(marker_count, 0);
    isActive = vector<bool>(marker_count, 1);
    vector<bool> isClosed(marker_count, 0);

    vector<HANDLE> threads_handles = start_threads(marker_count);

    while (!getBoolArrValue(isClosed)){
        while (!getBoolArrValue(isLocked));

        print(array);

        cout << "Enter # of marker to be closed: " << endl;
        int num; cin >> num;
        isActive[num - 1] = 0;
        isClosed[num - 1] = 1;
        WaitForSingleObject(threads_handles[num - 1], INFINITE);

        setIsLocked(isLocked);
    }

    cout << "RESULT ARRAY" << endl;
    print(array);
}
