#include <bits/stdc++.h>
// #include <sys/wait.h>
using namespace std;
using namespace chrono;

mt19937 Rng(random_device{}());
uniform_int_distribution<int> dist(0, 2000000000);
vector<int> Id;
vector<string> Name;

// extern char **environ;
// volatile sig_atomic_t pid;

// char path[] = "./main";
// char *argv[] = {
//     "./main",
//     "<",
//     "./testdata1.in",
//     NULL,
// };

// void hanlder(int sig) {
//     int olderrno = errno;
//     pid = waitpid(-1, NULL, 0);
//     errno = olderrno;
// }

int rng() {
    return dist(Rng);
}

string& rand_string(int len) {
    auto ret_p = new string;
    auto &ret = *ret_p;
    for(int i = 1; i <= len; ++i) {
        auto t = rng();
        ret += 'a' + t % 26;
        if(t % 2) ret.back() = toupper(ret.back());
    }
    return ret;
}

void gen_insert(int n) {
    for(int i = 1; i <= n; ++i) {
        auto id = rng(), salary = rng();
        auto &name = rand_string(25), &job = rand_string(25);
        cout << "insert person values(" << id << ", \"" << name << "\", \"" << job << "\", " << salary << ")\n";
        Id.push_back(id);
        Name.push_back(name);
        delete &name;
        delete &job;
    }
}

void gen_select(int m) {
    sort(Id.begin(), Id.end());
    auto size = Id.size();
    for(int i = 1; i <= m; ++i) {
        auto p = rng() % size;
        if(rng() % 2) cout << "select * from person where id = " << Id[p] << '\n'; 
        else cout << "select * from person where name = \"" << Name[p] << "\"\n";
    }
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        cerr << "Error!" << endl;
        exit(EXIT_FAILURE);
    }
    int n = atoi(argv[1]), m = atoi(argv[2]);
    // cout << argv[0] << " " << argv[1] << endl;
    cerr << "Start to generate input data." << endl;
    ofstream out("testdata1.in");
    streambuf *cout_stream = cout.rdbuf();
    cout.rdbuf(out.rdbuf());
    cout << "create test" << '\n';
    cout << "use test" << '\n';
    cout << "create table person (id int primary, name string primary, job string, salary int)" << '\n';
    cout << "insert person values(12345, \"Bob\", \"sleep\", 2500000)" << '\n';
    gen_insert(n);
    gen_select(m);
    cout << "exit" << '\n';
    cout << "exit" << '\n';
    // cout << "FILE END FlAG" << '\n';
    cout.rdbuf(cout_stream);
    out.close();
    cerr << "Generate data done." << endl;

    // signal(SIGCHLD, hanlder);
    // sigset_t mask, prev;
    // sigemptyset(&mask);
    // sigaddset(&mask, SIGCHLD);
    // sigprocmask(SIG_BLOCK, &mask, &prev);
    
    int res;
    res = system("cp ./testdata.in ./testdata.bak");
    assert(res == 0);
    // cout << "res = " << res << endl;
    auto start = steady_clock::now();
    char cmd[] = "./main < testdata1.in | sed -r \"s/\x1B\\[([0-9]{1,3}((;[0-9]{1,3})*)?)?[m|K]//g\" > res.out";
    res = system(cmd);
    assert(res == 0);
    // if(fork() == 0) {
    //     sigprocmask(SIG_SETMASK, &prev, NULL);
    //     if(execve(path, argv, environ) < 0) {
    //         cerr << "Error!" << endl;
    //         exit(0);
    //     }
    // }
    // while(!pid)
    //     sigsuspend(&prev);
    
    // sigprocmask(SIG_SETMASK, &prev, NULL);
    auto end = steady_clock::now();
    cout << "Time consuming: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;
    res = system("mv ./testdata.bak ./testdata.in");
    assert(res == 0);
    // for(int i = 0; i < 10; ++i)
    //     cout << dist(rng) << endl;
    return 0;
}