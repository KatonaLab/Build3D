import os
import math
import numpy as np
import a3dc_module_interface as a3
import pandas as pd
import modules.packages.a3dc.core as core 
from modules.a3dc_interface import threshold, save_image
from modules.a3dc_interface_utils import error,print_line_by_line, SEPARATOR, METHODS
from collections import OrderedDict 

meth_list=METHODS+['All']


def module_threshold(image, method="Otsu", kwargs={}):

    #Threshold image
    output, logText = threshold(image, method, **kwargs)

    #Print logText
    print_line_by_line(logText)
    return output


def generate_config(methods=meth_list):
    
    methods.remove('Manual')
    #Set Outputs and inputs
    config = [a3.Input('Image', a3.types.GeneralPyType),
              a3.Input('File Path', a3.types.url),
              a3.Input('Output Path', a3.types.url),
              a3.Parameter('Channel', a3.types.int8)
                .setIntHint('default', 1)
                #.setIntHint('max', 8)
                .setIntHint('min', 1),
                #.setIntHint('unusedValue', 1), 
                ]

    #Set parameters
    param=a3.Parameter('Method', a3.types.enum)
    for idx, m in enumerate(methods):
        param.setIntHint(str(m), idx)
    config.append(param)
         
    
    config.append(a3.Parameter('Manual Threshold Value', a3.types.float)
            .setFloatHint('default', float(math.inf)))
            #.setFloatHint('unusedValue', float(math.inf))
            #.setFloatHint('stepSize', 1))
    config.append(a3.Parameter('Slice/Stack Histogram', a3.types.bool))
    config.append(a3.Parameter('Save Images', a3.types.bool)
            .setFloatHint('default', False))
    
    return config


def module_main(ctx, methods=meth_list):

    try: 
        
        #Get file name and output path. If "Output Path" is not set or does not exist use "File Path".
        file_name = a3.inputs['File Path'].path
        if  os.path.isdir(a3.inputs['Output Path'].path):
            out_path=a3.inputs['Output Path'].path
        else:
            out_path=os.path.join(os.path.dirname(file_name), 'Output')
        
        out_file_path=os.path.join(out_path, 'Thresholder_results.xlsx')

        #Create output directory if does not exist
        if not os.path.exists(out_path):
            os.makedirs(out_path)
        #If excel file with results from previos run or previous batch steps 
        #load as dataframe   
        if not os.path.exists(out_file_path) or not os.path.isfile(out_file_path):
            results=pd.DataFrame()    
        else:
            results=pd.read_excel(out_file_path, 0)
           
        #Run threshold
        #Get method and mode. Get kwargs if method is manual
        method=methods[a3.inputs['Method'][-1]]
        if method=='Manual':
            kwargs={'lower':0, 'upper':a3.inputs['Manual Threshold Value']}
        elif method=='None':
            method='Manual'
            kwargs={'lower':0, 'upper':0}
        else:
            kwargs={}
            if a3.inputs['Slice/Stack Histogram']:
                kwargs['mode']='Stack'
            else:
                kwargs['mode']='Slice'

        
        results_dict={}
    
        if method=='All':
            methods.remove('All')
        else:
           methods=[method]
        
        #Analyze raw image parameters
        raw_data=core.analyze_raw(a3.inputs['Image'])
        
        #Create ordered dict for results and run analysis
        results_dict=OrderedDict()
         
        results_dict['File']=os.path.basename(file_name)
        for keys in raw_data.keys():
            results_dict[keys]=raw_data[keys]
        
        
        for meth in methods:
            
            image_thresholded, thr_value=core.threshold(a3.inputs['Image'], meth, **kwargs)
            
            if a3.inputs['Save Images']:
                save_image([image_thresholded], out_path, str(os.path.splitext(os.path.basename(file_name))[0]+'_'+str(meth)+'.tif'))
    
            
            if kwargs['mode']=='Slice':
                print('Thresholds using '+str(meth)+':')
                print('Slice thresholds: ', thr_value)
                thr_value=np.mean(thr_value)
                print('Mean threshold: ', thr_value)
                
            if kwargs['mode']=='Stack':
                print('Threshold using '+str(meth)+': ',thr_value)
            
            print('')
                
            results_dict[meth]=thr_value
        
        
        #Append results_dict to dataframe and save
        results=results.append(pd.DataFrame(results_dict, index=[0]))
        
        writer=pd.ExcelWriter(out_file_path, engine='xlsxwriter')
        results.to_excel(writer, index=False, sheet_name='Thresholds')
        writer.save()

        
    except Exception as e:
        raise error("Error occured while executing '"+str(ctx.type())+"' module '"+str(ctx.name())+"' !",exception=e)
    
a3.def_process_module(generate_config(), module_main)
