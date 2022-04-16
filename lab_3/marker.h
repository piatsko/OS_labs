//
// Created by Home on 16/04/2022.
//

#ifndef LAB_3_MARKER_H
#define LAB_3_MARKER_H

#endif //LAB_3_MARKER_H

#include <windows.h>

using namespace std;

static vector<int> array;

struct marker_params{
    marker_params(int index_)
    {
        index = index_;
        marked = 0;
    }
    int index;
    int marked;
};

DWORD WINAPI marker(void* params_){
    marker_params params = *reinterpret_cast<marker_params*>(params_);
    int index = params.index;
    int& marked =  params.marked;
    srand(index);

    while (true) {
        int random = rand();
        random %= array.size();

        if (array[random] == 0) {
            Sleep(5);
            array[random] = index;
            Sleep(5);
            marked++;
        } else {
            cout << "Marker index: " << index
                 << "\tMarked elements: " << marked
                 << "\tCan't mark: " << random
                 << endl;
            break;
        }
    }
    return 0;
}