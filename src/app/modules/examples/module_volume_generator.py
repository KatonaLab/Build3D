import a3dc_module_interface as a3
import numpy as np
from scipy.ndimage.filters import gaussian_filter


def module_main(ctx):
    w = a3.inputs['width']
    h = a3.inputs['height']
    d = a3.inputs['depth']
    seed = a3.inputs['seed']
    np.random.seed(seed)

    vol = np.random.rand(w, h, d)

    print('your volume is ready! 🍻')
    a3.outputs['volume'] = a3.MultiDimImageFloat_from_ndarray(vol)


inputs = [
    a3.Arg('width', a3.types.uint16, 'parameter'),
    a3.Arg('height', a3.types.uint16, 'parameter'),
    a3.Arg('depth', a3.types.uint16, 'parameter'),
    a3.Arg('seed', a3.types.uint16, 'parameter')]

# NOTE: new param/config proposal
config = [
    a3.Parameter('param name', a3.types.Int32)
    ]
    # a3.Parameter('int value', a3.types.Int32(min=3, max=15, default=7)),
    # a3.Input('input', a3.types.ImageFloat()),
    # a3.Output('output', a3.types.ImageUInt32()),
    # a3.RangeParameter('intensity range', a3.types.In32()),
    # a3.Parameter('filename', a3.types.FilePath(allowed_extensions=['tiff','tif'], multi_select=False, directory_select=False, default='/var/tmp')),
    # a3.Parameter('text', a3.types.String(default='hello'),
    # ]

# NOTE: the parameters should have a corresponding ParameterHelperModule,
# the min/max/default/etc things should be stored there and this module
# should be handed over to qml when rendering/representing the parameter

outputs = [a3.Arg('volume', a3.types.ImageFloat)]

a3.def_process_module(inputs, outputs, module_main)


