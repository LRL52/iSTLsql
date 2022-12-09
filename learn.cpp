#include <bits/stdc++.h>
#include "dlist.h"
using namespace std;

auto calc = [](int x)->int {
    return x + 3;
};

int sum(int&& x, int&& y) {
    x = 3;
    return x + y;
}

using callback = void (int);

void test_fun(callback f) {
    f(5);
}

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
    cout << sum(5, 6) << endl;
    // if(decltype(int) == int) cout << "emmm. " << endl;
    // auto fun = [&v1](int val)->void {
    //     v1.clear();
    //     v1.push_back(val);
    //     ++val;
    // };
    // test_fun(fun);
    // freopen("testdata.in", "r", stdin);
    // for(int i = 1; i <= 3; ++i) {
    //     string s;
    //     cin >> s;
    // }
    //fclose(stdin);
    // freopen("/dev/tty", "r", stdin);
    // string str;
    // cin >> str;
    // cout << str + string(" after fclose(stdin).") << endl;
    cout << "col1\tcol2\tcol3" << endl;
    cout.flags(ios::left);
    cout << setw(8);
    cout << "ac" << setw(8) << "w" << "dd" << endl;
    cout << "abc" << ' ' << "cde" << endl;
    return 0;
}