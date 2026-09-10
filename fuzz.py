# ============
# AI-generated
# ============

import argparse, random, subprocess, os

TEMP_FILE = "fuzz.cnf"

def gen_cnf(nv):
    nc = int(nv * random.uniform(3.0, 6.0))
    clauses = []
    for _ in range(nc):
        width = random.randint(2, 5)
        clause = []
        for _ in range(width):
            var = random.randint(1, nv)
            clause.append(-var if random.random() < 0.5 else var)
        clauses.append(clause)
    return clauses

def to_dimacs(fname, nv, clauses):
    with open(fname, "w") as f:
        f.write(f"p cnf {nv} {len(clauses)}\n")
        for c in clauses:
            f.write(f"{' '.join(map(str, c))} 0\n")

def run(cmd, fname):
    try:
        r = subprocess.run([cmd, fname], capture_output=True, text=True, timeout=2)
    except subprocess.TimeoutExpired:
        return "TIMEOUT"
    except OSError:
        return "ERR"
    if r.returncode < 0:
        return "CRASH"
    results = {line.strip() for line in r.stdout.splitlines()
               if line.strip() in ("s SATISFIABLE", "s UNSATISFIABLE")}
    if len(results) != 1:
        return "ERR"
    result = "SAT" if results.pop() == "s SATISFIABLE" else "UNSAT"
    if r.returncode not in (0, 10 if result == "SAT" else 20):
        return "ERR"
    return result

def check(clauses, nv, t_bin, r_bin):
    to_dimacs(TEMP_FILE, nv, clauses)
    rt, rr = run(t_bin, TEMP_FILE), run(r_bin, TEMP_FILE)
    if rr not in ("SAT", "UNSAT"):
        raise RuntimeError(f"Reference solver failed: {rr}; input saved in {TEMP_FILE}")
    is_bug = rt != rr
    return is_bug, rt, rr

def delta_debug(clauses, nv, t_bin, r_bin):
    if len(clauses) <= 1: return clauses
    print(f"\r    > Minimizing: {len(clauses):<5} clauses...", end="", flush=True)

    mid = len(clauses) // 2
    for part in [clauses[:mid], clauses[mid:]]:
        bug, _, _ = check(part, nv, t_bin, r_bin)
        if bug: return delta_debug(part, nv, t_bin, r_bin)
    return clauses

def main():
    p = argparse.ArgumentParser()
    p.add_argument("target")
    p.add_argument("--ref", default="kissat")
    args = p.parse_args()

    print(f"[*] Fuzzing {os.path.basename(args.target)} vs {os.path.basename(args.ref)}")

    i = 0
    while True:
        i += 1
        nv = random.randint(10, 50)
        clauses = gen_cnf(nv)
        nc = len(clauses)

        bug, rt, rr = check(clauses, nv, args.target, args.ref)

        print(f"\rIter: {i:<6} | V: {nv:<3} C: {nc:<4} | Target: {rt:<5} Ref: {rr:<5}", end="", flush=True)

        if bug:
            print(f"\n\n[!] BUG FOUND!")
            print(f"    Target: {rt}")
            print(f"    Ref:    {rr}")

            min_clauses = delta_debug(clauses, nv, args.target, args.ref)
            to_dimacs(f"bug_{i}.cnf", nv, min_clauses)

            print(f"\n    Saved reduced case to bug_{i}.cnf ({len(min_clauses)} clauses)")
            break

if __name__ == "__main__":
    try:
        main()
    except RuntimeError as error:
        print(f"\n[!] {error}")
        raise SystemExit(1)
    except KeyboardInterrupt:
        print("\n[!] Stopped.")
