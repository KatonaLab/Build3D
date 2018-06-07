import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.interface import tagImage
from modules.a3dc_modules.a3dc.imageclass import Image
import numpy as np

def module_main():
    im = a3.MultiDimImageUInt32_to_ndarray(a3.inputs['input'])
    a3dc_image = Image(im, {'Name': 'random_name', 'Type': np.uint8})

    a3dc_out, log_text = apply_filter(a3dc_image, filterDict=)
    print(log_text)

    a3.outputs['output'] = a3.MultiDimImageUInt32_from_ndarray(a3dc_out.array)


inputs = [a3.Arg('input', a3.types.ImageUInt32)]
outputs = [a3.Arg('output', a3.types.ImageUInt32)]

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
