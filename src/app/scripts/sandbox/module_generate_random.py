import a3dc
from a3dc import Arg
import numpy as np
from scripts.a3dc_utils import multi_dim_image_apply, multi_dim_image_plane_iterator

def module_main():
    out = a3dc.MultiDimImageFloat([128, 128, 128])
    for plane in multi_dim_image_plane_iterator(out):
        np.copyto(plane, np.random.rand(128, 128))
    a3dc.outputs['output'] = out

inputs = []
outputs = [Arg('output', a3dc.types.ImageFloat)]

a3dc.def_process_module(inputs, outputs, module_main)