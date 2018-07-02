import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.interface import save_data
from modules.a3dc_modules.a3dc.imageclass import Image
import numpy as np

def module_main(ctx):
    tagged_im = a3.MultiDimImageUInt32_to_ndarray(a3.inputs['tagged input'])
    db = a3.inputs['db output']
    a3dc_tagged_image = Image(tagged_im, metadata={'Name': 'random_name', 'Type': np.uint32}, database=db)

    log_text = save_data([a3dc_tagged_image], './', toText=False)
    print(log_text)


config = [a3.Input('tagged input', a3.types.ImageUInt32), a3.Input('db', a3.types.GeneralPyType)]

a3.def_process_module(config, module_main)
