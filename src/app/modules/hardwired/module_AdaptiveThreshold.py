import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.segmentation import threshold_adaptive
from modules.a3dc_modules.a3dc.imageclass import Image
import numpy as np
import time

METHODS=['Mean', 'Gaussian']



def adaptive_threshold(image, method , blocksize=5, offset=0):
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
    logText += '\n\tMethod: Adaptive' + method
    logText += '\n\tSettings: \n\t\tBlocksize:%s \n\t\tOffset:%s' % (str(blocksize),str(offset))


    # Run thresholding functions
    try:

        outputArray = threshold_adaptive(image.array, method, blocksize, offset)

    except Exception as e:
        raise Exception("Error occured while thresholding image!",e)

    # Finish timing and add to logText
    tstop = time.clock()
    logText += '\n\tProcessing finished in ' + str((tstop - tstart)) + ' seconds! '

    return Image(outputArray, image.metadata), logText

def init_config(methods=METHODS):
 
    config = [ a3.Input('Input_Image', a3.types.ImageFloat),
               a3.Input('Input_Metadata', a3.types.GeneralPyType)]

    method_param=a3.Parameter('Method', a3.types.enum)
    for idx, m in enumerate(methods):
        method_param.setIntHint(str(m), idx)
    config.append(method_param)
    
    #Add inputfield for BlockSize
    param_blocksize=a3.Parameter('BlockSize', a3.types.float)
    param_blocksize.setIntHint('min', 2)
    param_blocksize.setIntHint('max', 800)
    param_blocksize.setIntHint('stepSize', 1),
    config.append(param_blocksize)
    
    
    #Add inputfield for Offset
    param_offset=a3.Parameter('Offset', a3.types.float)
    param_offset.setIntHint('min', 0)
    param_offset.setIntHint('max', 800)
    param_offset.setIntHint('stepSize', 1),
    config.append(param_offset)
    
    return config



def module_main(ctx):
    
    #Create Image object
    img = Image(a3.MultiDimImageFloat_to_ndarray(a3.inputs['Input_Image']), a3.inputs['Input_Metadata'])

    
    #Get method and mode
    method=METHODS[a3.inputs['Method'][-1]]

    #Run thresholding            
    output_img, logText=adaptive_threshold(img, method , blocksize=a3.inputs['BlockSize'], offset=a3.inputs['Offset'])
    print(logText)
    
    #Change Name in metadata
    #output_img.metadata['Name']=img.metadata['Name']+'_adaptive_thr'

    #Set output
    a3.outputs['Output_Image']=a3.MultiDimImageFloat_from_ndarray(output_img.array.astype(np.float)/np.amax(output_img.array).astype(np.float))
    a3.outputs['Output_Metadata']=output_img.metadata
        
config = init_config()
config.append(a3.Output('Output_Image', a3.types.ImageFloat)) 
config.append(a3.Output('Output_Metadata', a3.types.GeneralPyType))

a3.def_process_module(config, module_main)
