import a3dc
from a3dc import Arg
import numpy as np

def module_main():
    inp = a3dc.inputs['input image']
    level = a3dc.inputs['level']
    for i in range(inp.dims()[2]):
        p = np.array(inp.plane([i]), copy=False)
        inp.set_plane([i], (p > level) * 1.0)
    a3dc.outputs['output image'] = inp
    print("++++++++++++++++ HELLO TEST TRHESHOLD ++++++++++++++++");
    print(inp.dims())

inputs = [Arg('level', a3dc.types.float, 'parameter'),
    Arg('input image', a3dc.types.ImageFloat)]
outputs = [Arg('output image', a3dc.types.ImageFloat)]

a3dc.def_process_module(inputs, outputs, module_main)