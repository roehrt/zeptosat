import argparse
import pathlib
import subprocess
import sys

parser = argparse.ArgumentParser(description='Run a test')
parser.add_argument('executable', type=pathlib.Path, help='SAT solver executable')
parser.add_argument('test', type=pathlib.Path, help='DIMACS CNF file')
args = parser.parse_args()

def run(executable, test, check):
    try:
        with open(test) as cnf:
            res = subprocess.run(executable, stdin=cnf, text=True, capture_output=True)
    except OSError as error:
        print(error, file=sys.stderr)
        return None
    results = {line.strip() for line in res.stdout.splitlines()
               if line.strip() in ('s SATISFIABLE', 's UNSATISFIABLE')}
    if len(results) != 1:
        return None
    sat = results.pop() == 's SATISFIABLE'
    expected_code = 0 if check else (10 if sat else 20)
    return sat if res.returncode == expected_code else None

our_ans = run(args.executable, args.test, True)
if our_ans is None:
    print("Error zeptosat failed", file=sys.stderr)
    exit(1)
cadical_ans = run(['cadical', '-q'], args.test, False)
if cadical_ans is None:
    print("Error CaDiCaL failed", file=sys.stderr)
    exit(1)
ok = our_ans == cadical_ans
print(f'{args.test.stem}: {"SAT" if our_ans else "UNSAT"} {"OK" if ok else "FAIL"}', file=sys.stderr)

exit(0 if ok else 1)
