import time
import copy
import sys
import a3dc_module_interface as a3
from modules.packages.a3dc.utils import error, warning
from modules.packages.a3dc.core import VividImage
from modules.packages.a3dc.constants import SEPARATOR


def module_main(ctx):
    try:
        #Inizialization
        tstart = time.clock()
        print(SEPARATOR)
        
        #Load and reshape image
        ##TempTempTemp##
        img = VividImage(a3.inputs['Image'], copy.deepcopy(a3.inputs['MetaData']))
        #img = Image(a3.inputs['Image'], a3.inputs['MetaData'])
        img.reorder('XYZCT')
        
        #Get channel from image. 
        ch=a3.inputs['Channel']
        print('Loading the following channel: ', img.metadata['Name'][ch])
        if ch>=img.metadata['SizeC']:
            raise Exception('Image has %s channels! Invalid channel %s' % (str(img.metadata['SizeC']), str(ch)))
        
        #Check if image is time series
        if img.metadata['SizeT']>1:
            warning("Image is a time series! Only the first time step will be extracted!", file=sys.stderr)
        
        #Modify metadata 
        img.metadata['SamplesPerPixel']=img.metadata['SamplesPerPixel'][ch]
        img.metadata['Name']=img.metadata['Name'][ch]   
        #img.metadata['Path']=filename
        
        #Create Output
        #Extract channel from image array    
        a3.outputs['Channel 1'] = img.get_dimension(a3.inputs['Channel'], 'C').to_multidimimage()
        #to_multidimimage(Image(array.astype(np.float),copy.deepcopy(img.metadata)))
       
        #Finalization
        tstop = time.clock()
        print('Processing finished in ' + str((tstop - tstart)) + ' seconds! ')
        print('Image loaded successfully!')
        print(SEPARATOR)
    
    except Exception as e:
        raise error("Error occured while executing '"+str(ctx.type())+"' module '"+str(ctx.name())+"' !",exception=e)

config = [a3.Parameter('Channel', a3.types.int8)
                .setFloatHint('default', 0)
                .setFloatHint('unusedValue',0),
    a3.Input('Image', a3.types.GeneralPyType),
    a3.Input('MetaData', a3.types.GeneralPyType),
    a3.Output('Channel', a3.types.ImageFloat)
    ]

a3.def_process_module(config, module_main)
