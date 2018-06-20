import a3dc_module_interface as a3
from a3dc_module_interface import def_process_module, Arg
from a3dc_module_interface import MultiDimImageFloat_from_ndarray
import numpy as np
from scipy.ndimage.filters import gaussian_filter


def module_main(ctx):
        r = a3.inputs['smooth radius']
        w = a3.inputs['width']
        h = a3.inputs['height']
        d = a3.inputs['depth']
        seed = a3.inputs['seed']
        np.random.seed(seed)

        vol = np.random.rand(w, h, d)
        # vol = gaussian_filter(vol, r)

        print('done making numpy array')
        a3.outputs['volume'] = MultiDimImageFloat_from_ndarray(vol)


inputs = [
    Arg('width', a3.types.uint16, 'parameter'),
    Arg('height', a3.types.uint16, 'parameter'),
    Arg('depth', a3.types.uint16, 'parameter'),
    Arg('seed', a3.types.uint16, 'parameter'),
    Arg('smooth radius', a3.types.uint8, 'parameter')]

outputs = [Arg('volume', a3.types.ImageFloat)]

def_process_module(inputs, outputs, module_main)
