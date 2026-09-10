#include "zeptosat.cpp"

int main(int argc, char** argv) {
    cin.tie(0)->sync_with_stdio(0);
    cin.exceptions(cin.failbit);

    if (argc > 1) freopen(argv[1], "r", stdin);

    {
        for (char c; cin >> c && c != 'p';) cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string cnf; cin >> cnf;
    }
    int n, m; cin >> n >> m;
    SAT solver(n);
    vvi clauses;
    rep(i,0,m) {
        vi clause;
        for (int x; cin >> x && x;)
            clause.push_back(x - (x>0));
        solver.add(clause);
        clauses.push_back(clause);
    }
    auto res = solver.solve();
    if (!res) {
        cout << "s UNSATISFIABLE" << endl;
    } else {
        cout << "s SATISFIABLE" << endl;
        cout << "v ";
        rep(i,0,n) cout << ((*res)[i] ? (i+1) : (-i-1)) << ' ';
        cout << "0\n";
        for (vi& clause : clauses) {
            bool sat = false;
            for (int x : clause) {
                if (x >= 0 && (*res)[x] || x < 0 && !(*res)[~x]) {
                    sat = true;
                    break;
                }
            }
            if (!sat) exit(42);
        }
    }

    return 0;
}
