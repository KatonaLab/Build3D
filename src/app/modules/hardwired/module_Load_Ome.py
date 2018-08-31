import a3dc_module_interface as a3
from modules.a3dc_modules.external.PythImage import Image
import numpy as np


def module_main(_):
    filename = a3.inputs['FileName'].path
   
    print('Loading the following image: ', filename)
    
    #Load and reshape image
    img = Image.load(filename, file_type='ome')
    img.reorder('XYZCT')

    #Get channel from image. 
    ch=a3.inputs['Channel']
    if ch>=img.metadata['SizeC']:
        raise Exception('Image has %s channels! Invalid channel %s' % (str(img.metadata['SizeC']), str(ch)))
        
    dims = len(img.image.shape)
    if dims == 5:
        raise Warning("Image is a time series! Only the first time step will be extracted!")
        array = img.image[0, ch, :, :, :]
    elif dims == 4:
        array = img.image[ch, :, :, :]
    elif dims == 3:
        array = img.image[:, :, :]
    else:
        raise Exception('Can only read images with 3-5 dimensions!')
    
    #Modify metadata 
    img.metadata['SamplesPerPixel']=img.metadata['SamplesPerPixel'][ch]
    img.metadata['Name']=img.metadata['Name'][ch]   
    #img.metadata['Path']=filename
    
    #Create Output
    a3.outputs['Volume'] = a3.MultiDimImageFloat_from_ndarray((array/np.amax(array)).astype(np.float))
    a3.outputs['MetaData']=img.metadata

    print('File loaded succesfully!')

config = [
    a3.Parameter('FileName', a3.types.url),
    a3.Parameter('Channel', a3.types.int8)
            .setIntHint('min', 0)
            .setIntHint('max', 8)
            .setIntHint('stepSize', 1),
    a3.Output('Volume', a3.types.ImageFloat),
    a3.Output('MetaData', a3.types.GeneralPyType)]

a3.def_process_module(config, module_main)
