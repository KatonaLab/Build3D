import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.interface import threshold


def make_threshold_module(method):

    def proto_threshold_main():
        im = a3.MultiDimImageFloat_to_ndarray(a3.inputs['input'])
        im = threshold(image=im, method=method)
        a3.outputs['output'] = a3.MultiDimImageFloat_from_ndarray(im)

    inputs = [a3.Arg('input', a3.types.ImageFloat)]
    outputs = [a3.Arg('output', a3.types.ImageFloat)]

    a3.def_process_module(inputs, outputs, proto_threshold_main)
