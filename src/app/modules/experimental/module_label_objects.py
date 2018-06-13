import a3dc_module_interface as a3
from scipy.ndimage.measurements import label
import numpy as np

def module_main():
    input_image = a3.MultiDimImageFloat_to_ndarray(a3.inputs['binary volume'])
    structure = np.array([
        [[0,0,0],[0,1,0],[0,0,0]],
        [[0,1,0],[1,1,1],[0,1,0]],
        [[0,0,0],[0,1,0],[0,0,0]]])
    labeled_image, _ = label(input_image, structure)
    print(labeled_image)
    a3.outputs['labeled volume'] = a3.MultiDimImageUInt32_from_ndarray(labeled_image)
    print('object labeling complete 🔖')


inputs = [a3.Arg('binary volume', a3.types.ImageFloat)]

outputs = [a3.Arg('labeled volume', a3.types.ImageUInt32)]

a3.def_process_module(inputs, outputs, module_main)
