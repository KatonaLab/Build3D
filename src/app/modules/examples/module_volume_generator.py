from a3dc_module_interface import Arg, def_process_module
#import numpy as np
#from scripts.imageProcessing import autoThreshold
#from scripts.a3dc_utils import multi_dim_image_to_numpy_array, multi_dim_image_plane_iterator

def module_main():
    #im = multi_dim_image_to_numpy_array(a3dc.inputs['in'])
    #im = autoThreshold(im, 'Otsu')
    #a3dc.outputs['out'] = a3dc.MultiDimImageFloat(a3dc.inputs['in'].dims())
    #for i, plane in enumerate(multi_dim_image_plane_iterator(a3dc.outputs['out'])):
    #    np.copyto(plane, im[:, :, i])

#inputs = [Arg('in', a3dc.types.ImageFloat)]
#outputs = [Arg('out', a3dc.types.ImageFloat)]

a3dc.def_process_module(inputs, outputs, module_main)