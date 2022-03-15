#include <iostream>
#include <windows.h>
#include <vector>

using namespace std;

static int min_ = 0;
static int max_ = 0;

DWORD WINAPI min_max(LPVOID arr){
    vector<int>& array = *reinterpret_cast<vector<int>*>(arr);
    min_ = array[0];
    max_ = array[0];
    if (array.size() == 1)
        return 0;
    for (int i = array.size() % 2; i < array.size(); i += 2 ){
        if (array[i] > array[i + 1]){
            min_ = min(min_, array[i + 1]);
            max_ = max(max_, array[i]);
        }
        else {
            min_ = min(min_, array[i]);
            max_ = max(max_, array[i + 1]);
        }
    }
    return 0;
}

int main() {
    cout << "Enter array dimension\n";
    int size;
    cin >> size;
    cout << "Enter array elements\n";
    vector<int> arr(size);
    for (int i = 0; i < size; i++)
        cin >> arr[i];

    HANDLE hMinMaxThread;
    DWORD IDMinMaxThread;

    hMinMaxThread = CreateThread(NULL, 0, min_max, (void*)& arr, 0, &IDMinMaxThread);
    if (hMinMaxThread == NULL)
        return GetLastError();
    WaitForSingleObject(hMinMaxThread, INFINITE);

    cout << "MIN: " << min_ << '\n';
    cout << "MAX: " << max_;

    return 0;
}
