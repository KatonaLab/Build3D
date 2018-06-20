import a3dc_module_interface as a3


def module_main(ctx):
    level = a3.inputs['level']
    # TODO: this is a naive way, we should not copy the data with
    # MultiDimImageFloat_to_ndarray and MultiDimImageFloat_from_ndarray
    # but do the leveling in-place
    input_image = a3.MultiDimImageFloat_to_ndarray(a3.inputs['input'])
    bin_image = (input_image >= level) * 1.0
    a3.outputs['binary volume'] = a3.MultiDimImageFloat_from_ndarray(bin_image)
    print('binarization complete 🍰')


inputs = [a3.Arg('input', a3.types.ImageFloat),
    a3.Arg('level', a3.types.float, 'parameter')]

outputs = [a3.Arg('binary volume', a3.types.ImageFloat)]

a3.def_process_module(inputs, outputs, module_main)
