import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.interface import threshold
from modules.a3dc_modules.a3dc.imageclass import Image
from modules.a3dc_modules.a3dc.utils import SEPARATOR
import time, traceback



METHODS=['Manual', 'Triangle', 'IsoData', 'MaxEntropy', 'Moments','RenyiEntropy','Huang', 'Li','KittlerIllingworth','Yen','Shanbhag','Otsu']

def module_threshold(image, method="Otsu", kwargs={}):
    '''

    :param image:
    :param imageDictionary:
    :param method:
    :param kwargs:
        lowerThreshold, upperThreshold, mode,blockSize=5, offSet=0

    :return:
        LogText
    '''

    try:
        
        #Threshold image
        output, logText = threshold(image, method, **kwargs)
        print('Threshold value(s): ' + str(logText.split('\n')[-2].split(':')[-1]))
    
    except Exception as e:
        
        if isinstance(e, Warning):
            print('Warning :'+str(e))
        else:
            traceback.print_exc()
            raise Exception("Error occured while thresholding image!",e)

    return output


def init_config(methods=METHODS):
 
    config = [ a3.Input('Input_Image', a3.types.ImageFloat),
               a3.Input('Input_Metadata', a3.types.GeneralPyType)]

    param=a3.Parameter('Method', a3.types.enum)
    for idx, m in enumerate(methods):
        param.setIntHint(str(m), idx)
    
    config.append(param)
    config.append(a3.Parameter('Threshold', a3.types.float)
            .setFloatHint('min', 0)
            .setFloatHint('max', 2**64)
            .setFloatHint('stepSize', 1))
    config.append(a3.Parameter('Stack', a3.types.bool))
    
    return config




def module_main(ctx):
    
    #Inizialization
    tstart = time.clock()
    print(SEPARATOR)
    print('Thresholding started!')
    
    
    #Create Image object
    img = Image(a3.MultiDimImageFloat_to_ndarray(a3.inputs['Input_Image']), a3.inputs['Input_Metadata'])
    print('Thresholding: '+img.metadata['Name'])
    
    #Get method and mode
    method=METHODS[a3.inputs['Method'][-1]]
    print('Method: ' + method)
    
    #Get kwargs if method is manual
    if method=='Manual':
        kwargs={'lower':0, 'upper':a3.inputs['Threshold']}
    else:
        kwargs={}
        if a3.inputs['Stack']:
            kwargs['mode']='Stack'
        else:
            kwargs['mode']='Slice'
        print('Mode: ' +kwargs['mode'])
        
      
    #Run thresholding            
    output_img=module_threshold(img, method,kwargs)

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
