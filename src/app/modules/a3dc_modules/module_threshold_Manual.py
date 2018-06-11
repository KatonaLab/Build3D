import a3dc_module_interface as a3


def module_main():
    level = a3.inputs['level']
    im = a3.MultiDimImageFloat_to_ndarray(a3.inputs['input'])
    bin_image = (im >= level) * 1.0
    a3.outputs['output'] = a3.MultiDimImageFloat_from_ndarray(bin_image)

inputs = [a3.Arg('input', a3.types.ImageFloat),
    a3.Arg('level', a3.types.float, 'parameter')]
outputs = [a3.Arg('output', a3.types.ImageFloat)]

a3.def_process_module(inputs, outputs, module_main)