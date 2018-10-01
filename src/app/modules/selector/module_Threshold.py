import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.interface import threshold

from modules.a3dc_modules.a3dc.utils import SEPARATOR, error
import time, math


from modules.a3dc_modules.a3dc.imageclass import VividImage
from modules.a3dc_modules.a3dc.multidimimage import from_multidimimage, to_multidimimage


METHODS=['Manual', 'Triangle', 'IsoData', 'MaxEntropy', 'Moments','RenyiEntropy','Huang', 'Li','KittlerIllingworth','Yen','Shanbhag','Otsu']

def module_threshold(image, method="Otsu", kwargs={}):

    #Threshold image
    output, logText = threshold(image, method, **kwargs)
    print('Threshold value(s): ' + str(logText.split('\n')[-2].split(':')[-1]))

    return output


def generate_config(methods=METHODS):
    
    #Set Outputs and inputs
    config = [a3.Input('Input Image', a3.types.ImageFloat),
               a3.Output('Output Image', a3.types.ImageFloat)]

    #Set parameters
    param=a3.Parameter('Method', a3.types.enum)
    for idx, m in enumerate(methods):
        param.setIntHint(str(m), idx)
    
    config.append(param)
    config.append(a3.Parameter('Threshold', a3.types.float)
            .setFloatHint('default', float(math.inf))
            .setFloatHint('unusedValue', float(math.inf))
            .setFloatHint('stepSize', 1))
    config.append(a3.Parameter('Stack Histogram', a3.types.bool))
    
    return config


def module_main(ctx):
    
    try:
        #Inizialization
        tstart = time.clock()
        print(SEPARATOR)
        print('Thresholding started!')
        
        #Create Image object
        img =from_multidimimage(a3.inputs['Input Image'])
        print('Thresholding: '+img.metadata['Name'])
        
        #Get method and mode
        method=METHODS[a3.inputs['Method'][-1]]
        print('Method: ' + method)
        
        #Get kwargs if method is manual
        if method=='Manual':
            kwargs={'lower':0, 'upper':a3.inputs['Threshold']}
        else:
            kwargs={}
            if a3.inputs['Stack Histogram']:
                kwargs['mode']='Stack'
            else:
                kwargs['mode']='Slice'
            print('Mode: ' +kwargs['mode'])
            
        #Run thresholding            
        output_img=module_threshold(img, method,kwargs)
        
        #Change Name in metadata
        #output_img.metadata['Name']=img.metadata['Name']+'_auto_thr'
        
        #Set output
        a3.outputs['Output Image']=to_multidimimage(output_img)
      
        #Finalization
        tstop = time.clock()
        print('Processing finished in ' + str((tstop - tstart)) + ' seconds!')
        print('Autothresholding was successfully!')
        print(SEPARATOR)
    
    except Exception as e:
        raise error("Error occured while executing "+str(ctx.name())+" !",exception=e)
    
a3.def_process_module(generate_config(), module_main)
