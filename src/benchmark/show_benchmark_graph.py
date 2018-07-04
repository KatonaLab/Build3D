from subprocess import Popen, PIPE
import sys
import pandas as pd
import platform
from bokeh.plotting import figure, output_file, show, gridplot
from bokeh.palettes import Dark2_5
import itertools
import os.path

PERF_CSV_FILENAME = 'perf.csv'


def git_branch():
    git_proc = Popen(['git', 'rev-parse', '--abbrev-ref', 'HEAD'], stdout=PIPE)
    (br, err) = git_proc.communicate()
    exit_code = git_proc.wait()
    if exit_code != 0:
        print('can not determine current git branch')
        sys.exit(1)
    return br.decode('ascii').rstrip()


if not os.path.isfile(PERF_CSV_FILENAME):
    print(('\'{}\' does not exist, can not create graph,\n'
           'run collect_benchmark_info.py to generate \'{}\'')
          .format(PERF_CSV_FILENAME, PERF_CSV_FILENAME))
    sys.exit(2)

branch = git_branch()
print('git branch = {}\nplatform = {}'.format(branch, platform.platform()))

dt = pd.read_csv(PERF_CSV_FILENAME, delimiter=';')

current_dt = dt[(dt['git_branch'] == branch) &
                (dt['platform'] == platform.platform())]

output_file('benchmark.html')
p = figure(title='benchmark on branch \'{}\' and platform \'{}\''
           .format(branch, platform.platform()),
           x_axis_label='timestamp',
           y_axis_label='ns/op')

colors = itertools.cycle(Dark2_5)

for bm, c in zip(current_dt['name'].unique(), colors):
    bm_dt = current_dt[current_dt['name'] == bm]
    x = bm_dt['timestamp'].values
    y = bm_dt['runtime'].values
    p.line(x, y, legend=bm, color=c)
    p.circle(x, y, legend=bm, color=c, size=6)

fig = gridplot([[p]], sizing_mode='stretch_both', merge_tools=False)
show(fig)
