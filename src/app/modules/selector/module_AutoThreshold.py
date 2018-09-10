import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.segmentation import threshold_auto
from modules.a3dc_modules.a3dc.imageclass import Image
from modules.a3dc_modules.a3dc.utils import SEPARATOR
import time, traceback


METHODS=['Triangle', 'IsoData', 'MaxEntropy', 'Moments','RenyiEntropy','Huang', 'Li','KittlerIllingworth','Yen','Shanbhag','Otsu']

def auto_threshold(image, method="Otsu", mode="Slice"):
    '''

    :param image:
    :param imageDictionary:
    :param method:
    :param kwargs:
        lowerThreshold, upperThreshold, mode,blockSize=5, offSet=0

    :return:
        LogText
    '''
    # Creatre LogText and start logging
    print('Thresholding: '+image.metadata['Name'])
    print('Method: ' + method)
    print('Mode: ' + mode)

    try:
        
        #Threshold image
        output_array, threshold_value = threshold_auto(image.array, method, mode)
        
        #Create metadata
        output_metadta=image.metadata
        output_metadta['Type']=output_array.dtype
        
        print('Threshold values: ' + str(threshold_value))

    except Exception as e:
        traceback.print_exc()
        raise Exception("Error occured while thresholding image!",e)

    return Image(output_array, output_metadta)


def init_config(methods=METHODS):
 
    config = [ a3.Input('Input_Image', a3.types.ImageFloat),
               a3.Input('Input_Metadata', a3.types.GeneralPyType)]

    param=a3.Parameter('Method', a3.types.enum)
    for idx, m in enumerate(methods):
        param.setIntHint(str(m), idx)
    
    config.append(param)
    
    config.append(a3.Parameter('Stack', a3.types.bool))
    
    return config




def module_main(ctx):
    
    #Inizialization
    tstart = time.clock()
    print(SEPARATOR)
    print('Autothresholding started!')
    
    #Create Image object
    img = Image(a3.MultiDimImageFloat_to_ndarray(a3.inputs['Input_Image']), a3.inputs['Input_Metadata'])

    
    #Get method and mode
    method=METHODS[a3.inputs['Method'][-1]]
      
    if a3.inputs['Stack']:
        mode='Stack'
    else:
        mode='Slice'
                    
    #Run thresholding            
    output_img=auto_threshold(img, method, mode)

    #Change Name in metadata
    #output_img.metadata['Name']=img.metadata['Name']+'_auto_thr'
    
    #Set output
    a3.outputs['Output_Image']=a3.MultiDimImageFloat_from_ndarray(output_img.array.astype(float))
    a3.outputs['Output_Metadata']=output_img.metadata

    #Finalization
    tstop = time.clock()
    print('Processing finished in ' + str((tstop - tstart)) + ' seconds!')
    print('Autothresholding was successfully!')
    print(SEPARATOR)
    
config = init_config()
config.append(a3.Output('Output_Image', a3.types.ImageFloat)) 
config.append(a3.Output('Output_Metadata', a3.types.GeneralPyType))

a3.def_process_module(config, module_main)
