#!/usr/bin/env python3

import csv
import json
import optparse
import subprocess

parser = optparse.OptionParser()
parser.add_option("--info", help="use line info JSON from file")
(options, args) = parser.parse_args()


def run_scc():
    scc_command = "scc . --exclude-dir=lua,libm --format json"
    o = subprocess.run(scc_command, shell=True, capture_output=True)
    j = json.loads(o.stdout)
    for language in j:
        del language['Files']
    return j


def move_repo(to="HEAD^"):
    git_command = "git checkout " + to
    subprocess.run(git_command, shell=True)


def count_commits():
    git_command = "git rev-list --count HEAD"
    o = subprocess.run(git_command, shell=True, capture_output=True)
    return int(o.stdout)


def get_lines_info():
    counts = {}
    while True:
        depth = count_commits()
        counts[depth] = run_scc()
        if depth == 1:
            print(counts)
            break
        move_repo()
    return counts


def sum_code(dump):
    return sum(l['Code'] for l in dump)


def main():
    counts = {}
    if options.info:
        with open(options.info) as f:
            counts = json.loads(f.read())
    else:
        move_repo("master")
        counts = get_lines_info()
        with open('count.json', 'w') as f:
            f.write(json.dumps(counts))

    with open('count.csv', 'w', newline='') as csvfile:
        w = csv.writer(csvfile)
        for k, languages in counts.items():
            total_code = sum_code(languages)
            w.writerow([k, total_code])


if __name__ == '__main__':
    main()
