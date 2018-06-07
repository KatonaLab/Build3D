import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.interface import apply_filter
from modules.a3dc_modules.a3dc.imageclass import Image
import numpy as np

def module_main():
    tagged_im = a3.MultiDimImageUInt32_to_ndarray(a3.inputs['tagged input'])
    db = a3.inputs['db output']
    a3dc_tagged_image = Image(tagged_im, metadata={'Name': 'random_name', 'Type': np.uint32}, database=db)

    filter_dict = {'meanIntensity': {'min': 0, 'max': 1},
        'standardDeviation': {'min': 0, 'max': 1},
        'cumulativeIntensity': {'min': 0, 'max': 1}}

    a3dc_out, log_text = apply_filter(a3dc_tagged_image, filterDict=filter_dict, removeFiltered=True, overWrite=False)
    print(log_text)

    a3.outputs['tagged output'] = a3.MultiDimImageUInt32_from_ndarray(a3dc_out.array)
    a3.outputs['db output'] = a3dc_out.database


inputs = [a3.Arg('tagged input', a3.types.ImageUInt32),
    a3.Arg('db', a3.types.GeneralPyType),
    a3.Arg('mean intensity min', a3.types.ImageFloat, 'parameter'),
    a3.Arg('mean intensity max', a3.types.ImageFloat, 'parameter'),
    a3.Arg('stdev min', a3.types.ImageFloat, 'parameter'),
    a3.Arg('stdev max', a3.types.ImageFloat, 'parameter'),
    a3.Arg('sum intensity min', a3.types.ImageFloat, 'parameter'),
    a3.Arg('sum intensity max', a3.types.ImageFloat, 'parameter')]

outputs = [a3.Arg('tagged output', a3.types.ImageUInt32),
    a3.Arg('db output', a3.types.GeneralPyType)]

a3.def_process_module(inputs, outputs, module_main)

