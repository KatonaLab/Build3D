import time
import os
import math
import pandas as pd
import a3dc_module_interface as a3

from modules.a3dc_interface import threshold, save_image
from modules.a3dc_interface_utils import error,print_line_by_line, SEPARATOR, get_next_filename
from modules.packages.a3dc.ImageClass import ImageClass


METHODS=['Manual', 'Triangle', 'IsoData', 'MaxEntropy', 'Moments','RenyiEntropy','Huang', 'Li','KittlerIllingworth','Yen','Shanbhag','Otsu','None']


def threshold_to_text(path, filename, channel_name, method, channel_threshold):
    
    
    file_path=os.path.join(path, str(method)+"_threshold_run.txt")
    
    #open with a+ so file is created 
    with open( file_path, "a+") as text_file:
        
        text_file.write("File %s channel %s Thersholding Method: %s Threshold Value(s): %s \r\n" % ( filename,channel_name, method, str(channel_threshold) ))
    
    

def module_threshold(image, method="Otsu", kwargs={}):

    #Threshold image
    output, log_text = threshold(image, method, **kwargs)
    

    return output, log_text


def generate_config(methods=METHODS):
    
    #Set Outputs and inputs
    config = [a3.Input('File Path', a3.types.url),
              a3.Input('Output Path', a3.types.url),
              a3.Input('Input Image', a3.types.GeneralPyType),
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
    
    switch_list=[a3.Parameter('Save Threshold(s)', a3.types.bool).setBoolHint("default", True),
                 a3.Parameter('Save Image', a3.types.bool).setBoolHint("default", False)]
    config.extend(switch_list)
    
    
    return config


def module_main(ctx):
    
    try:
        #Inizialization
        tstart = time.process_time()
        print(SEPARATOR)
        print('Thresholding started!')
        
        #Create Image object
        img =a3.inputs['Input Image']
        print('Thresholding: '+img.metadata['Name'])
        
        
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
            print('Mode: ' +kwargs['mode'])
        
        print('Method: ' + method)
        
        #Run thresholding            
        output_img, log_text=module_threshold(img, method,kwargs)
        threshold_value=str(log_text.split('\n')[-1].split(':')[-1])

        print('Threshold value(s): ' + threshold_value)
        
        #Change Name in metadata
        output_img.metadata['Name']=img.metadata['Name']+'_auto_thr'
        
        
        
        if (a3.inputs['Save Threshold(s)'] or a3.inputs['Save Image']):
            
            #Set path and filename
            #Generate base directory
            if  os.path.isdir(a3.inputs['Output Path'].path):
                output_path=a3.inputs['Output Path'].path
            else:
                output_path=os.path.dirname(a3.inputs['File Path'].path)
                
            #Create otput directory
            output_path=os.path.join(output_path, 'Thresholding')
            if not os.path.exists(output_path):
                os.makedirs(output_path)            
                

            #Save threshold in file
            if a3.inputs['Save Threshold(s)']:
        
                print('Saving Threshold Values to file!')
                
                channel_name=img.metadata['Name']
                filename=a3.inputs['File Path'].path
                
                print('#########################################################################')
                print(threshold_value)
        
                threshold_to_text(output_path, filename, channel_name, method, threshold_value)
            
               
            #Save images
            #Generate output filename base for images
            filename_img=os.path.basename(output_img.metadata['Path'])
            basename_img=os.path.splitext(filename_img)[0]
            
            if a3.inputs['Save Image']:
        
                print('Saving output images!')
                #image_list=[ch1_img, ch2_img, ovl_img]
                name_img = basename_img+'_{}_{}.ome.tiff'.format( str(method), str(kwargs['mode']))
                
                
                save_image(output_img, output_path, name_img)
            
            
            
        #Set output
        a3.outputs['Thresholded Image']=output_img
      
        #Finalization
        tstop = time.process_time()
        print('Processing finished in ' + str((tstop - tstart)) + ' seconds!')
        print('Autothresholding was successfully!')
        print(SEPARATOR)
    
    except Exception as e:
        raise error("Error occured while executing '"+str(ctx.type())+"' module '"+str(ctx.name())+"' !",exception=e)
    
a3.def_process_module(generate_config(), module_main)
