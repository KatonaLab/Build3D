import a3dc
from a3dc import Arg
import numpy as np
from imageProcessing import autoThreshold

def module_main():
    im = a3dc.inputs['in']
    im = np.array(im.planes())
    for i in range(im.dims()[2]):
        p = np.array(im.plane([i]), copy=False)
        p += param
    a3dc.outputs['out_image'] = im

inputs = [Arg('in', a3dc.types.ImageFloat)]
outputs = [Arg('out', a3dc.types.ImageFloat)]

a3dc.def_process_module(inputs, outputs, module_main)