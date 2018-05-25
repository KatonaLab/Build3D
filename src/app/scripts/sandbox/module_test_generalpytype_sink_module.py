import a3dc
from a3dc import Arg
import numpy as np


def module_main():
    inp = a3dc.inputs['py dict input']
    print('general py type sink: ' + str(inp))


inputs = [Arg('py dict input', a3dc.types.GeneralPyType)]
outputs = []

a3dc.def_process_module(inputs, outputs, module_main)