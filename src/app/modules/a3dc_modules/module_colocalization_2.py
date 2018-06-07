import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.interface import colocalization
from modules.a3dc_modules.a3dc.imageclass import Image
import numpy as np


def module_main():
    ndim_1 = a3.MultiDimImageFloat_to_ndarray(a3.inputs['input1'])
    ndim_2 = a3.MultiDimImageFloat_to_ndarray(a3.inputs['input2'])

    r_ndim_1 = a3.MultiDimImageFloat_to_ndarray(a3.inputs['raw_input1'])
    r_ndim_2 = a3.MultiDimImageFloat_to_ndarray(a3.inputs['raw_input2'])

    i1 = Image(ndim_1, {'Name': 'random_name', 'Type': np.float})
    i2 = Image(ndim_2, {'Name': 'random_name', 'Type': np.float})

    r1 = Image(r_ndim_1, {'Name': 'random_name', 'Type': np.float})
    r2 = Image(r_ndim_2, {'Name': 'random_name', 'Type': np.float})

    overlappingImage, taggedImageList, logText = colocalization(taggedImageList=[i1, i2], sourceImageList=[r1, r2],
        overlappingFilter=None, removeFiltered=False, overWrite=True)

    print(logText)

    a3.outputs['output'] = a3.MultiDimImageFloat_from_ndarray(a3dc_out.array)

inputs = [a3.Arg('input1', a3.types.ImageFloat), a3.Arg('input2', a3.types.ImageFloat),
    a3.Arg('raw_input1', a3.types.ImageFloat), a3.Arg('raw_input2', a3.types.ImageFloat)]
outputs = [a3.Arg('output', a3.types.ImageFloat)]

a3.def_process_module(inputs, outputs, module_main)



def apply_filter(image, filterDict=None, removeFiltered=True, overWrite=!!!False!!!):

=== analyze
kell:
'volume'
'voxelCount'
'centroid'
'pixelsOnBorder'

nem kell:
'ellipsoidDiameter'
'boundingBox'
'elongation'
'equivalentSphericalRadius'
'flatness'
'principalAxes'
'principalMoments'
'roundness'
'feretDiameter'
'perimeter'
'perimeterOnBorder'
'perimeterOnBorderRatio'
'equivalentSphericalPerimeter'

---

=== apply_filter
kell:
'meanIntensity'
'centerOfMass'
'standardDeviation'
'cumulativeIntensity'

nem kell:
'medianIntensity'
'skewness'
'kurtosis'
'variance'
'maximumPixel'
'minimumPixel'
'maximumValue'
'minimumValue'
'getWeightedElongation'
'getWeightedFlatness'
'getWeightedPrincipalAxes'
'getWeightedPrincipalMoments'

---

=== colocalization
kell:
'colocalizationCount'
'totalOverlappingRatio'
'volume'