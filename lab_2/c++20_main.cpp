#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <execution>
#include <future>

using namespace std;

typedef vector<int>::iterator iter;

void average(iter first, iter last, promise<int> average_promise)
{
    auto f = [](const int& a, const int& b){
        this_thread::sleep_for(chrono::milliseconds(12));
        return a + b;};
    int sum = std::accumulate(first, last, 0, f);
    average_promise.set_value(sum / distance(first, last));
}

void minmax_(iter first, iter last, promise<pair<iter,iter>> minmax_promise)
{
    auto f = [](const int& a, const int& b){
        this_thread::sleep_for(chrono::milliseconds(7));
        return a < b;
    };
    minmax_promise.set_value(minmax_element(first, last, f));
}

int main(){
    cout << "Enter array dimension\n";
    int size;
    cin >> size;
    cout << "Enter array elements\n";
    vector<int> arr(size, 0);
    for_each(arr.begin(), arr.end(), [](int& n){cin >> n;});

    promise<pair<iter, iter>> minmax_promise;
    future<pair<iter, iter>> minmax_future = minmax_promise.get_future();
    thread minmax_thread(minmax_, arr.begin(), arr.end(), move(minmax_promise));

    auto minmax = minmax_future.get();
    minmax_thread.join();
    cout << "MINMAX: " << *minmax.first << " " << *minmax.second << '\n';

    promise<int> average_promise;
    future<int> average_future = average_promise.get_future();
    thread average_thread(average, arr.begin(), arr.end(), move(average_promise));

    int average = average_future.get();
    cout << "AVERAGE: " << average << '\n';
    average_thread.join();

    *minmax.first = average;
    *minmax.second = average;

    for_each(arr.begin(), arr.end(), [](const int& n){cout << n << ' ';});
}