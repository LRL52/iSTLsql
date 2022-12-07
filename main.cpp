#include "dlist.h"
#include <bits/stdc++.h>
using namespace std;

enum class data_type { INT, STRING };

function<void (bool)> Assert = [](bool res)->void { //maybe just use for learning c++ new feature 
    if(res == false)
        throw invalid_argument("Error: invalid argument!");
};
//#define check(x) check(s, x)

class NameException : public exception {
private:
    string message;
public:
    NameException(const string &s) noexcept : message(s) {}
    virtual const char* what() const throw() {
        return message.c_str();
    }
};

template<typename T>
bool compare(const T& x, const string& op, const T& y) {
    if(op == "=") return x == y;
    if(op == ">") return x > y;
    return x < y;
}

class Data {
private:
    vector<int> val_int;
    vector<string> val_string;
public:
    void emplace_int(const int& val) {
        val_int.emplace_back(val);
    }
    void emplace_string(const string& val) {
        val_string.emplace_back(val);
    }
    int at_int(const size_t& pos) {
        if(pos >= val_int.size())
            throw out_of_range(string("Error: ") + to_string(pos) + string("out of range!"));
        return val_int[pos];
    }
    string at_string(const size_t& pos) {
        if(pos >= val_string.size())
            throw out_of_range(string("Error: ") + to_string(pos) + string("out of range!"));
        return val_string[pos];
    }
    void resize(size_t int_cnt, size_t string_cnt) {
        val_int.resize(int_cnt);
        val_string.resize(string_cnt);
    }
    Data(size_t int_cnt = 0, size_t string_cnt = 0) noexcept {
        val_int.reserve(int_cnt); //这里应该使用reserve
        val_string.reserve(string_cnt);
    }
    ~Data() {}
};
using data_ptr = Data*;

