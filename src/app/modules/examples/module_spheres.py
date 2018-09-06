import a3dc_module_interface as a3
import numpy as np


def module_main(ctx):
    w = a3.inputs['width']
    h = a3.inputs['height']
    d = a3.inputs['depth']
    x1 = a3.inputs['sphere 1 x']
    y2 = a3.inputs['sphere 2 y']

    c1 = [x1, 0, 0]
    c2 = [0, y2, 0]

    x_ = np.linspace(-w / 2, w / 2, w)
    y_ = np.linspace(-h / 2, h / 2, h)
    z_ = np.linspace(-d / 2, d / 2, d)

    x, y, z = np.meshgrid(x_, y_, z_, indexing='ij')
    sp1 = np.maximum(0, 32 - np.sqrt(np.square(x - c1[0]) + np.square(y - c1[1]) + np.square(z - c1[2])))
    sp2 = np.maximum(0, 32 - np.sqrt(np.square(x - c2[0]) + np.square(y - c2[1]) + np.square(z - c2[2])))

    a3.outputs['sphere 1'] = a3.MultiDimImageFloat_from_ndarray(sp1)
    a3.outputs['sphere 2'] = a3.MultiDimImageFloat_from_ndarray(sp2)


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
    a3.Parameter('sphere 1 x', a3.types.uint16).setIntHint('min', 1)
                                               .setIntHint('max', 2048)
                                               .setIntHint('default', 24),
    a3.Parameter('sphere 2 y', a3.types.uint16).setIntHint('min', 1)
                                               .setIntHint('max', 2048)
                                               .setIntHint('default', 48),
    a3.Output('sphere 1', a3.types.ImageFloat),
    a3.Output('sphere 2', a3.types.ImageFloat)]

a3.def_process_module(config, module_main)
