import a3dc_module_interface as a3
from skimage.filters import threshold_mean

def module_main(ctx):
    level = a3.inputs['level']
    # TODO: this is a naive way, we should not copy the data with
    # MultiDimImageFloat_to_ndarray and MultiDimImageFloat_from_ndarray
    # but do the leveling in-place
    input_image = a3.MultiDimImageFloat_to_ndarray(a3.inputs['input'])

    if a3.inputs['input'].meta.has('type'):
        print('type', a3.inputs['input'].meta.get('type'))

    if a3.inputs['input'].meta.has('normalized'):
        print('normalized', a3.inputs['input'].meta.get('normalized'))

    if a3.inputs['input'].meta.has('path'):
        print('path', a3.inputs['input'].meta.get('path'))

    if a3.inputs['input'].meta.has('channel'):
        print('channel', a3.inputs['input'].meta.get('channel'))

    print(str(a3.inputs['input'].meta))

    bin_image = (input_image >= level) * 1.0
    a3.outputs['binary volume'] = a3.MultiDimImageFloat_from_ndarray(bin_image)
    print('binarization complete 🍰')


config = [a3.Input('input', a3.types.ImageFloat),
    a3.Parameter('level', a3.types.float),
    a3.Output('binary volume', a3.types.ImageFloat)]

a3.def_process_module(config, module_main)
