//
// Created by Home on 16/04/2022.
//

#ifndef LAB_3_MARKER_H
#define LAB_3_MARKER_H

#endif //LAB_3_MARKER_H

#include <windows.h>

using namespace std;

static vector<int> array;

static vector<bool> isLocked;
static vector<bool> isActive;

int lock = 0;

void xchg(register int& r, int& x){
    int temp;

    temp = r;
    r = x;
    x = temp;
}

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

    register int key = 1;
    while (isActive[number - 1]) {
        while(key == 1)
            xchg(key, lock);
        while (!isLocked[number - 1]) {
            int random = rand();
            random %= array.size();
            if (array[random] == 0) {
                Sleep(5);
                array[random] = number;
                Sleep(5);
                marked++;
            } else {
                printf("Marker number: %d\tMarked elements: %d\t Can't mark: %d\n", number, marked, random + 1);
                isLocked[number - 1] = 1;
            }
        }
        lock = 0;
    }

    for (int i = 0; i < array.size(); i++){
        if (array[i] == number)
            array[i] = 0;
    }

    return 0;
}
#pragma clang diagnostic pop