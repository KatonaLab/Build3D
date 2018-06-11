import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.interface import colocalization
from modules.a3dc_modules.a3dc.imageclass import Image
import numpy as np

tagged_input_base = 'tagged input {}'
db_input_base = 'db {}'
intensity_input_base = 'intensity input {}'
output_tagged_base = 'output tagged image {}'
output_db_base = 'output db {}'
N = 2
K = N + 1

def convert_input(i):
    tagged_input_name = tagged_input_base.format(i)
    db_input_name = db_input_base.format(i)
    intensity_input_name = intensity_input_base.format(i)

    tagged = a3.MultiDimImageUInt32_to_ndarray(a3.inputs[tagged_input_name])
    db = a3.inputs[db_input_name]
    im = a3.MultiDimImageFloat_to_ndarray(a3.inputs[intensity_input_name])

    converted_tagged = Image(tagged, metadata={'Name': 'random_name', 'Type': np.uint32}, db=db)
    converted_intensity = Image(im, metadata={'Name': 'random_name', 'Type': np.float})

    return converted_tagged, converted_intensity


def add_input_triplet(input_list, i):
    input_list.append(a3.Arg(tagged_input_base.format(i), a3.types.ImageUInt32))
    input_list.append(a3.Arg(db_input_base.format(i), a3.types.GeneralPyType))
    input_list.append(a3.Arg(intensity_input_base.format(i), a3.types.ImageFloat))
    return input_list

def module_main():

    tagged_image_list = []
    source_image_list = []
    for i in range(1, K):
        tagged, intensity = convert_input(i)
        tagged_image_list.append(tagged)
        source_image_list.append(intensity)

    filter_dict = {
        'colocalizationCount': {'min': a3.inputs['co-loc count min'], 'max': a3.inputs['co-loc count max']},
        'totalOverlappingRatio': {'min': a3.inputs['overlap ration min'], 'max': a3.inputs['overlap ration max']},
        'volume': {'min': a3.inputs['volume min'], 'max': a3.inputs['volume max']}}

    overlapping_image, tagged_image_list, log_text = colocalization(
        taggedImageList=tagged_image_list,
        sourceImageList=source_image_list,
        overlappingFilter=filter_dict,
        removeFiltered=True,
        overWrite=False)

    print(log_text)

    a3.outputs['overlapping image'] = a3.MultiDimImageUInt32_from_ndarray(overlapping_image.array)
    a3.outputs['overlapping db'] = overlapping_image.db

    for i in range(1, K):
        a3.outputs[output_tagged_base.format(i)] = a3.MultiDimImageUInt32_from_ndarray(tagged_image_list[i].array)
        a3.outputs[output_db_base.format(i)] = tagged_image_list[i].db

inputs = [
    a3.Arg('co-loc count min', a3.types.uint32, 'parameter'),
    a3.Arg('co-loc count max', a3.types.uint32, 'parameter'),
    a3.Arg('overlap ration min', a3.types.float, 'parameter'),
    a3.Arg('overlap ration max', a3.types.float, 'parameter'),
    a3.Arg('volume min', a3.types.uint32, 'parameter'),
    a3.Arg('volume max', a3.types.uint32, 'parameter')]

outputs = [
    a3.Arg('overlapping image', a3.types.ImageUInt32),
    a3.Arg('overlapping db', a3.types.GeneralPyType)]

for i in range(1, K):
    inputs = add_input_triplet(inputs, i)
    outputs.append(a3.Arg(output_tagged_base.format(i), a3.types.ImageUInt32))
    outputs.append(a3.Arg(output_db_base.format(i), a3.types.GeneralPyType))

a3.def_process_module(inputs, outputs, module_main)

# def apply_filter(image, filterDict=None, removeFiltered=True, overWrite=!!!False!!!):

# === analyze
# kell:
# 'volume'
# 'voxelCount'
# 'centroid'
# 'pixelsOnBorder'

# nem kell:
# 'ellipsoidDiameter'
# 'boundingBox'
# 'elongation'
# 'equivalentSphericalRadius'
# 'flatness'
# 'principalAxes'
# 'principalMoments'
# 'roundness'
# 'feretDiameter'
# 'perimeter'
# 'perimeterOnBorder'
# 'perimeterOnBorderRatio'
# 'equivalentSphericalPerimeter'

# ---

# === apply_filter
# kell:
# 'meanIntensity'
# 'centerOfMass'
# 'standardDeviation'
# 'cumulativeIntensity'

# nem kell:
# 'medianIntensity'
# 'skewness'
# 'kurtosis'
# 'variance'
# 'maximumPixel'
# 'minimumPixel'
# 'maximumValue'
# 'minimumValue'
# 'getWeightedElongation'
# 'getWeightedFlatness'
# 'getWeightedPrincipalAxes'
# 'getWeightedPrincipalMoments'

# ---

# === colocalization
# kell:
# 'colocalizationCount'
# 'totalOverlappingRatio'
# 'volume'