import a3dc_module_interface as a3
from modules.packages.a3dc.segmentation import threshold_manual
import numpy as np
import time
from modules.packages.a3dc.utils import error
from modules.packages.a3dc.ImageClass import VividImage
from modules.packages.a3dc.constants import SEPARATOR


def generate_config():
 
    config = [ a3.Input('Input Image', a3.types.ImageFloat),
               a3.Output('Output Image', a3.types.ImageFloat)]

    #Add inputfield for Upper threshold
    param_upper=a3.Parameter('Upper', a3.types.float)
    #param_upper.setIntHint('min', 0)
    #param_upper.setIntHint('max', 65535)
    param_upper.setIntHint('stepSize', 1),
    config.append(param_upper)
    
    #Add inputfield for Lower threshold
    param_lower=a3.Parameter('Lower', a3.types.float)
    #param_lower.setIntHint('min', 0)
    #param_lower.setIntHint('max', 65535)
    param_lower.setIntHint('stepSize', 1),
    config.append(param_lower)
    
    return config


def module_main(ctx):
    try:
        #Inizialization
        tstart = time.clock()
        print(SEPARATOR)
        
        #Create Image object
        img =VividImage.from_multidimimage(a3.inputs['Input Image'])
        
        # Creatre LogText and start logging
        print('Thresholding: '+img.metadata['Name'])
        print('Method: Manual')
        
        #Set upper and lower threshold
        upper=np.amax([a3.inputs['Upper'], a3.inputs['Lower']]) 
        lower=np.amin([a3.inputs['Upper'], a3.inputs['Lower']])
        print('Settings: \n\t\tUpper:%s \n\t\tLower:%s' % (str(upper),str(lower)))
        
        #Run thresholding
        print('Autothresholding started!')
        output_img=VividImage(threshold_manual(img.array, upper, lower), img.metadata)

        #Set output
        a3.outputs['Output Image']=output_img.to_multidimimage()
    
        #Finalization
        tstop = time.clock()
        print('Processing finished in ' + str((tstop - tstart)) + ' seconds!')
        print('Manual thresholding was successfully!')
        print(SEPARATOR)
    
    except Exception as e:
        raise error("Error occured while executing '"+str(ctx.type())+"' module '"+str(ctx.name())+"' !",exception=e)

a3.def_process_module(generate_config(), module_main)
