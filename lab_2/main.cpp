#include <iostream>
#include <windows.h>
#include <vector>
#include <typeinfo>

using namespace std;

static int min_ = 0;
static int max_ = 0;
static int average_ = 0;

DWORD WINAPI min_max(LPVOID arr){
    vector<int>& array = *reinterpret_cast<vector<int>*>(arr);
    min_ = array[0];
    max_ = array[0];
    if (array.size() == 1)
        return 0;
    for (int i = array.size() % 2; i < array.size(); i += 2 ){
        if (array[i] > array[i + 1]){
            min_ = min(min_, array[i + 1]);
            Sleep(7);
            max_ = max(max_, array[i]);
            Sleep(7);
        }
        else {
            min_ = min(min_, array[i]);
            Sleep(7);
            max_ = max(max_, array[i + 1]);
            Sleep(7);
        }
        Sleep(7);
    }

    return 0;
}

DWORD WINAPI average(LPVOID arr){
    vector<int>& array = *reinterpret_cast<vector<int>*>(arr);
    for (int i = 0; i < array.size(); i++){
        average_ += array[i];
        Sleep(12);
    }
    average_ /= array.size();

    return 0;
}

void changeMaxMin(vector<int>& arr){
    bool minChanged = 0, maxChanged = 0;
    for (int i = 0; i < arr.size(); i++){
        if (arr[i] == max_ && !maxChanged){
            arr[i] = average_;
            maxChanged = true;
        }
        else if (arr[i] == min_ && !minChanged){
            arr[i] = average_;
            minChanged = true;
        }
    }
}

int main() {
    cout << "Enter array dimension\n";
    int size;
    cin >> size;
    cout << "Enter array elements\n";
    vector<int> arr(size);
    for (int i = 0; i < size; i++) {
        cin >> arr[i];
    }

    HANDLE hMinMaxThread;
    DWORD IDMinMaxThread;

    hMinMaxThread = CreateThread(NULL, 0, min_max, (void*)& arr, 0, &IDMinMaxThread);
    if (hMinMaxThread == NULL)
        return GetLastError();
    WaitForSingleObject(hMinMaxThread, INFINITE);

    HANDLE hAverageThread;
    DWORD IDAverageThread;

    hAverageThread = CreateThread(NULL, 0, average, (void*)& arr, 0, &IDAverageThread);
    if (hAverageThread == NULL)
        return GetLastError();
    WaitForSingleObject(hAverageThread, INFINITE);

    cout << "MIN: " << min_ << '\n';
    cout << "MAX: " << max_ << '\n';
    cout << "AVERAGE: " << average_ << '\n';

    changeMaxMin(arr);

    cout << "RESULTING ARRAY: ";
    for (int i = 0; i < size; i++)
        cout << arr[i] << ' ';
    cout << '\n';

    return 0;
}
