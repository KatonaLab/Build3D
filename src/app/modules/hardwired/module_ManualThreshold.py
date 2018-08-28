import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.segmentation import threshold_manual
from modules.a3dc_modules.a3dc.imageclass import Image
import numpy as np
import time

def manual_threshold(image,  upper, lower):
    '''
    :param image:
    :param imageDictionary:
    :param method:
    :param kwargs:
        lowerThreshold, upperThreshold, mode,blockSize=5, offSet=0

    :return:
        LogText
    '''

    # Start timing
    tstart = time.clock()

    # Creatre LogText and start logging
    logText = '\nThresholding: '+image.metadata['Name']
    logText += '\n\tMethod: Manual'
    logText += '\n\tSettings: \n\t\tUpper:%s \n\t\tLower:%s' % (str(upper),str(lower))
   

    try:
        
        outputArray = threshold_manual(image.array, upper, lower)

    except Exception as e:
        raise Exception("Error occured while thresholding image!",e)

    # Finish timing and add to logText
    tstop = time.clock()
    logText += '\n\tProcessing finished in ' + str((tstop - tstart)) + ' seconds! '

    return Image(outputArray, image.metadata), logText

def init_config():
 
    config = [ a3.Input('Input_Image', a3.types.ImageFloat),
               a3.Input('Input_Metadata', a3.types.GeneralPyType)]

    
    #Add inputfield for Lower threshold
    param_blocksize=a3.Parameter('Lower', a3.types.int8)
    param_blocksize.setIntHint('min', 0)
    param_blocksize.setIntHint('max', 65535)
    param_blocksize.setIntHint('stepSize', 1),
    config.append(param_blocksize)
    
    
    #Add inputfield for Upper threshold
    param_offset=a3.Parameter('Upper', a3.types.int8)
    param_offset.setIntHint('min', 0)
    param_offset.setIntHint('max', 65535)
    param_offset.setIntHint('stepSize', 1),
    config.append(param_offset)
    
    return config




def module_main(ctx):
    
    #Create Image object
    img = Image(a3.MultiDimImageFloat_to_ndarray(a3.inputs['Input_Image']), a3.inputs['Input_Metadata'])
    
    #Run thresholding            
    output_img, logText=manual_threshold(img, a3.inputs['Upper'], a3.inputs['Lower'])
    print(logText)
    
    #Set output
    a3.outputs['Output_Image']=a3.MultiDimImageFloat_from_ndarray(output_img.array.astype(np.float)/np.amax(output_img.array).astype(np.float))
    a3.outputs['Output_Metadata']=output_img.metadata
    
config = init_config()
config.append(a3.Output('Output_Image', a3.types.ImageFloat)) 
config.append(a3.Output('Output_Metadata', a3.types.GeneralPyType))

a3.def_process_module(config, module_main)
