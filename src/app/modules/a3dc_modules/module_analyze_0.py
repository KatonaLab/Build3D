import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.interface import analyze
from modules.a3dc_modules.a3dc.imageclass import Image
import numpy as np

def module_main(ctx):
    im = a3.MultiDimImageUInt32_to_ndarray(a3.inputs['tagged input'])
    a3dc_image = Image(im, {'Name': 'random_name', 'Type': np.uint32})

    meas_list = ['volume', 'voxelCount', 'centroid', 'pixelsOnBorder',
        'ellipsoidDiameter', 'boundingBox', 'elongation',
        'equivalentSphericalRadius', 'flatness', 'principalAxes',
        'principalMoments', 'roundness', 'feretDiameter', 'perimeter',
        'perimeterOnBorder', 'perimeterOnBorderRatio', 'equivalentSphericalPerimeter']

    a3dc_out, log_text = analyze(a3dc_image, imageList=None, measurementInput=meas_list)
    print(log_text)

    a3.outputs['tagged output'] = a3.MultiDimImageUInt32_from_ndarray(a3dc_out.array)
    a3.outputs['db output'] = a3dc_out.database


config = [a3.Input('tagged input', a3.types.ImageUInt32),
    a3.Output('tagged output', a3.types.ImageUInt32),
    a3.Output('db output', a3.types.GeneralPyType)]

a3.def_process_module(config, module_main)
