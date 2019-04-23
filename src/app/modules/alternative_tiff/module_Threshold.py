import time
import math
import a3dc_module_interface as a3


from modules.a3dc_interface import threshold
from modules.a3dc_interface_utils import error,print_line_by_line, SEPARATOR


METHODS=['Manual', 'Triangle', 'IsoData', 'MaxEntropy', 'Moments','RenyiEntropy','Huang', 'Li','KittlerIllingworth','Yen','Shanbhag','Otsu','IsoData_skimage', 'Otsu_skimage','Li_skimage','Yen_skimage','Triangle_skimage','None']


def module_threshold(image, method="Otsu", kwargs={}):

    #Threshold image
    output, logText = threshold(image, method, **kwargs)

    #Print logText
    print_line_by_line(logText)
    return output


def generate_config(methods=METHODS):
    
    #Set Outputs and inputs
    config = [a3.Input('Input Image', a3.types.GeneralPyType),
               a3.Output('Thresholded Image', a3.types.GeneralPyType)]

    #Set parameters
    param=a3.Parameter('Method', a3.types.enum)
    for idx, m in enumerate(methods):
        param.setIntHint(str(m), idx)
    
    config.append(param)
    config.append(a3.Parameter('Manual threshold value', a3.types.float)
            .setFloatHint('default', float(math.inf)))
            #.setFloatHint('unusedValue', float(math.inf))
            #.setFloatHint('stepSize', 1))
    config.append(a3.Parameter('Slice/Stack histogram', a3.types.bool))
    
    return config


def module_main(ctx):
    
    try:
        #Inizialization
        tstart = time.clock()
        print(SEPARATOR)
        print('Thresholding started!')
        
        #Create Image object
        img =a3.inputs['Input Image']

        #Get method and mode. Get kwargs if method is manual
        method=METHODS[a3.inputs['Method'][-1]]
        if method=='Manual':
            kwargs={'lower':0, 'upper':a3.inputs['Manual threshold value']}
        elif method=='None':
            method='Manual'
            kwargs={'lower':0, 'upper':0}
        else:
            kwargs={}
            if a3.inputs['Slice/Stack histogram']:
                kwargs['mode']='Stack'
            else:
                kwargs['mode']='Slice'
        
        #Run thresholding         
        output_img=module_threshold(img, method,kwargs)
        
        #Change Name in metadata
        #output_img.metadata['Name']=img.metadata['Name']+'_auto_thr'
        
        #Set output
        a3.outputs['Thresholded Image']=output_img
      
        #Finalization
        tstop = time.clock()
        print('Processing finished in ' + str((tstop - tstart)) + ' seconds!')
        print('Autothresholding was successfully!')
        print(SEPARATOR)
    
    except Exception as e:
        raise error("Error occured while executing '"+str(ctx.type())+"' module '"+str(ctx.name())+"' !",exception=e)
    
a3.def_process_module(generate_config(), module_main)
