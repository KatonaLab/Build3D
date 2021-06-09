import time
import a3dc_module_interface as a3
import modules.packages.a3dc.core as core  
from modules.packages.a3dc.ImageClass import ImageClass
from modules.a3dc_interface_utils import error, warning, print_line_by_line, SEPARATOR
import a3dc_module_interface as md

import os



def module_main(ctx):

    try:

        filename = a3.inputs['FileName'].path

        #Inizialization
        tstart = time.clock()
        print(SEPARATOR)
        print('Loading the following image: ', filename)
        
        #Process Files
        directory=os.path.dirname(filename)
        extension = os.path.splitext(filename)[1]
        
        file_list = [f for f in os.listdir(directory) if (os.path.isfile(os.path.join(directory, f)) and os.path.splitext(f)[1]==extension)]
        
        #Thresholding Methods
        METHODS=['Huang', 'IsoData', 'IsoData_skimage', 'KittlerIllingworth', 'Li','Li_skimage',  'MaxEntropy','MaxEntropy_skimage','Moments','Otsu', 'Otsu_skimage', 'RenyiEntropy', 'Shanbhag','Triangle', 'Triangle_skimage', 'Yen','Yen_skimage']
        print('Filename '+'meanIntensity sumIntensity '+str(METHODS).replace('[','').replace(']','').replace(',',''))
        for name in file_list:
            
            
            #Load and reshape image
            img=ImageClass.load(os.path.join(directory, name))
                
            #Print important image parameters
            #print_line_by_line(str(img))
            
            ch_1_Nb=a3.inputs['Channel A']-1
            ch_1=img.get_dimension(ch_1_Nb, 'C')
            ch_1.metadata['Path']=filename
            ch_1.reorder('ZYXCT')
       
            
            #Run thresholding
            res=name
            
            #Analyze raw image parameters
            raw_data=core.analyze_raw(ch_1)
            
            res+=' '+str(raw_data['meanIntensity'])+' '+str(raw_data['sumIntensity'])
            
            for meth in METHODS: 
                ch_1_thr, thr_value=core.threshold(ch_1, meth, mode='Stack')
                res+=' '+str(thr_value)
                
            print(res)
        
        #Output
        a3.outputs['Raw Image'] = ch_1.to_multidimimage()
        a3.outputs['Thresholded Image']=ch_1_thr.to_multidimimage()
        
        #Finalization
        tstop = time.clock()
        print('Processing finished in ' + str((tstop - tstart)) + ' seconds! ')
        print('Image loaded successfully!')
        print(SEPARATOR)
        
    except Exception as e:
        raise error("Error occured while executing '"+str(ctx.type())+"' module '"+str(ctx.name())+"' !",exception=e)


    
config = [a3.Input('FileName', a3.types.url),
          a3.Parameter('Channel A', a3.types.int8)
                .setIntHint('default', 1)
                #.setIntHint('max', 8)
                .setIntHint('min', 1),
                #.setIntHint('unusedValue', 1),
          a3.Output('Raw Image', a3.types.ImageFloat),
          a3.Output('Thresholded Image', a3.types.ImageFloat)]
    

a3.def_process_module(config, module_main)
