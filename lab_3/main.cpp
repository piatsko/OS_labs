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

int main() {
    cout << "Enter array size: ";
    int arr_size; cin >> arr_size;
    array = vector<int>(arr_size, 0);

    cout << "Enter markers count: ";
    int marker_count; cin >> marker_count;

    vector<HANDLE> threads_handles = start_threads(marker_count);

    for (int i = 0; i < array.size(); i++)
        WaitForSingleObject(threads_handles[i], INFINITE);

    int active_markers = marker_count;

    cout << "RESULT ARRAY" << endl;
    for (int i = 0; i < array.size(); i++)
        cout << array[i] << " ";
}
