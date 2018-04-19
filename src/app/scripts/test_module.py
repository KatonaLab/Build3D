import a3dc
from a3dc import Arg
import numpy as np

def module_main():
    im = a3dc.inputs['in_image']
    param = a3dc.inputs['tunable parameter']
    for i in range(im.dims()[2]):
        p = np.array(im.plane([i]), copy=False)
        p += param
    a3dc.outputs['out_image'] = im

inputs = [
    Arg('in_image', a3dc.types.ImageFloat),
    Arg('tunable parameter', a3dc.types.float, 'parameter')]
outputs = [Arg('out_image', a3dc.types.ImageFloat)]

a3dc.def_process_module(inputs, outputs, module_main)