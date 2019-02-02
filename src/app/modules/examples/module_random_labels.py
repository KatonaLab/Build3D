import a3dc_module_interface as a3
import numpy as np


def module_main(ctx):
    w = a3.inputs['width']
    h = a3.inputs['height']
    d = a3.inputs['depth']
    seed = a3.inputs['seed']
    nlabels = a3.inputs['number of labels']
    np.random.seed(seed)

    vol = np.random.randint(nlabels, size=(w, h, d)).astype(np.uint32)

    a3.outputs['labels'] = a3.MultiDimImageUInt32_from_ndarray(vol)
    print('your labeled volume is ready!🎲')


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
    a3.Parameter('number of labels', a3.types.uint16).setIntHint('default', 5),
    a3.Output('labels', a3.types.ImageUInt32)]

a3.def_process_module(config, module_main)
