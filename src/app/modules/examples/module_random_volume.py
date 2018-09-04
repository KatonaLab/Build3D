import a3dc_module_interface as a3
import numpy as np


def module_main(ctx):
    w = a3.inputs['width']
    h = a3.inputs['height']
    d = a3.inputs['depth']
    seed = a3.inputs['seed']
    np.random.seed(seed)

    vol = np.random.rand(w, h, d)

    print('your volume is ready! 🍻')
    a3.outputs['volume'] = a3.MultiDimImageFloat_from_ndarray(vol)


config = [
    a3.Parameter('width', a3.types.uint16).setIntHint('min', 1)
                                          .setIntHint('max', 2048)
                                          .setIntHint('default', 64),
    a3.Parameter('height', a3.types.uint16).setIntHint('min', 1)
                                           .setIntHint('max', 2048)
                                           .setIntHint('default', 64),
    a3.Parameter('depth', a3.types.uint16).setIntHint('min', 1)
                                          .setIntHint('max', 2048)
                                          .setIntHint('default', 16),
    a3.Parameter('seed', a3.types.uint16).setIntHint('default', 42),
    a3.Output('volume', a3.types.ImageFloat)]

a3.def_process_module(config, module_main)
