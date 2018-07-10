import a3dc_module_interface as a3
from modules.a3dc_modules.external.PythImage import Image
import numpy as np


def module_main(_):
    filename = a3.inputs['filename']
    print('provided filename', filename)

    output = Image.load(filename, file_type='ome')
    output.reorder('XYZCT')

    nd = len(output.image.shape)
    if nd < 3 or nd > 5:
        print('can not read image less than 3 dimensions')
        return

    if nd == 5:
        im = output.image[0, 0, :, :, :]
    elif nd == 4:
        im = output.image[0, :, :, :]
    elif nd == 3:
        im = output.image[:, :, :]
    else:
        pass

    print(im.shape)
    a3.outputs['volume'] = a3.MultiDimImageFloat_from_ndarray(im.astype(np.float))


config = [
    a3.Parameter('filename', a3.types.string).setBoolHint('file', True),
    a3.Output('volume', a3.types.ImageFloat)]

a3.def_process_module(config, module_main)
