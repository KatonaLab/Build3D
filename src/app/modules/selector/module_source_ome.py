import a3dc_module_interface as a3
from modules.a3dc_modules.external.PythImage import Image
from modules.a3dc_modules.a3dc.utils import SEPARATOR, error, print_line_by_line
import time, os

from modules.a3dc_modules.a3dc.a3image import  image_to_a3image

def module_main(ctx):
    
    try:
        filename = a3.inputs['FileName'].path
        
        #Inizialization
        tstart = time.clock()
        print(SEPARATOR)
        print('Loading the following image: ', filename)
        
        #Load and reshape image
        img = Image.load(filename, file_type='ome')
        img.reorder('XYZCT')
        
        #Print important image parameters
        print_line_by_line(str(img))
        
        #Create Output
        a3.outputs['Array'] = img.image
        a3.outputs['MetaData']=img.metadata
        
        #Add path and filename to metadata
        a3.outputs['MetaData']['Path']=os.path.dirname(filename)
        a3.outputs['MetaData']['FileName']=os.path.basename(filename)
        
        #Finalization
        tstop = time.clock()
        print('Processing finished in ' + str((tstop - tstart)) + ' seconds! ')
        print('Image loaded successfully!')
        print(SEPARATOR)

    except Exception as e:
        raise error("Error occured while executing "+str(ctx.name())+" !",exception=e)

config = [a3.Input('FileName', a3.types.url),
    a3.Output('Array', a3.types.GeneralPyType),
    a3.Output('MetaData', a3.types.GeneralPyType)]
    

a3.def_process_module(config, module_main)
