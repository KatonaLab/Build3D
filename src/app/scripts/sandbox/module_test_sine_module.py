import a3dc
from a3dc import Arg
import numpy as np

def module_main():
    out = a3dc.MultiDimImageFloat([128, 128, 128])
    fx, fy, fz = a3dc.inputs['freq x'], a3dc.inputs['freq y'], a3dc.inputs['freq z']
    for i in range(out.dims()[2]):
        p = np.array(out.plane([i]), copy=False)
        g = np.mgrid[0:128, 0:128]
        f = 30
        p[:, :] = np.sin(f * g[0, :, :]/128.0 * fx) * np.sin(f * g[1, :, :]/128.0 * fy) * np.sin(f * i/128.0 * fz) * 0.5 + 0.25
    a3dc.outputs['volume'] = out
    print("++++++++++++++++ HELLO SINE GENERATOR ++++++++++++++++");
    print(fx, fy, fz)

inputs = [
    Arg('freq x', a3dc.types.float, 'parameter'),
    Arg('freq y', a3dc.types.float, 'parameter'),
    Arg('freq z', a3dc.types.float, 'parameter')]
outputs = [Arg('volume', a3dc.types.ImageFloat)]

a3dc.def_process_module(inputs, outputs, module_main)