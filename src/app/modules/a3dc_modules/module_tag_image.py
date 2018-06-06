import a3dc_module_interface as a3if
import a3dc.interface


def module_main():
    im = a3if.MultiDimImageFloat_to_ndarray(a3if.inputs['input'])
    im = a3dc.interface.tagImage(im)
    a3if.outputs['output'] = a3if.MultiDimImageFloat_from_ndarray(im)


inputs = [a3if.Arg('input', a3if.types.ImageFloat)]
outputs = [a3if.Arg('output', a3if.types.ImageFloat)]

a3if.def_process_module(inputs, outputs, module_main)
