#include "dlist.h"
#include <bits/stdc++.h>
using namespace std;

#ifndef WIN

#define TERM_RED    "\e[1;31m"
#define TERM_GREEN  "\e[1;32m"
#define TERM_BROWN  "\e[1;33m"
#define TERM_BLUE   "\e[1;34m"
#define TERM_CYAN   "\e[1;36m"
#define TERM_WHITE  "\e[1;37m"
#define TERM_RESET  "\e[0m"

#else

#define TERM_RED    ""
#define TERM_GREEN  ""
#define TERM_BROWN  ""
#define TERM_BLUE   ""
#define TERM_CYAN   ""
#define TERM_WHITE  ""
#define TERM_RESET  ""

#endif

enum class data_type { INT, STRING, ALL }; //增添 ALL 作为枚举是为了 select * 语句

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
    using column_property = tuple<size_t, data_type, bool>; //依次为每一列的存储位置，数据类型，是否为主键（是否建立索引）
    template<typename T>
    struct index_t { //T可以是int或string
        T val;
        data_ptr p;
        inline bool operator<(const index_t&rhs) const {
            return val < rhs.val;
        }
    };
    using int_set_ptr = multiset<index_t<int> >*;
    using string_set_ptr = multiset<index_t<string> >*;
    using callback = void (data_ptr, data_type, size_t, vector<string>&, unordered_map<string, column_property>&);

private:
    unordered_map<string, column_property> hash_table; //记录每一列的属性
    unordered_map<string, int_set_ptr> index_int_table; //int索引的multiset指针集合
    unordered_map<string, string_set_ptr> index_string_table; //string索引的multiset指针集合
    vector<string> column_name; //记录每一列的名字

    string table_name;
    size_t string_cnt = 0, int_cnt = 0;
    dlist<data_ptr> l;

    template<typename set_t, typename val_t>
    void index_search(set_t set_ptr, const string& op, const val_t& val, data_type sel_type, size_t sel_pos, callback output) {
        auto it = set_ptr->begin();
        if(op == "=") it = set_ptr->lower_bound({val, data_ptr(nullptr)});
        if(op == ">") it = set_ptr->upper_bound({val, data_ptr(nullptr)});
        for(; it != set_ptr->end(); it++) {
            if(compare(it->val, op, val))
                output(it->p, sel_type, sel_pos, column_name, hash_table);
            else
                break;
        }
    }
