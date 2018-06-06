import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.interface import threshold
from modules.a3dc_modules.a3dc.imageclass import Image
import numpy as np


def make_threshold_module(method):

    def proto_threshold_main():
        im = a3.MultiDimImageFloat_to_ndarray(a3.inputs['input'])
        a3dc_image = Image(im, {'Name': 'random_name', 'Type': np.float})

        a3dc_out, log_text = threshold(image=a3dc_image, method=method)
        print(log_text)

        a3.outputs['output'] = a3.MultiDimImageFloat_from_ndarray(a3dc_out.array)

    inputs = [a3.Arg('input', a3.types.ImageFloat)]
    outputs = [a3.Arg('output', a3.types.ImageFloat)]

    a3.def_process_module(inputs, outputs, proto_threshold_main)