class Table {
public:
    //using column_property = pair<int, data_type>;
    using column_property = tuple<size_t, data_type, bool>; //依次为每一列的存储位置，数据类型，是否为主键
private:
    unordered_map<string, column_property> hash_table; //记录每一列的属性
    size_t string_cnt = 0, int_cnt = 0;
    dlist<data_ptr> l;
public:
    Table() {}
    ~Table() {
        for(auto &t : l)
            delete t;
    }
    void create() {
        string name, type, primary;
        bool end_flag = false, first = true; //第一次需要判断'('
        auto fix = [&]()->void { //使用lamada表达式，解决读入末尾字符处理以及判断是否读入完的问题
            auto last = type[type.size() - 1]; //type的最后一个字符，成功读入后type一定非空
            if(last == ')') end_flag = true; //读入完成
            if(last != ',' && last != ')') {
                cin >> primary;
                Assert(primary.size() >= 7);
                if(primary.back() == ')') end_flag = true;
                primary.pop_back();
                Assert(primary == "primary");
            } else
                type.pop_back();
            Assert(type == "int" || type == "string");
        };
        // try {
            while(true) {
                cin >> name;
                if(first) {
                    first = false;
                    Assert(name.size() > 1 && name[0] == '(');
                    //name = name.substr(1, string::npos); //去掉(
                    name.erase(name.begin());
                }
                cin >> type;
                fix();
                if(hash_table.find(name) != hash_table.end())
                    throw NameException(string("Error: ") + name + string(" already exists."));
                hash_table[name] = make_tuple((type == "int" ? int_cnt++ : string_cnt++),
                                              (type == "int" ? data_type::INT : data_type::STRING), 
                                              (primary == "primary" ? true : false));
                //to-do,处理主键,建立索引
                if(primary == "primary") {
                    //...
                    primary.clear();
                }
                if(end_flag) return;
            }
        // } catch(invalid_argument &e) { //冗余代码？
        //     cout << e.what() << endl;
        // } catch(NameException &e) {
        //     cout << e.what() << endl; 
        // } catch(exception &e) {
        //     cout << e.what() << endl;
        // }
    }
    void insert() {
        auto item = new Data(int_cnt, string_cnt);
        string data; //仅一个数据(string 或 int)
        bool end_flag = false, first = true;
        // size_t cnt1 = 0, cnt2 = 0; //cnt1, cnt2分别是int和string的个数
        auto fix = [&]()->data_type {
            auto last = data[data.size() - 1];
            if(last == ')') end_flag = true;
            else Assert(last == ',');
            data.pop_back();
            Assert(data.size() > 0);
            if(data[0] == '\"') {
                Assert(data.size() >= 2);
                data = data.substr(1, data.size() - 2);
                return data_type::STRING;
            } else
                return data_type::INT;
        };
        try {
            while(true) {
                cin >> data;
                if(first) {
                    first = false;
                    Assert(data.size() > 7 && data.substr(0, 7) == "values(");
                    data = data.substr(7, string::npos);
                }
                auto type = fix();
                if(type == data_type::INT) {
                    auto val = stoi(data);
                    item->emplace_int(val);
                } else
                    item->emplace_string(data);
                if(end_flag) break;
            }
            l.emplace_back(item);
        } catch(invalid_argument &e) { //冗余代码
            delete item;
            cout << e.what() << endl;
        } catch(exception &e) {
            delete item;
            cout << e.what() << endl;
        }
    }
    void select(string &name, stringstream &ss) {
        string s, op, val;
        // int ival;
        bool first = true;
        auto sel = hash_table.find(name);
        if(sel == hash_table.end())
            throw NameException(string("Error: ") + name + string(" doesn't exist."));
        auto sel_type = get<1>(sel->second);
        auto sel_pos = get<0>(sel->second);
        auto output = [&name, &first](data_ptr p,data_type type, size_t pos)->void {
            if(first) {
                first = false;
                cout << name << endl;
            }
            if(type == data_type::INT) cout << p->at_int(pos) << endl;
            else cout << p->at_string(pos) << endl;
        };
        if(ss >> s) {
            Assert(s == "where");
            Assert(bool(ss >> s));
            auto cond = hash_table.find(s); //where的条件列
            if(cond == hash_table.end())
                throw NameException(string("Error: ") + s + string(" doesn't exist."));
            auto cond_type = get<1>(cond->second);
            auto cond_pos = get<0>(cond->second);
            Assert(bool(ss >> op));
            Assert(op == "<" || op == "=" || op == ">");
            Assert(bool(ss >> val));
            for(auto &t : l) {
                if((cond_type == data_type::INT && compare(t->at_int(cond_pos), op, stoi(val))) ||
                    (cond_type == data_type::STRING && compare(t->at_string(cond_pos), op, val)))
                    output(t, sel_type, sel_pos);
            }
        } else {
            for(auto &t : l)
                output(t, sel_type, sel_pos);
        }
    }
};
using table_ptr = Table*;