public:
    Table() {}
    ~Table() {
        for(auto &t : l)
            delete t;
        for(auto &it : index_int_table)
            delete it.second;
        for(auto &it : index_string_table)
            delete it.second;
    }
    void create(const string& table_name) { //创建失败的话抛出异常由调用函数回收内存
        this->table_name = table_name;
        string name, type, primary;
        bool end_flag = false, first = true; //第一次需要判断'('
        auto fix = [&]()->void { //使用lamada表达式，解决读入末尾字符处理以及判断是否读入完的问题
            auto last = type[type.size() - 1]; //type的最后一个字符，成功读入后type一定非空，最后一个字符也可以用last.back()表示，下面使用了primary.back()
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
                if(name == "*")
                    throw NameException("Error: '*' is not allowed to be a column name.");
                cin >> type;
                fix();
                if(hash_table.find(name) != hash_table.end())
                    throw NameException(string("Error: ") + name + string(" already exists."));
                hash_table[name] = make_tuple((type == "int" ? int_cnt++ : string_cnt++),
                                              (type == "int" ? data_type::INT : data_type::STRING), 
                                              (primary == "primary" ? true : false));
                column_name.emplace_back(name);
                //to-do,处理主键,建立索引 √
                if(primary == "primary") {
                    primary.clear(); //清除索引标志
                    if(type == "int") {
                        auto set_ptr = new multiset<index_t<int> >;
                        index_int_table[name] = set_ptr;
                    } else {
                        auto set_ptr = new multiset<index_t<string> >;
                        index_string_table[name] = set_ptr;
                    }
                }
                if(end_flag) break;
            }
            column_name.resize(int_cnt + string_cnt); //修正列的宽度，避免内存浪费
            hash_table["*"] = make_tuple(0, data_type::ALL, false);
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
        size_t cnt1 = 0, cnt2 = 0; //cnt1, cnt2分别是int和string的个数
        auto fix = [&]()->data_type {
            auto last = data[data.size() - 1];
            if(last == ')') end_flag = true;
            else Assert(last == ',');
            data.pop_back();
            Assert(data.size() > 0);
            if(data[0] == '\"') {
                Assert(data.size() >= 2);
                Assert(data.back() == '\"');
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
                    ++cnt1;
                    auto val = stoi(data);
                    item->emplace_int(val);
                } else {
                    ++cnt2;
                    item->emplace_string(data);
                }
                if(end_flag) break;
            }
            Assert(cnt1 == int_cnt && cnt2 == string_cnt);
            //应该在插入数据成功后再插入索引，否则不好撤销
            for(size_t i = 0; i < cnt1 + cnt2; ++i) {
                auto name = column_name[i];
                auto [pos, type, is_primary] = hash_table[name]; //结构化解绑定
                if(is_primary == false) continue;
                if(type == data_type::INT) {
                    auto set_ptr = index_int_table[name];
                    set_ptr->insert({item->at_int(pos), item});
                } else {
                    auto set_ptr = index_string_table[name];
                    set_ptr->insert({item->at_string(pos), item});
                }
            }
            l.emplace_back(item);
        } catch(invalid_argument &e) { //冗余代码?
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
        // bool first = true;
        auto sel = hash_table.find(name);
        if(sel == hash_table.end())
            throw NameException(string("Error: ") + name + string(" doesn't exist."));
        auto sel_type = get<1>(sel->second);
        auto sel_pos = get<0>(sel->second);
        auto out_put_first = [&name](const vector<string> &column_name)->void {
            if(name != "*") cout << TERM_CYAN << name << TERM_RESET << endl;
            else {
                bool first = true;
                for(const auto& str : column_name) {
                    if(first) {
                        cout << TERM_CYAN << str;
                        first = false;
                    } else
                        cout << "\t\t" << str;
                }
                cout << TERM_RESET << endl;                    
            }
        };
        auto output = /*[&name, &first]*/[](data_ptr p, data_type type, size_t pos, vector<string> &column_name, unordered_map<string, column_property> &hash_table)->void {
            // if(first) {
            //     first = false;
            //     cout << TERM_CYAN << name << TERM_RESET << endl
            // }
            if(type == data_type::INT) 
                cout << p->at_int(pos) << endl;
            else if(type == data_type::STRING)
                cout << p->at_string(pos) << endl;
            else {
                //难蚌, lambda 函数内访问不到类变量，只能传参进来了
                cout.flags(ios::left);
                for(const auto& str : column_name) {
                    auto [pos, type, is_primary] = hash_table[str];
                    cout << setw(16);
                    if(type == data_type::INT) 
                        cout << p->at_int(pos);
                    else 
                        cout << p->at_string(pos);
                }
                cout << endl;
            }
        };
        
        if(ss >> s) {
            Assert(s == "where");
            Assert(bool(ss >> s)); //s是where的条件列
            auto cond = hash_table.find(s); //where的条件列
            if(cond == hash_table.end())
                throw NameException(string("Error: ") + s + string(" doesn't exist."));
            auto cond_pos = get<0>(cond->second);
            auto cond_type = get<1>(cond->second);
            auto cond_primary = get<2>(cond->second);
            Assert(bool(ss >> op));
            Assert(op == "<" || op == "=" || op == ">");
            Assert(bool(ss >> val));
            if(cond_type == data_type::STRING) {
                Assert(val.size() >= 2);
                Assert(val.front() == '\"' && val.back() == '\"');
                val = val.substr(1, val.size() - 2);
            }
            out_put_first(column_name);
            if(cond_primary == true) {
                if(cond_type == data_type::INT) {
                    auto set_ptr = index_int_table[s];
                    Assert(set_ptr != nullptr);
                    auto ival = stoi(val);
                    index_search(set_ptr, op, ival, sel_type, sel_pos, output);
                } else {
                    auto set_ptr = index_string_table[s];
                    Assert(set_ptr != nullptr);
                    index_search(set_ptr, op, val, sel_type, sel_pos, output);
                }
            } else {
                for(auto &t : l) {
                    if((cond_type == data_type::INT && compare(t->at_int(cond_pos), op, stoi(val))) ||
                        (cond_type == data_type::STRING && compare(t->at_string(cond_pos), op, val)))
                        output(t, sel_type, sel_pos, column_name, hash_table);
                }
            }
        } else {
            out_put_first(column_name);
            for(auto &t : l)
                output(t, sel_type, sel_pos, column_name, hash_table);
        }
        cout << endl;
    }
    void save() { //save理论上说是noexcept的
        bool first = true;
        for(const auto& name : column_name) {
            auto [pos, type, is_primary] = hash_table[name];
            auto type_s = (type == data_type::INT ? "int" : "string");
            if(first) {
                first = false;
                cout << "(" << name << " " << type_s;               
            } else {
                cout << ", " << name << " " << type_s;   
            }
            if(is_primary) cout << " primary";
        }
        cout << ")" << endl;
        for(const auto& data : l) {
            cout << "insert " << table_name << " ";
            first = true;
            for(const auto& name : column_name) {
                auto [pos, type, is_primary] = hash_table[name];
                if(first) {
                    first = false;
                    cout << "values(";
                } else {
                    cout << ", ";
                }
                if(type == data_type::INT)
                    cout << data->at_int(pos);
                else
                    cout << '\"' << data->at_string(pos) << '\"';
            }
            cout << ")" << endl;                
        }
    }
};
using table_ptr = Table*;

