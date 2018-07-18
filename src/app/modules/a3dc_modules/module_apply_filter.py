import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.interface import apply_filter
from modules.a3dc_modules.a3dc.imageclass import Image
import numpy as np

def module_main(ctx):
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


config = [a3.Input('tagged input', a3.types.ImageUInt32),
    a3.Input('db', a3.types.GeneralPyType),
    a3.Parameter('mean intensity min', a3.types.float),
    a3.Parameter('mean intensity max', a3.types.float),
    a3.Parameter('stdev min', a3.types.float),
    a3.Parameter('stdev max', a3.types.float),
    a3.Parameter('sum intensity min', a3.types.float),
    a3.Parameter('sum intensity max', a3.types.float),
    a3.Output('tagged output', a3.types.ImageUInt32),
    a3.Output('db output', a3.types.GeneralPyType)]

a3.def_process_module(config, module_main)
