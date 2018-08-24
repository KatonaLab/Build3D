from a3dc_module_interface import def_process_module
import sys


def module_main(ctx):
    print('Hello Module! Module name: {}'.format(ctx.name()))
    print('your Python interpreter is {}'.format(sys.executable))
    print('of version {}'.format(sys.version))
    print('using search paths for modules {}'.format(sys.path))
    print('on platform {}'.format(sys.platform))
    print('Bye Module!')


def_process_module([], module_main)
