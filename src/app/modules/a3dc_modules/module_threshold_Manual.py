import a3dc_module_interface as a3


def module_main(ctx):
    level = a3.inputs['level']
    im = a3.MultiDimImageFloat_to_ndarray(a3.inputs['input'])
    bin_image = (im >= level) * 1.0
    a3.outputs['output'] = a3.MultiDimImageFloat_from_ndarray(bin_image)

config = [a3.Input('input', a3.types.ImageFloat),
    a3.Parameter('level', a3.types.float),
    a3.Output('output', a3.types.ImageFloat)]

a3.def_process_module(config, module_main)