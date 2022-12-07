#include <bits/stdc++.h>
#include "dlist.h"
using namespace std;

auto calc = [](int x)->int {
    return x + 3;
};

int main() {
    try {
        throw invalid_argument("test error");
    } catch(exception &e) {
        cout << e.what() << endl;   
    }
    cout << calc(5) << endl;
    function<void (bool)> Assert = [](bool res)->void {
        if(res == true) cout << "true" << endl;
    };
    Assert(true);
    tuple<int, double, bool> t = make_tuple(3, 5.2, true);
    cout << get<0>(t) << ' ' << get<1>(t) << ' ' << get<2>(t) << endl;
    cout << (string("") < string("a")) << endl;
    //string s, s2;
    //cin >> s2;
    //getline(cin, s);
    //stringstream ss(s);
    //ss.str(s);
    // assert(bool(ss >> s2));
    // assert(bool(ss >> s2));
    // assert(bool(ss >> s2));
    // while(ss >> s2) {
    //     cout << s2 << endl;
    // }
    vector<int> v1;
    //v1.reserve(5);
    for(int i = 1; i <= 6; ++i) v1.emplace_back(i);
    for(auto& x : v1) cout << x << ' ';
    cout << endl;
    cout << v1.capacity() << ' ' << v1.size() << endl;
    dlist<int> l{1, 2, 3, 4, 5};
    for(auto it = l.begin(); it != l.end(); ++it) {
        if(it->data == 3) {
            l.pop(it);
            break;
        }
    }
    for(auto x : l) cout << x << ' ';
    cout << endl;
    return 0;
}