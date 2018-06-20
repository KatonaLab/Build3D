import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.interface import analyze
from modules.a3dc_modules.a3dc.imageclass import Image
import numpy as np

def module_main(ctx):
    tagged_im = a3.MultiDimImageUInt32_to_ndarray(a3.inputs['tagged input'])
    a3dc_tagged_image = Image(tagged_im, {'Name': 'random_name', 'Type': np.uint32})

    meas_list = ['volume', 'voxelCount', 'centroid', 'pixelsOnBorder',
        'ellipsoidDiameter', 'boundingBox', 'elongation',
        'equivalentSphericalRadius', 'flatness', 'principalAxes',
        'principalMoments', 'roundness', 'feretDiameter', 'perimeter',
        'perimeterOnBorder', 'perimeterOnBorderRatio', 'equivalentSphericalPerimeter',
        'meanIntensity', 'centerOfMass', 'standardDeviation', 'cumulativeIntensity',
        'medianIntensity', 'skewness', 'kurtosis', 'variance', 'maximumPixel',
        'minimumPixel', 'maximumValue', 'minimumValue', 'getWeightedElongation',
        'getWeightedFlatness', 'getWeightedPrincipalAxes', 'getWeightedPrincipalMoments']

    float_im_1 = a3.MultiDimImageFloat_to_ndarray(a3.inputs['intensity input 1'])
    a3dc_float_image_1 = Image(float_im_1, {'Name': 'random_name', 'Type': np.float})

    image_list = [a3dc_float_image_1]

    a3dc_out, log_text = analyze(a3dc_tagged_image, imageList=image_list, measurementInput=meas_list)
    print(log_text)

    a3.outputs['tagged output'] = a3.MultiDimImageUInt32_from_ndarray(a3dc_out.array)
    a3.outputs['db output'] = a3dc_out.database


inputs = [a3.Arg('tagged input', a3.types.ImageUInt32),
    a3.Arg('intensity input 1', a3.types.ImageFloat)]

outputs = [a3.Arg('tagged output', a3.types.ImageUInt32),
    a3.Arg('db output', a3.types.GeneralPyType)]

a3.def_process_module(inputs, outputs, module_main)
