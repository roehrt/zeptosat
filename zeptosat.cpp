#include <bits/stdc++.h>
#include <cassert>
#define rep(i,a,b) for (int i = (a); i < (b); ++i)
#define all(x) begin(x), end(x)
#define sz(x) (int)(x).size()
#define eb emplace_back

using namespace std;
using vi = vector<int>;
using vvi = vector<vector<int>>;
using pii = pair<int, int>;

struct SAT {
    int n, m = 0, level = -1, conf = 0;
    vvi clauses; vector<vector<pii>> watch;
    vi val, tr, heu, lvl, rsn, dt, units;
    priority_queue<pii> pool;
    SAT(int n) : n(n), watch(2*n), val(2*n), heu(n), lvl(n), rsn(n) {}
    void add(vi c, int d = INT_MAX) {
        if (d == INT_MAX) for (int& x : c) x = max(2*~x, 2*x+1);
        ranges::sort(c, {}, [&](int x) { return pii(-lvl[x / 2], x); });
        c.erase(unique(all(c)), end(c));
        if (!sz(c)) conf = 1;
        else if (sz(c) == 1) units.eb(c[0]);
        else if (ranges::adjacent_find(c, [&](int a, int b) { return a/2 == b/2; }) == end(c)) {
            rep(i,0,2) watch[c[i^1]].eb(m, sz(c) == 2 ? ~c[i] : c[i]);
            for (int x : c) ++heu[x / 2], val[x&~1] += (level<0)*(1-2*(x&1));
            c.insert(begin(c), 1), clauses.eb(c), dt.eb(d);
            if (++m % 256 == 0 && sz(tr)) for (int& x:heu) x /= 2;
        }
    }
    void propagate(int l, int r = -2) {
        if (val[l] > 0 || val[l] < 0 && (conf = ~r)) return;
        val[l] = 2, val[l ^ 1] = -2;
        tr.eb(l);
        rsn[l / 2] = r, lvl[l / 2] = level;
        erase_if(watch[l^1], [&](auto &x) {
            auto &[c, bl] = x;
            if (!conf && bl < 0 && val[~bl] <= 0) propagate(~bl, c);
            if (conf || bl < 0 || val[bl] > 0) return false;
            auto &cc = clauses[c];
            if (sz(cc) > 16 && m - dt[c] > 256) return true;
            if (cc[1] ^ l ^ 1) swap(cc[1], cc[2]);
            int &i = cc[0];
            rep(_,2,sz(cc)) {
                if (++i == sz(cc)) i = 2;
                if (!val[bl = cc[i]] && i > 2) {
                    watch[bl].eb(c, cc[2]);
                    swap(cc[1], cc[i]);
                    return true;
                }
                if (val[bl] > 0) return false;
            }
            propagate(cc[2], c);
            return false;
        });
    }
    optional<vi> solve() {
        if (conf) return {};
        tr.clear(), pool = {};
        rep(i,0,n) pool.emplace(heu[i], 2*i + (val[2*i] < 0));
        val.assign(2*n, level = 0);
        for (int u: units) propagate(u);
        if (conf) return {};
        for (; sz(tr) < n; ++level) {
            while (val[pool.top().second]) pool.pop();
            if (level) propagate(pool.top().second);
            while (conf) {
                if (!level) return {};
                int i = sz(tr), v = ~conf, bck = conf = 0;
                vi learn;
#define traverse(w) for (int x : clauses[w] | views::drop(1)) \
                    if (int u=x/2; lvl[u] == level) lvl[u] = ~level, ++conf; \
                    else if (lvl[u] > 0) learn.eb(x), bck=max(bck,lvl[u]);
                traverse(v)
                for (;;) {
                    while (lvl[v = tr[--i] / 2] >= 0);
                    if (!--conf) break;
                    traverse(rsn[v])
                    ++heu[v], lvl[v] ^= -1;
                }
                learn.eb(tr[i]^1);
                lvl[v] = ~lvl[v], level=bck;
                for (int b; sz(tr) && lvl[(b = tr.back()) / 2] > level; tr.pop_back()) {
                    val[b] = val[b^1] = 0;
                    pool.emplace(heu[b/2], b);
                }
                propagate(learn.back(), m);
                add(learn, m);
            }
        }
        rep(i,0,n) lvl[i] = val[2*i] < 0;
        return lvl;
    }
    // === (optional) cardinality encoding ===
    int addVar() {
        rep(i,0,2) val.eb(), watch.eb();
        lvl.eb(), rsn.eb(), heu.eb();
        return n++;
    }
    vi counter(vi a, int k) {
        vvi s(sz(a), vi(k));
        rep(i,0,sz(a)) rep(j,0,k) {
            s[i][j] = addVar();
            if (i && (add({~s[i-1][j], s[i][j]}), j))
                add({~a[i], ~s[i-1][j-1], s[i][j]});
        }
        rep(i,0,sz(a)) add({~a[i], s[i][0]});
        return s.back();
    }
    void at_most_k(vi a, int k) {
        if (k < 0) add({});
        else if (k < sz(a)) add({~counter(a, k+1)[k]});
    }
};