class iSTLsql { // 数据库，内包含多张表
private:
    string database_name;
    struct _iSTLsql {
        string name; // 表的名字
        table_ptr p;
        _iSTLsql(const string &name = "", table_ptr p = nullptr) noexcept {
            this->name = name;
            this->p = p;
        }
    };
    dlist<_iSTLsql> l;

public:
    iSTLsql() {}
    iSTLsql(const string& name) noexcept : database_name(name) {}
    ~iSTLsql() {
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
            p->create(s); 
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
    void show() {
        string s;
        cin >> s;
        Assert(s == "tables");
        cout << TERM_BLUE << "tables" << TERM_RESET << endl;
        for(const auto& t : l)
            cout << t.name << endl;
        cout << endl;
    }
    void save() {
        for(const auto& t : l) {
            cout << "create table " << t.name << ' ';
            t.p->save();
        }
    }
    void start(bool read_data = false) {
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
                else if(cmd == "show")
                    this->show();
                else if(cmd == "drop")
                    this->drop();
                else if(cmd == "exit") {
                    if(!read_data) cout << TERM_GREEN << "exit " << database_name << TERM_RESET << endl;
                    return;
                } else {
                    cout << "Error: Not found command." << endl;
                }
            } catch(exception &e) {
                cout << e.what() << endl;
            }
            #ifndef DEBUG
            if(read_data == false) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(),'\n');
            }
            #endif
        }
    }
};
using iSTLsql_ptr = iSTLsql*;

class DataBase { // 数据库总体，内包含多个数据库
private:
    struct _DataBase {
        string name; // 数据库的名字
        iSTLsql_ptr p;
        _DataBase() noexcept : name(string("")), p(nullptr) {}
        _DataBase(const string& _name, iSTLsql_ptr _p) noexcept : name(_name), p(_p) {} 
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
        iSTLsql_ptr p = new iSTLsql(s);
        l.emplace_back(s, p);
    }
    void use(bool read_data = false) {
        string s;
        cin >> s;
        for(const auto& t : l) {
            if(t.name == s) {
                t.p->start(read_data);
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
    void show() {
        string s;
        cin >> s;
        Assert(s == "databases");
        cout << TERM_BLUE << "Database" << TERM_RESET << endl;
        for(const auto& t : l)
            cout << t.name << endl;
        cout << endl;
    }
    void save() {
        freopen("testdata.in", "w", stdout);
        for(const auto& t : l) {
            cout << "create " << t.name << endl;
            cout << "use " << t.name << endl;
            t.p->save();
            cout << "exit" << endl;   
        }
        cout << "exit" << endl;
        cout << "FILE END FlAG" << endl;
        freopen("/dev/tty", "w", stdout);
    }
    void start(bool read_data = false) {
        if(!read_data) cout << TERM_GREEN << "iSTLsql start..." << TERM_RESET << endl;
        string cmd;
        while(true) {
            cin >> cmd;
            if(cmd == "create")
                this->create();
            else if(cmd == "use")
                this->use(read_data);
            else if(cmd == "show")
                this->show();
            else if(cmd == "drop")
                this->drop();
            else if(cmd == "exit") {
                this->save();
                #ifndef DEBUG
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(),'\n');
                #endif
                return;
            } else {
                cout << "Error: Not found command." << endl;
            }
            #ifndef DEBUG
            if(read_data == false) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(),'\n');
            }
            #endif
        }
    }
    void init() {
        freopen("testdata.in", "r", stdin);
        this->start(true);
        freopen("/dev/tty", "r", stdin);
        cout << TERM_GREEN << "Read data done." << TERM_RESET << endl;
        cout << "=============================================" << endl;
        cout << endl;
        //cin.clear();
        //cin.ignore(numeric_limits<streamsize>::max(),'\n');
        //fflush(stdin);
    }
}db;

int main() {
    // freopen("testdata.in", "r", stdin);
    //ios::sync_with_stdio(false); 
    //cin.tie(nullptr);
    db.init();
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