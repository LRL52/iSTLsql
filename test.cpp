//main.cpp
#include "dlist.h"
#include <bits/stdc++.h>
using namespace std;

struct Node {
    int a, b;
};


union U {
    string s;
    int n;
public:
    U() { new(&s) string; }
    ~U() { s.~string(); }
};

class SplaySql {  
public:
    int x, y;
    SplaySql(int _x, int _y) noexcept : x(_x), y(_y) {}
    // bool operator<(const Node&t1, const Node&t2) const {
    //     if(this->x > this->y) return t1.a < t2.a;
    //     else return t1.b < t2.b;
    // }
    friend std::ostream& operator<<(std::ostream& os, const SplaySql& t) {
        return std::cout << t.x << ' ' << t.y << std::endl;
    }
};
using splaysql_ptr = SplaySql*;
enum class data_type { INT, STRING };
using column_property = pair<int, data_type>;
unordered_map<string, column_property> H;

struct _DataBase {
    string name;
    int p;
    _DataBase() noexcept : name(string("")) , p(0) {}
    _DataBase(const string& _name, int _p) noexcept : name(_name), p(_p) {} 
};

int main() {
    dlist<SplaySql*> l;
    SplaySql *t1 = new SplaySql(3, 5);
    l.emplace_back(t1);
    SplaySql *t2 = new SplaySql(6, 8);
    l.emplace_back(t2);
    for(const splaysql_ptr& u : l)
        std::cout << *u;
    delete t1;
    delete t2;
    // U t;
    // t.s += "abc";
    // cout << t.s << endl;
    // t.n = 3;
    // cout << t.n << endl;
    H["id"] = make_pair(3, data_type::STRING);
    H["age"] = make_pair(2, data_type::INT);
    for(const auto& t : H) {
        if(t.second.second == data_type::STRING)
            cout << t.first << ' ' << t.second.first << ' ' << "string" << endl;
        else
            cout << t.first << ' ' << t.second.first << ' ' << "int" << endl;
    }
    string s1("abcd");
    string s2("abcd");
    if(s1 == s2) cout << "s1 = s2" << endl;
    s1.erase(s1.begin());
    cout << s1 << endl;
    dlist<_DataBase> l2;
    // string data("3359348598");
    // cout << stoi(data) << endl;
    return 0;
}
