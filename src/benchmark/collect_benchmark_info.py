from subprocess import Popen, PIPE
from io import StringIO
import os
import os.path
import sys
import pandas as pd
from datetime import datetime
import platform

BENCHMARK_EXECUTABLE = './benchmark'
PERF_CSV_FILENAME = 'perf.csv'

if len(sys.argv) >= 2:
    BENCHMARK_EXECUTABLE = sys.argv[1]

if not os.path.isfile(BENCHMARK_EXECUTABLE):
    print(('\'{}\' does not exist, can not create \'{}\',\n'
           'try specifying the executable:\n\n'
           '\tpython {} <benchmark executable path>\n')
          .format(BENCHMARK_EXECUTABLE,
                  PERF_CSV_FILENAME,
                  sys.argv[0]))
    sys.exit(1)


def produce_new_benchmark():

    process = Popen([BENCHMARK_EXECUTABLE], stdout=PIPE, shell=True)
    print('running ...')

    output = b''
    while True:
        line = process.stdout.readline()
        if not line:
            break
        output += line
        print(line.decode('ascii').rstrip())
    exit_code = 0

    # (output, err) = process.communicate()
    # exit_code = process.wait()
    print('benchmarking is complete')

    if exit_code != 0:
        print(('error while running benchmark, {}\n'
               'try specifying the executable:\n\n'
               '\tpython {} <benchmark executable path>\n')
              .format(BENCHMARK_EXECUTABLE, sys.argv[0]))
        sys.exit(2)

    new_dt = pd.read_csv(StringIO(output.decode('ascii')), delimiter=';')

    n = len(new_dt['name'])
    ts = datetime.now().timestamp()
    git_proc = Popen(['git', 'rev-parse', '--abbrev-ref', 'HEAD'], stdout=PIPE)
    (br, err) = git_proc.communicate()
    exit_code = git_proc.wait()
    br = br.decode('ascii').rstrip()

    if exit_code != 0:
        print('error while running git')
        sys.exit(3)

    new_dt['timestamp'] = pd.Series([ts] * n, index=new_dt.index)
    new_dt['platform'] = pd.Series([platform.platform()] * n,
                                   index=new_dt.index)
    new_dt['git_branch'] = pd.Series([br] * n, index=new_dt.index)

    return new_dt


new_dt = produce_new_benchmark()

if os.path.isfile(PERF_CSV_FILENAME):
    dt = pd.read_csv(PERF_CSV_FILENAME, delimiter=';')
    dt = pd.concat([dt, new_dt], ignore_index=True)
else:
    dt = new_dt

dt.to_csv(PERF_CSV_FILENAME, sep=';', index_label=False)
print('saved to \'{}\''.format(PERF_CSV_FILENAME))
