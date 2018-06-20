import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.interface import save_image
from modules.a3dc_modules.a3dc.imageclass import Image
import numpy as np

def module_main(ctx):
    tagged_im = a3.MultiDimImageUInt32_to_ndarray(a3.inputs['tagged input'])
    db = a3.inputs['db output']
    a3dc_tagged_image = Image(tagged_im, metadata={'Name': 'random_name', 'Type': np.uint32}, database=db)

    log_text = save_image([a3dc_tagged_image], './', 'suffix')
    print(log_text)


inputs = [a3.Arg('tagged input', a3.types.ImageUInt32), a3.Arg('db', a3.types.GeneralPyType)]

outputs = []

a3.def_process_module(inputs, outputs, module_main)
