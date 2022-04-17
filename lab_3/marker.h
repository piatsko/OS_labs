//
// Created by Home on 16/04/2022.
//

#ifndef LAB_3_MARKER_H
#define LAB_3_MARKER_H

#endif //LAB_3_MARKER_H

#include <windows.h>

using namespace std;

static vector<int> array;

CRITICAL_SECTION workingWithArray;
HANDLE* markedEvents;
HANDLE* closeThreadEvents;
HANDLE continueEvent;

struct marker_params{
    marker_params(int number_)
    {
        number = number_;
    }
    int number;
};

DWORD WINAPI marker(void* params_){
    marker_params params = *reinterpret_cast<marker_params*>(params_);
    int number = params.number;
    int marked = 0;
    srand(number);

    EnterCriticalSection(&workingWithArray);
    while(true) {
        int random = rand();
        random %= array.size();
        if (array[random] == 0) {
            Sleep(5);
            array[random] = number;
            Sleep(5);
            marked++;
        } else {
            printf("Marker number: %d\tMarked elements: %d\t Can't mark: %d\n", number, marked, random + 1);
            LeaveCriticalSection(&workingWithArray);

            SetEvent(markedEvents[number - 1]);
            //Wait either for continue event or close event
            HANDLE* possibleOptions = new HANDLE[2];
            possibleOptions[0] = continueEvent;
            possibleOptions[1] = closeThreadEvents[number - 1];
            DWORD option = WaitForMultipleObjects(2, possibleOptions, FALSE, INFINITE);
            if (option == WAIT_OBJECT_0 + 1){
                break;
            }
        }
    }
    for (int i = 0; i < array.size(); i++){
        if (array[i] == number)
            array[i] = 0;
    }

    return 0;
}