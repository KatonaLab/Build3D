import a3dc_module_interface as a3
import numpy as np

def module_main():
    input_A = a3.MultiDimImageFloat_to_ndarray(a3.inputs['labeled volume A'])
    input_B = a3.MultiDimImageFloat_to_ndarray(a3.inputs['labeled volume B'])

    n_labels_A = np.max(input_A)
    n_labels_B = np.max(input_B)

    a3.outputs['labeled volume'] = a3.MultiDimImageFloat_from_ndarray(labeled_image)
    print('object labeling complete 🔖')


inputs = [a3.Arg('labeled volume A', a3.types.ImageUInt32),
    a3.Arg('labeled volume B', a3.types.ImageUInt32)]

outputs = [a3.Arg('A intersect B', a3.types.ImageUInt32),
    a3.Arg('A \ B', a3.types.ImageUInt32),
    a3.Arg('B \ A', a3.types.ImageUInt32)]

a3.def_process_module(inputs, outputs, module_main)
