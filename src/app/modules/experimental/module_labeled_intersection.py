import a3dc_module_interface as a3
import numpy as np


def intersect_labels(labeled_1, labeled_2):
    n_1 = np.max(labeled_1) + 1
    intersection_mask = labeled_1 * labeled_2 > 0
    # unique labels for the intersections
    intersection_labels = intersection_mask * (labeled_2 * n_1 + labeled_1)
    ids_1 = labeled_1[intersection_mask].ravel()
    ids_int = intersection_labels[intersection_mask].ravel()
    ids_2 = labeled_2[intersection_mask].ravel()
    # intersecting label triplets
    intersecting_ids = np.unique(np.c_[ids_1, ids_int, ids_2], axis=0)
    return intersecting_ids, intersection_labels


def module_main():
    input_A = a3.MultiDimImageUInt32_to_ndarray(a3.inputs['labeled A'])
    input_B = a3.MultiDimImageUInt32_to_ndarray(a3.inputs['labeled B'])

    intersecting_ids, intersection_labels = intersect_labels(input_A, input_B)

    a3.outputs['labeled intersection'] = \
        a3.MultiDimImageUInt32_from_ndarray(intersection_labels.astype(np.uint32))
    print(intersection_labels.shape)
    a3.outputs['label pair list'] = intersecting_ids
    print('labeled intersections are ready ✨')


inputs = [a3.Arg('labeled A', a3.types.ImageUInt32),
          a3.Arg('labeled B', a3.types.ImageUInt32)]

outputs = [a3.Arg('labeled intersection', a3.types.ImageUInt32),
           a3.Arg('label pair list', a3.types.GeneralPyType)]

a3.def_process_module(inputs, outputs, module_main)
