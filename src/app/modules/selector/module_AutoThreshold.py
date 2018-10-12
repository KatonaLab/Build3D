import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.segmentation import threshold_auto
from modules.a3dc_modules.a3dc.utils import SEPARATOR, error, VividImage
import time


METHODS=['Triangle', 'IsoData', 'MaxEntropy', 'Moments','RenyiEntropy','Huang', 'Li','KittlerIllingworth','Yen','Shanbhag','Otsu']

def auto_threshold(image, method="Otsu", mode="Slice"):

    print('Thresholding: '+image.metadata['Name'])
    print('Method: ' + method)
    print('Mode: ' + mode)
    
    #Threshold image
    output_array, threshold_value = threshold_auto(image.array, method, mode)
    
    #Create metadata
    output_metadta=image.metadata
    output_metadta['Type']=output_array.dtype
    
    print('Threshold values: ' + str(threshold_value))

    return VividImage(output_array, output_metadta)


def generate_config(methods=METHODS):
 
    config = [ a3.Input('Input Image', a3.types.ImageFloat),
              a3.Output('Output Image', a3.types.ImageFloat)]

    param=a3.Parameter('Method', a3.types.enum)
    for idx, m in enumerate(methods):
        param.setIntHint(str(m), idx)
    
    config.append(param)
    
    config.append(a3.Parameter('Stack Histogram', a3.types.bool))
    
    return config




def module_main(ctx):
    try:
        #Inizialization
        tstart = time.clock()
        print(SEPARATOR)
        print('Autothresholding started!')
        
        #Create Image object
        img = VividImage.from_multidimimage(a3.inputs['Input Image'])
        
        
        #Get method and mode
        method=METHODS[a3.inputs['Method'][-1]]
          
        if a3.inputs['Stack Histogram']:
            mode='Stack'
        else:
            mode='Slice'
                        
        #Run thresholding            
        output_img=auto_threshold(img, method, mode)
        
        #Change Name in metadata
        #output_img.metadata['Name']=img.metadata['Name']+'_auto_thr'
        
        #Set output
        a3.outputs['Output Image']=output_img.to_multidimimage()
        #Finalization
        tstop = time.clock()
        print('Processing finished in ' + str((tstop - tstart)) + ' seconds!')
        print('Autothresholding was successfully!')
        print(SEPARATOR)
    
    except Exception as e:
        raise error("Error occured while executing '"+str(ctx.type())+"' module '"+str(ctx.name())+"' !",exception=e)
        

 


a3.def_process_module(generate_config(), module_main)
