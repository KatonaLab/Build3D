import a3dc_module_interface as a3
from modules.a3dc_modules.external.PythImage import Image
import numpy as np
from modules.a3dc_modules.a3dc.utils import SEPARATOR
import time

def module_main(_):
    #Load and reshape image
    img = Image(a3.inputs['Image'], a3.inputs['MetaData'])
    img.reorder('XYZCT')
    
    
    
    #Inizialization
    tstart = time.clock()
    print(SEPARATOR)
    
    
    #Get channel from image. 
    ch=a3.inputs['Channel']
    print('Loading the following channel: ', img.metadata['Name'][ch])
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
    print(img.metadata)
    #Create Output
    a3.outputs['Channel'] = a3.MultiDimImageFloat_from_ndarray(array.astype(np.float))
    a3.outputs['MetaData2']=img.metadata
    print(a3.outputs['MetaData2'])
    #Finalization
    tstop = time.clock()
    print('Processing finished in ' + str((tstop - tstart)) + ' seconds! ')
    print('Image loaded successfully!')
    print(SEPARATOR)


config = [a3.Parameter('Channel', a3.types.int8)
            .setIntHint('min', 0)
            .setIntHint('max', 8)
            .setIntHint('stepSize', 1),
    a3.Input('Image', a3.types.GeneralPyType),
    a3.Input('MetaData', a3.types.GeneralPyType),
    a3.Output('Channel', a3.types.ImageFloat),
    a3.Output('MetaData2', a3.types.GeneralPyType)]

a3.def_process_module(config, module_main)
