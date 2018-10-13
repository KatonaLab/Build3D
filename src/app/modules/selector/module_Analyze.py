import time
import math
import sys
import a3dc_module_interface as a3
from modules.packages.a3dc.interface import tagImage, analyze, apply_filter
from modules.packages.a3dc.utils import SEPARATOR, error, value_to_key, VividImage

FILTERS = ['volume', 'meanIntensity']
TRANSLATE={'volume':'Volume', 'meanIntensity':'Mean intensity' }

          
def analyze_image(source, mask, settings, removeFiltered=False):

    print('Processing the following channels: '+ str(source.metadata['Name']))
    print('Filter settings: '+str(settings))
    
    #Parameters to measure
    measurementList = ['volume', 'voxelCount', 'centroid', 'pixelsOnBorder']
    
    #TEMP###########TEMP##############TEMP#################TEMP
    multi_img_keys = ['meanIntensity','medianIntensity', 'skewness', 'kurtosis', 'variance','maximumPixel',
                           'maximumValue', 'minimumValue','minimumPixel','centerOfMass','standardDeviation',
                           'cumulativeIntensity','getWeightedElongation','getWeightedFlatness','getWeightedPrincipalAxes',
                           'getWeightedPrincipalMoments']
    
    for key in settings:
        if key in multi_img_keys:
            settings[str(key)+' in '+str(source.metadata['Name'])] = settings[key]
            del settings[key]

    #Tagging Image
    print('Running connected components!')
    taggedImage, _ = tagImage(mask)
    
    # Analysis and Filtering of objects
    print('Analyzing tagged image!')
    taggedImage, _ = analyze(taggedImage, image_list=[source], measurementInput=measurementList)
    
    print('Filtering object database!')
    taggedImage, _ = apply_filter(taggedImage, filter_dict=settings, remove_filtered=removeFiltered)#{'tag':{'min': 2, 'max': 40}}
        
    return taggedImage


def read_params(filters=[TRANSLATE[key] for key in FILTERS]):
    
    params = {'Source': VividImage.from_multidimimage(a3.inputs['Source Image']),
                    'Mask':VividImage.from_multidimimage(a3.inputs['Mask Image'])}

    settings = {}
    for f in filters:
        settings[value_to_key(TRANSLATE,f)] = {}
        for m in ['min', 'max']:
            settings[value_to_key(TRANSLATE,f)][m] = a3.inputs['{} {}'.format( f, m)]
    
    if a3.inputs['Filter objects on border']:       
        settings['pixelsOnBorder']={'min': 1, 'max':float(math.inf)}

    if a3.inputs['Volume in pixels/um\u00B3'] and ('volume' in settings.keys()):
        
        #Check if physical size metadata is available  if any is missing raise Exeption
        size_list=['PhysicalSizeX','PhysicalSizeY', 'PhysicalSizeZ']
        missing_size=[s for s in size_list if s not in params['Source'].metadata.keys()]
        if len(missing_size)!=0:
            raise Exception('Missing :'+str(missing_size)+'! Unable to carry out analysis!')

        #Check if unit metadata is available, default Unit is um!!!!!!!!
        unit_list=['PhysicalSizeZUnit', 'PhysicalSizeZUnit', 'PhysicalSizeZUnit']
        missing_unit=[u for u in unit_list if u not in params['Source'].metadata.keys()]
        if len(missing_size)!=0:
            print('Warning: DEFAULT value (um or micron) used for :'
                 +str(missing_unit)+'!', file=sys.stderr)        
        
        #Set default Unit values if not in metadata
        #Remember that if unit value is missing an exception is raised
        for un in missing_unit:
            params['Source'].metadata[un]='um'
            params['Mask'].metadata[un]='um'
        
        print('Physical voxel volume is : '
              +str(params['Source'].metadata['PhysicalSizeX']*params['Source'].metadata['PhysicalSizeY']*params['Source'].metadata['PhysicalSizeZ'])
              +' '+params['Source'].metadata['PhysicalSizeXUnit']+'*'+params['Source'].metadata['PhysicalSizeYUnit']+'*'+params['Source'].metadata['PhysicalSizeZUnit'])
        
  
    else:
        settings['voxelCount'] = settings.pop('volume')
    
    params['Settings'] = settings
    
    params['removeFiltered']=a3.inputs['Keep/Remove filtered objects']

    return params    
    

def generate_config(filters=[TRANSLATE[key] for key in FILTERS]):
    
    #Set Outputs and inputs
    config = [a3.Input('Source Image', a3.types.ImageFloat),
             a3.Input('Mask Image', a3.types.ImageFloat),
             a3.Output('Analyzed Image', a3.types.ImageFloat),
             a3.Output('Analyzed Binary', a3.types.ImageFloat),  
             a3.Output('Analyzed Database', a3.types.GeneralPyType)]

    #Set parameters 
    for f in filters:
        for m in ['min', 'max']:
            config.append(
                a3.Parameter('{} {}'.format(f, m), a3.types.float)
                .setFloatHint('default', 0 if m == 'min' else float(math.inf))
                .setFloatHint('unusedValue',0 if m == 'min' else float(math.inf)))
    
    switch_list=[a3.Parameter('Keep/Remove filtered objects', a3.types.bool).setBoolHint("default", False),
                 a3.Parameter('Filter objects on border', a3.types.bool).setBoolHint("default", False),
                 a3.Parameter('Volume in pixels/um\u00B3', a3.types.bool).setBoolHint("default", False)]
    config.extend(switch_list)
 
    return config

def module_main(ctx):
    try:
        #Inizialization
        tstart = time.clock()
        print(SEPARATOR)
        print('Object analysis started!')
        
        #Read Parameters
        print('Reading input parameters!')
        params = read_params()
        
        output=analyze_image(params['Source'],
                   params['Mask'],
                   params['Settings'],
                   params['removeFiltered'])
        
        #Change Name in metadata
        #output.metadata['Name']=params['Mask'].metadata['Name']+'_tagged'
        
        #Create Output
        a3.outputs['Analyzed Image'] = output.to_multidimimage()
        a3.outputs['Analyzed Binary'] = VividImage(output.image>0,output.metadata).to_multidimimage()
        a3.outputs['Analyzed Database']=output.database
        
        #Finalization
        tstop = time.clock()
        print('Processing finished in ' + str((tstop - tstart)) + ' seconds! ')
        print('Object analysis was run successfully!')
        print(SEPARATOR)

    except Exception as e:
        raise error("Error occured while executing '"+str(ctx.type())+"' module '"+str(ctx.name())+"' !",exception=e)
    




a3.def_process_module(generate_config(), module_main)
