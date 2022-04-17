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

HANDLE* CreateEvents(int count, bool manualReset, bool initialState){
    HANDLE* events = new HANDLE[count];
    for (int i = 0; i < count; i++){
        events[i] = CreateEventA(NULL, manualReset, initialState, NULL);
    }
    return events;
}

void print(vector<int>& v){
    for (int i = 0; i < v.size(); i++)
        cout << v[i] << " ";
    cout << endl;
}

void SetRemovedEvents(vector<HANDLE>& removedEvents){
    for (int i = 0; i < removedEvents.size(); i++){
        SetEvent(removedEvents[i]);
    }
}

int main() {

    InitializeCriticalSection(&workingWithArray);

    cout << "Enter array size: ";
    int arr_size; cin >> arr_size;
    array = vector<int>(arr_size, 0);

    cout << "Enter markers count: ";
    int marker_count; cin >> marker_count;

    markedEvents = CreateEvents(marker_count, FALSE, FALSE);
    continueEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
    closeThreadEvents = CreateEvents(marker_count, TRUE, FALSE);
    vector<HANDLE> threads_handles = start_threads(marker_count);

    vector<HANDLE> removedMarkedEvents;

    int active_markers = marker_count;
    while (active_markers != 0){
        SetRemovedEvents(removedMarkedEvents);
        WaitForMultipleObjects(marker_count, markedEvents, TRUE, INFINITE);
        print(array);

        cout << "Enter # of marker to be closed: " << endl;
        int num; cin >> num;
        SetEvent(closeThreadEvents[num - 1]);
        WaitForSingleObject(threads_handles[num - 1], INFINITE);
        removedMarkedEvents.push_back(markedEvents[num - 1]);
        active_markers--;
        PulseEvent(continueEvent);
    }

    cout << "RESULT ARRAY" << endl;
    print(array);
}
