import a3dc
from a3dc import Arg
import numpy as np


def module_main():
    out = {'first': 1,
        'second': 2,
        'list': [42, 2024],
        'dict': {'name': 'Ford Prefect'},
        'other dict': {'name': 'Agent Deckard'}}
    a3dc.outputs['py dict'] = out
    print('general py type source: ' + str(out))


inputs = []
outputs = [Arg('py dict', a3dc.types.GeneralPyType)]

a3dc.def_process_module(inputs, outputs, module_main)