#include "dlist.h"
#include <bits/stdc++.h>
using namespace std;

class SplaySql {  
public:
    SplaySql() {}
    void start() {
        cout << "start..." << endl;
    }
};
using splaysql_ptr = SplaySql*;

class DataBase {
private:
    struct _DataBase {
        string name;
        splaysql_ptr p;
        _DataBase() noexcept : name(string("")), p(nullptr) {}
        _DataBase(const string& _name, splaysql_ptr _p) noexcept : name(_name), p(_p) {} 
    };
    dlist<_DataBase> l;

public:
    ~DataBase() {
        for(const auto& t : l) {
            delete t.p;
        }
    }

    void create(const string& s) {
        for(const auto& t : l) {
            if(t.name == s) {
                cout << "Error: " << s << " already exists." << endl;
                return;
            }
        }
        splaysql_ptr p = new SplaySql;
        l.emplace_back(s, p);
    }
    void use(const string &s) {
        for(const auto& t : l) {
            if(t.name == s) {
                t.p->start();
                return;
            }
        }
        cout << "Error: Not found database " << s << "." << endl;
    }
}db;

int main() {
    string s("test_database");
    db.create(s);
    s = "cplusplus";
    db.create(s);
    db.use(string("test_database"));
    db.use(string("ac"));
    db.create(s);
    return 0;
}
