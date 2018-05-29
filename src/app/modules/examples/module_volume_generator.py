import a3dc_module_interface as a3
from a3dc_module_interface import def_process_module, Arg
from a3dc_module_interface import MultiDimImageFloat_from_ndarray
import numpy as np
from scipy.ndimage.filters import gaussian_filter


def make_ball(coord_im, pos, radius):
    return ((coord_im - np.array(pos)) ** 2 < radius ** 2) * 1.0


def module_main():
        n = a3.inputs['ball count']
        w = a3.inputs['width']
        h = a3.inputs['height']
        d = a3.inputs['depth']
        seed = a3.inputs['seed']
        min_dim = np.min(np.array([w, h, d]))

        np.random.seed(seed)
        vol = np.zeros((w, h, d))
        coords = np.reshape(np.mgrid[0:w, 0:h, 0:d], (w, h, d, -1))
        scaler = np.array([w, h, d, min_dim, 1])
        for _ in range(n):
            x, y, z, r, f = np.random.rand(5) * scaler
            vol = np.maximum(vol, f * make_ball(coords, (x, y, z), r))

        vol = gaussian_filter(vol, np.maximum(min_dim * 0.5, 16))
        outputs['volume'] = MultiDimImageFloat_from_ndarray(vol)


inputs = [
    Arg('width', a3.types.uint16, 'parameter'),
    Arg('height', a3.types.uint16, 'parameter'),
    Arg('depth', a3.types.uint16, 'parameter'),
    Arg('seed', a3.types.uint16, 'parameter'),
    Arg('ball count', a3.types.uint8, 'parameter')]

outputs = [Arg('volume', a3.types.ImageFloat)]

def_process_module(inputs, outputs, module_main)
