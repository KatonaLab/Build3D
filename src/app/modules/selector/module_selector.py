import a3dc_module_interface as a3
from modules.a3dc_modules.external.PythImage import Image
import numpy as np
from modules.a3dc_modules.a3dc.utils import SEPARATOR, error, warning
import time
import copy
import sys


from modules.a3dc_modules.a3dc.a3image import  image_to_a3image


def module_main(ctx):
    try:
        #Inizialization
        tstart = time.clock()
        print(SEPARATOR)
        
        #Load and reshape image
        ##TempTempTemp##
        img = Image(a3.inputs['Image'], copy.deepcopy(a3.inputs['MetaData']))
        #img = Image(a3.inputs['Image'], a3.inputs['MetaData'])
        img.reorder('XYZCT')
        
        #Get channel from image. 
        ch=a3.inputs['Channel']
        print('Loading the following channel: ', img.metadata['Name'][ch])
        if ch>=img.metadata['SizeC']:
            raise Exception('Image has %s channels! Invalid channel %s' % (str(img.metadata['SizeC']), str(ch)))
        
        #Check if image is time series
        if img.metadata['SizeT']>1:
            warning("Image is a time series! Only the first time step will be extracted!", file=sys.stderr)
        
        #Extract channel from image array    
        dims = len(img.image.shape)
        if dims == 5:
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
        image_to_a3image(Image(array.astype(np.float),copy.deepcopy(img.metadata)))
       
        #Finalization
        tstop = time.clock()
        print('Processing finished in ' + str((tstop - tstart)) + ' seconds! ')
        print('Image loaded successfully!')
        print(SEPARATOR)
    
    except Exception as e:
        raise error("Error occured while executing "+str(ctx.name())+" !",exception=e)

config = [a3.Parameter('Channel', a3.types.int8)
                .setFloatHint('default', 0)
                .setFloatHint('unusedValue',0),
    a3.Input('Image', a3.types.GeneralPyType),
    a3.Input('MetaData', a3.types.GeneralPyType),
    a3.Output('Channel', a3.types.ImageFloat)
    ]

a3.def_process_module(config, module_main)
