import a3dc_module_interface as a3
from modules.a3dc_modules.external.PythImage.ImageClass import ImageClass as PythImage
from modules.a3dc_modules.a3dc.utils import SEPARATOR, error, print_line_by_line, warning
from modules.a3dc_modules.a3dc.imageclass import Image as Im
import time, copy
import numpy as np


from modules.a3dc_modules.a3dc.a3image import  image_to_a3image

def get_channel(img, ch):
    
    img.reorder('ZXYCT')
    
    #Get channel from image. 
    print('Loading the following channel: ', img.metadata['Name'][ch])
    if ch>=img.metadata['SizeC']:
        raise Exception('Image has %s channels! Invalid channel %s' % (str(img.metadata['SizeC']), str(ch)))
    
    #Check if image is time series
    if img.metadata['SizeT']>1:
        warning("Image is a time series! Only the first time step will be extracted!")
            
    #Create metadata
    metadata=copy.deepcopy(img.metadata)
    metadata['SamplesPerPixel']=metadata['SamplesPerPixel'][ch]
    metadata['Name']=metadata['Name'][ch]  
    
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
    print(array.shape)
    #array=np.flip(np.swapaxes(array,0,2), axis=2),axis=1)
    #array=np.swapaxes(array,0,2)[::-1,::-1,::]
    array=array[::-1,::-1,::]
    print(array.shape)
    
    return Im(array, metadata)

def module_main(ctx):

    try:
        

        filename = a3.inputs['FileName'].path

        #Inizialization
        tstart = time.clock()
        print(SEPARATOR)
        print('Loading the following image: ', filename)
        
        #Load and reshape image
        img=PythImage.load(filename)
        
        #Print important image parameters
        print_line_by_line(str(img))
   
        #Create Output 1
        ch_1=get_channel(img, a3.inputs['Channel 1'])
        ch_1.metadata['Path']=filename
        a3.outputs['Channel 1'] = image_to_a3image(ch_1)


        #Create Output 2
        ch_2=get_channel(img, a3.inputs['Channel 2'])
        ch_2.metadata['Path']=filename
        a3.outputs['Channel 2'] = image_to_a3image(ch_2)

     
        #Finalization
        tstop = time.clock()
        print('Processing finished in ' + str((tstop - tstart)) + ' seconds! ')
        print('Image loaded successfully!')
        print(SEPARATOR)

    except Exception as e:
        raise error("Error occured while executing "+str(ctx.name())+" !",exception=e)


    
config = [a3.Input('FileName', a3.types.url),
          a3.Parameter('Channel 1', a3.types.int8)
                .setIntHint('default', 0),
                #.setIntHint('max', 8)
                #.setIntHint('min', 0)
                #.setIntHint('unusedValue', 1),  
          a3.Parameter('Channel 2', a3.types.int8)
                .setIntHint('default', 1),
                #.setIntHint('max', 8)
                #.setIntHint('min', 0)
                #.setIntHint('unusedValue', 1),
          a3.Output('Channel 1', a3.types.ImageFloat),
          a3.Output('Channel 2', a3.types.ImageFloat)]
    

a3.def_process_module(config, module_main)
