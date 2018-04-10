import a3dc
import numpy as np

def module_main():
    im = a3dc.inputs['in_image']
    for i in range(im.dims()[2]):
        p = np.array(im.plane([i]), copy=False)
        p += 1
    a3dc.outputs['out_image'] = im

inputs = {'in_image': a3dc.types.ImageUInt8}
outputs = {'out_image': a3dc.types.ImageUInt8}
a3dc.def_process_module(inputs, outputs, module_main)