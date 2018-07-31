from subprocess import Popen, PIPE
import sys
import pandas as pd
import numpy as np
import platform
from bokeh.plotting import figure, output_file, show
from bokeh.models.widgets import Panel, Tabs
from bokeh.palettes import d3
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


def contains_any_keyword(text, keywords_list):
    return any([k in text for k in keywords_list])


def remove_hash_tags(text):
    return ' '.join(filter(lambda x: x[0] != '#', text.split()))


def draw_plot(p, current_dt, unq_names, relative_scale=False):
    colors = itertools.cycle(d3['Category20'][20])
    t_index = {
        ts: i for i, ts in enumerate(
            sorted(current_dt['timestamp'].unique()))}

    for bm, c in zip(unq_names, colors):
        bm_dt = current_dt[current_dt['name'] == bm]
        x = [t_index[ts] for ts in bm_dt['timestamp'].values]
        y = bm_dt['runtime'].values
        if relative_scale:
            y = y / np.max(y)
        p.line(x, y, legend=bm, color=c)
        p.circle(x, y, legend=bm, color=c, size=6)

    p.legend.location = 'top_left'
    p.legend.click_policy = 'hide'


# get filtering keywords
keywords = sys.argv[1:]
print('keywords: {}'.format(' '.join(keywords)))

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

unq_names = current_dt['name'].unique()

if len(keywords) > 0:
    unq_names = [name for name in unq_names
                 if contains_any_keyword(name, keywords)]

unq_names = [remove_hash_tags(name) for name in unq_names]

# canonic names
current_dt['name'] = current_dt['name'].apply(remove_hash_tags)

output_file('benchmark.html')
plot_title = 'benchmark on branch \'{}\' and platform \'{}\''.format(
    branch, platform.platform())

tabs = []
for plot_type, param in [('absolute', False), ('relative', True)]:
    p = figure(title=plot_title + ' ' + plot_type,
               x_axis_label='measurement',
               y_axis_label='%' if param else 'ns/op',
               plot_width=1600,
               plot_height=800)
    draw_plot(p, current_dt, unq_names, relative_scale=param)
    tabs.append(Panel(child=p, title=plot_type))

tabs_fig = Tabs(tabs=[tabs[0], tabs[1]])
show(tabs_fig)
