import a3dc_module_interface as a3
from scipy.ndimage.measurements import labeled_comprehension
import numpy as np


def module_main():
    label_pairs = a3.inputs['label pair list']
    intensity_image = a3.MultiDimImageFloat_to_ndarray(a3.inputs['intensity image'])
    labeled_image = a3.MultiDimImageUInt32_to_ndarray(a3.inputs['labeled'])

    ids_1 = label_pairs[:, 0]
    cnt_1 = labeled_comprehension(intensity_image, labeled_image, ids_1, len, int, -1)
    cnt_1 /= np.max(cnt_1)

    a3.outputs['voxel size value'] = a3.MultiDimImageFloat_from_ndarray(cnt_1)
    print('labeled intersections are ready 🍀')


inputs = [a3.Arg('label pair list', a3.types.GeneralPyType),
          a3.Arg('intensity image', a3.types.ImageFloat),
          a3.Arg('labeled', a3.types.ImageUInt32)]

outputs = [a3.Arg('voxel size value', a3.types.ImageFloat)]

a3.def_process_module(inputs, outputs, module_main)
