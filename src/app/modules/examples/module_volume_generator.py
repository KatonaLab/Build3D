import a3dc_module_interface as a3
import numpy as np
from scipy.ndimage.filters import gaussian_filter


def module_main():
    r = a3.inputs['smooth radius']
    w = a3.inputs['width']
    h = a3.inputs['height']
    d = a3.inputs['depth']
    seed = a3.inputs['seed']
    np.random.seed(seed)

    vol = np.random.rand(w, h, d)
    vol = gaussian_filter(vol, r)

    print('your volume is ready! 🍻')
    a3.outputs['volume'] = a3.MultiDimImageFloat_from_ndarray(vol)


inputs = [
    a3.Arg('width', a3.types.uint16, 'parameter'),
    a3.Arg('height', a3.types.uint16, 'parameter'),
    a3.Arg('depth', a3.types.uint16, 'parameter'),
    a3.Arg('seed', a3.types.uint16, 'parameter'),
    a3.Arg('smooth radius', a3.types.uint8, 'parameter')]

outputs = [a3.Arg('volume', a3.types.ImageFloat)]

a3.def_process_module(inputs, outputs, module_main)
