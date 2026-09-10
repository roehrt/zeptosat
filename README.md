# Zeptosat

This repository contains Zeptosat, a header-only, dependency-free, incremental SAT solver written in, say, questionable,
but short, C++20 (~80 lines of code).
Features include Variable State Independent Decaying Sum (VSIDS) branching, phase saving, two-watched literals, blocking literals, inlined binary clauses, and clause deletion.

Excluding our short ICPC template and optional code for cardinality constraints, Zeptosat is only ~80 lines of code.
Apart from the AI-generated fuzzer in `fuzz.py`, everything is handwritten.

### Quickstart

#### Usage as a library

Copy zeptosat.cpp into your project. Then,
```cpp
SAT pizza(4);
int mushrooms = 0, pineapple = 1, mozzarella = 2, pepperoni = 3;

pizza.add({mushrooms, pineapple});
pizza.add({~mushrooms, mozzarella, pepperoni});
pizza.add({~pineapple, mushrooms, pepperoni});
pizza.add({~mozzarella, pineapple});
pizza.add({~pepperoni, mozzarella});
pizza.add({~mushrooms, ~pineapple, ~pepperoni});

auto ans = pizza.solve();
if (!ans) cout << "No pizza possible :(\n";
else {
    auto &toppings = *ans;
    cout << "Order a pizza with:\n";
    if (toppings[mushrooms])  cout << "- Mushrooms\n";
    if (toppings[pineapple])  cout << "- Pineapple\n";
    if (toppings[mozzarella]) cout << "- Mozzarella\n";
    if (toppings[pepperoni])  cout << "- Pepperoni\n";
}
```
The solver is also incremental, meaning you can continue adding constraints and then call `solve()` again—[^1]Zeptosat
will reuse information it learned from the previous calls to `solve()`.

Note that the solver only works when compiled with `g++`, as it makes use of [`g++`'s policy-based priority queue](https://gcc.gnu.org/onlinedocs/libstdc++/manual/policy_data_structures.html#pbds.intro.motivation.priority_queue).

#### Usage as a standalone solver

`main.cpp` contains a small standalone DIMACS CNF parser. Copy and compile it together with `zeptosat.cpp` and provide your CNF
either as a file passed as the first argument to the executable or via standard input, i.e.:

```shell
g++ -std=c++20 -O3 main.cpp -o zeptosat
./zeptosat example.cnf
./zeptosat < example.cnf
```

### Motivation behind Zeptosat

Due to being header-only, Zeptosat is intended to be used in programming competitions.
Zeptosat is so small that it is part of HPI's ICPC team reference document, a printed text document, which can be used during
ICPC competitions to manually copy code.

For the purpose of programming competitions, there is also [`togasat`](https://github.com/togatoga/togasat), which is
also header-only, but not quite as compressed with ~520 lines of code.

### Performance

Publishing accurate and fair benchmarks is hard. One should always take them with a grain of salt.
That being said, from my experience so far, I can say: _it is not bad (for its size)._
It punches above its weight and is at the very least not worse than its "competitor" `togasat`.
On a few instances it even outperformed `minisat`, a proper SAT solver.

### Why is it called Zeptosat?

Well, there are already `minisat`[^2] and `picosat`[^3], which
claim to be small, but in comparison to Zeptosat, really aren't.

Obviously, the logical consequence is to further descend the list of SI prefixes, but as it turns out Femtosat and
Attosat name a class of very small satellites[^4]. So zepto just was the next smaller SI prefix.
However, one could of course also argue that the jump in size is so significant that it justifies skipping two prefixes.

### Contributing

Found a bug, or have an idea to reduce the code size even further? Please open an issue.

[^1]: Set by a human.

[^2]: http://minisat.se/

[^3]: https://fmv.jku.at/picosat/

[^4]: [Femtosatellites (Wikipedia)](https://en.wikipedia.org/wiki/Small_satellite#Femtosatellites)