class SplaySql { // 数据库，内包含多张表
private:
    string database_name;
    struct _SplaySql {
        string name; // 表的名字
        table_ptr p;
        _SplaySql(const string &name = "", table_ptr p = nullptr) noexcept {
            this->name = name;
            this->p = p;
        }
    };
    dlist<_SplaySql> l;

public:
    SplaySql() {}
    SplaySql(const string& name) noexcept : database_name(name) {}
    ~SplaySql() {
        for(auto &t : l)
            delete t.p;
    }
    auto find(const string& name) {
        for(const auto& t : l)
            if(t.name == name)
                return t.p;
        return table_ptr(nullptr);   
    }
    void create() { //未读入数据
        table_ptr p = new Table;
        string s;
        try {
            cin >> s; 
            Assert(s == "table");
            cin >> s;
            for(const auto& t : l)
                if(t.name == s)
                    throw NameException(string("Error: ") + s + string(" already exists."));
            p->create(); 
            l.emplace_back(s, p);                     
        // } catch(invalid_argument &e) { //冗余代码？
        //     cout << e.what() << endl;
        // } catch(NameException &e) {
        //     cout << e.what() << endl;
        } catch(exception &e) {
            delete p;
            cout << e.what() << endl;
        }
    }
    void select() {
        string s1, s2, input;
        getline(cin, input);
        stringstream ss(input); //构造stringstream
        Assert(bool(ss >> s1)); //读入需要select的列
        Assert(bool(ss >> s2) && s2 == "from");
        Assert(bool(ss >> s2)); //读入表的名字
        auto p = find(s2);
        if(p != nullptr)
            p->select(s1, ss);
        else {
            cout << "Error: " << s2 << " doesn't exist." << endl;
            //throw NameException(string("Error: ") + s2 + string(" doesn't exist."));
        }

        // bool find_flag = false;
        // for(const auto &t : l) {
        //     if(t.name == s2) {
        //         find_flag = true;
        //         t.p->select(s1, ss);
        //         break;
        //     }
        // }
        // if(!find_flag) {
        //     cout << "Error: " << s2 << " doesn't exist." << endl;
        //     //throw NameException(string("Error: ") + s2 + string(" doesn't exist."));
        // }
    }
    void insert() {
        string s;
        cin >> s;
        auto p = find(s);
        if(p != nullptr)
            p->insert();
        else
            throw NameException(string("Error: ") + s + string(" doesn't exist."));
    }
    void drop() {
        string s;
        cin >> s;
        Assert(s == "table");
        cin >> s;
        for(auto it = l.begin(); it != l.end(); ++it) {
            auto data = it->data;
            if(data.name == s) {
                delete data.p;
                l.pop(it);
                return;
            }
        }
        throw NameException(string("Error: table ") + s + string(" doesn't exist."));
    }
    void start() {
        string cmd;
        while(true) {
            try {
                cin >> cmd;
                if(cmd == "create")
                    this->create();
                else if(cmd == "insert")
                    this->insert();
                else if(cmd == "select")
                    this->select();
                else if(cmd == "drop")
                    this->drop();
                else if(cmd == "exit") {
                    cout << "Exit " << database_name << "." << endl;
                    return;
                } else {
                    cout << "Error: Not found command." << endl;
                }
            } catch(exception &e) {
                cout << e.what() << endl;
            }
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
        }
    }
};
using splaysql_ptr = SplaySql*;

class DataBase { // 数据库总体，内包含多个数据库
private:
    struct _DataBase {
        string name; // 数据库的名字
        splaysql_ptr p;
        _DataBase() noexcept : name(string("")), p(nullptr) {}
        _DataBase(const string& _name, splaysql_ptr _p) noexcept : name(_name), p(_p) {} 
    };
    dlist<_DataBase> l;

public:
    ~DataBase() {
        for(auto& t : l)
            delete t.p;
    }

    void create() {
        string s;
        cin >> s;
        for(const auto& t : l) {
            if(t.name == s) {
                cout << "Error: " << s << " already exists." << endl;
                return;
            }
        }
        splaysql_ptr p = new SplaySql(s);
        l.emplace_back(s, p);
    }
    void use() {
        string s;
        cin >> s;
        for(const auto& t : l) {
            if(t.name == s) {
                t.p->start();
                return;
            }
        }
        cout << "Error: Not found database " << s << "." << endl;
    }
    void drop() {
        string s;
        cin >> s;
        for(auto it = l.begin(); it != l.end(); ++it) {
            auto data = it->data;
            if(data.name == s) {
                delete data.p;
                l.pop(it);
                return;
            }
        }
        cout << "Error: Not found database " << s << "." << endl;
    }
    void start() {
        cout << "SplaySql start..." << endl;
        string cmd;
        while(true) {
            cin >> cmd;
            if(cmd == "create")
                this->create();
            else if(cmd == "use")
                this->use();
            else if(cmd == "drop")
                this->drop();
            else if(cmd == "exit") {
                cin.ignore(numeric_limits<streamsize>::max(),'\n');
                return;
            } else {
                cout << "Error: Not found command." << endl;
            }
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
        }
    }
}db;

int main() {
    //ios::sync_with_stdio(false); 
    //cin.tie(nullptr);
    db.start();
    // string s("test_database");
    // db.create(s);
    // s = "cplusplus";
    // db.create(s);
    // db.use(string("test_database"));
    // db.use(string("ac"));
    // db.create(s);
    return 0;
}
