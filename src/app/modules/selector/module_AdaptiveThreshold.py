import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.segmentation import threshold_adaptive
from modules.a3dc_modules.a3dc.imageclass import Image
import time
from modules.a3dc_modules.a3dc.utils import SEPARATOR, error
from modules.a3dc_modules.a3dc.multidimimage import from_multidimimage, to_multidimimage

METHODS=['Mean', 'Gaussian']



def adaptive_threshold(image, method , blocksize=5, offset=0):

    print('Thresholding: '+image.metadata['Name'])
    print('Method: Adaptive' + method)
    print('Settings: \n\t\tBlocksize:%s \n\t\tOffset:%s' % (str(blocksize),str(offset)))
    
    outputArray = threshold_adaptive(image.array, method, blocksize, offset)

    return Image(outputArray, image.metadata)

def init_config(methods=METHODS):
 
    config = [ a3.Input('Input Image', a3.types.ImageFloat)]

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
    
    config.append(a3.Output('Output Image', a3.types.ImageFloat)) 
    
    return config



def module_main(ctx):
    try:
        #Inizialization
        tstart = time.clock()
        print(SEPARATOR)
        print('Adaptive thresholding started!')
        
        #Create Image object
        img =from_multidimimage(a3.inputs['Input Image'])
    
        
        #Get method and mode
        method=METHODS[a3.inputs['Method'][-1]]
    
        #Run thresholding            
        output_img=adaptive_threshold(img, method , blocksize=a3.inputs['BlockSize'], offset=a3.inputs['Offset'])
        
        #Change Name in metadata
        #output_img.metadata['Name']=img.metadata['Name']+'_adaptive_thr'
    
        #Set output
        a3.outputs['Output Image']=to_multidimimage(output_img)
        
        #Finalization
        tstop = time.clock()
        print('Processing finished in ' + str((tstop - tstart)) + ' seconds!')
        print('Adaptive thresholding was successfully!')
        print(SEPARATOR)
    
    except Exception as e:
        raise error("Error occured while executing '"+str(ctx.type())+"' module '"+str(ctx.name())+"' !",exception=e)
        
config = init_config()



a3.def_process_module(config, module_main)
