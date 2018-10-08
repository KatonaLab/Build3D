import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.imageclass import VividImage
from modules.a3dc_modules.a3dc.interface import colocalization, save_data, save_image#, apply_filter
from modules.a3dc_modules.a3dc.core import filter_database
from modules.a3dc_modules.a3dc.utils import quote, SEPARATOR, error, warning, value_to_key

import os
import math
import sys
import time

from modules.a3dc_modules.a3dc.multidimimage import from_multidimimage, to_multidimimage


CHFILTERS=['ChA totalOverlappingRatio', 'ChB totalOverlappingRatio']#,'ChA colocalizationCount','ChB colocalizationCount']#['Ch1 totalOverlappingRatio', 'Ch2 totalOverlappingRatio','Ch1 colocalizationCount','Ch2 colocalizationCount']
OVLFILTERS=[ 'volume']#,'Ch1 overlappingRatio','Ch2 overlappingRatio']

TRANSLATE={'volume':'Overlapping volume', 'ChA totalOverlappingRatio':'ChA Overlapping ratio', 'ChB totalOverlappingRatio':'ChB Overlapping ratio'}
DEFAULT_VALUE={'volume':float(math.inf), 'ChA totalOverlappingRatio':1.0, 'ChB totalOverlappingRatio':1.0}
#Generate filter list. 
#Sort so the input fields come in the appropriate order
FILTERS = sorted(OVLFILTERS+CHFILTERS, key=str.lower)

def colocalize(ch1_img, ch2_img, ch1_settings, ch2_settings, ovl_settings, path, show=True, to_text=False, remove_filtered=False):
    
    tagged_img_list=[ch1_img, ch2_img]
    print('Processing the following channels: '+ str([img.metadata['Name'] for img in tagged_img_list]))
    print('Filter settings: ' + str(ovl_settings))
    
    #Run colocaliyation analysis
    ovl_img, _=colocalization(tagged_img_list, overlapping_filter=ovl_settings, remove_filtered=remove_filtered)
    
    #Run filtering steps
    ch1_img.database=filter_database(ch1_img.database, ch1_settings, overwrite=True)
    ch2_img.database=filter_database(ch2_img.database, ch2_settings, overwrite=True)
    #ch1_img, _ =apply_filter(ch1_img, ch1_settings, overwrite=False, remove_filtered=remove_filtered)
    #ch2_img, _ =apply_filter(ch2_img, ch2_settings, overwrite=False, remove_filtered=remove_filtered)
    
    #Print number of objects to logText
    print('Number of Overlapping Objects: '+str(len(ovl_img.database['tag'])))            
    
    
    #Set path and filename
    outputPath=os.path.join(path, 'Output')
    if not os.path.exists(outputPath):
        os.makedirs(outputPath)            
    
    #Generate output filename
    filename_1=os.path.basename(ch1_img.metadata['Path'])
    filename_2=os.path.basename(ch1_img.metadata['Path'])
    if filename_1!=filename_2:
        
        basename=os.path.splitext(filename_1)[0]+'_'+os.path.splitext(filename_2)[0]
    else:
        basename=os.path.splitext(filename_1)[0]
        
    #Save databases
    print('Saving object dataBases!')
    file_name=basename+'_'+ch1_img.metadata['Name']+'_'+ch2_img.metadata['Name']
    
    #Get extension
    if to_text==True:
        extension=".txt"    
    else:
        extension=".xlsx"
    
    #If filename exists generate a neme that is not used
    i=1
    final_name=file_name
    while os.path.exists(os.path.join(outputPath, final_name+extension)):
        final_name=file_name+'_'+str('{:03d}'.format(i))
        i += 1
    if i!=1:
        file_name=final_name
        warning('Warning: Filename already exists!! Data will be saved to '+file_name+extension)

    #Save data and give output path
    save_data([ch1_img, ch2_img ,ovl_img], path=outputPath, file_name=file_name, to_text=to_text)
    
    #Save images
    print('Saving output images!')
    image_list=[ch1_img, ch2_img, ovl_img]
    name_img = basename+'{}_{}.ome.tiff'.format(ch1_img.metadata['Name'],ch2_img.metadata['Name'])
    
    save_image(image_list, outputPath, name_img)
    
    #Create outputpath ox data
    output_path=os.path.join(outputPath, file_name+extension)
    
    return ovl_img,  output_path  



def read_params(filters=FILTERS):
    
    out_dict = {}
    out_dict['Path']=os.path.dirname(a3.inputs['Path'].path)

    out_dict['ChA Image']=from_multidimimage(a3.inputs['ChA Image'],a3.inputs['ChA DataBase'])
    out_dict['ChB Image']=from_multidimimage(a3.inputs['ChB Image'],a3.inputs['ChB DataBase'])
    
    out_dict['to_text']=a3.inputs['Save to xlsx/text']
    out_dict['remove_filtered']=a3.inputs['Keep/Remove filtered objects']
    
    #Load parameters
    settings = {}
    for f in [TRANSLATE[key] for key in FILTERS]:
        settings[value_to_key(TRANSLATE,f)] = {}
        for m in ['min', 'max']:
            settings[value_to_key(TRANSLATE,f)][m] = a3.inputs['{} {}'.format( f, m)]
            
    #Generate channel 1 and channel 2 settings dictionary
    ch1_settings={}
    ch2_settings={}
    for key in CHFILTERS:
        if key in settings:
            prefix=key.split('_', 1)[0]
            filter_key=key.split('_', 1)[-1]
            
            if prefix=='ChA':
                ch1_settings[filter_key] = settings[key]
            if prefix=='ChB':
                ch2_settings[filter_key] = settings[key]
    
    out_dict['ChA'] = ch1_settings
    out_dict['ChB'] = ch2_settings
            
    #Generate overlapping settings dictionary
    ovl_settings={}
    for key in OVLFILTERS:

        if key in settings:
            
            prefix=key.split('_', 1)[0]
            filter_key=key.split('_', 1)[-1]
            
            if prefix=='ChA':
                ovl_settings[filter_key+' in '+str(a3.inputs['ChA MetaData']['Name'])] = settings[key]
     
            if prefix=='ChB':
                ovl_settings[filter_key+' in '+str(a3.inputs['ChB MetaData']['Name'])] = settings[key]

        else:
            ovl_settings[key] = settings[key]
    
    if a3.inputs['Volume in pixels/um\u00B3'] and ('volume' in ovl_settings.keys()):
        
        #Check if physical size metadata is available  if any is missing raise Exeption
        size_list=['PhysicalSizeX','PhysicalSizeY', 'PhysicalSizeZ']
        missing_size=[s for s in size_list if s not in out_dict['ChA Image'].metadata.keys()]
        if len(missing_size)!=0:
            raise Exception('Missing :'+str(missing_size)+'! Unable to carry out analysis!')

        #Check if unit metadata is available, default Unit is um!!!!!!!!
        unit_list=['PhysicalSizeZUnit', 'PhysicalSizeZUnit', 'PhysicalSizeZUnit']
        missing_unit=[u for u in unit_list if u not in out_dict['ChA Image'].metadata.keys()]
        if len(missing_size)!=0:
            print('Warning: DEFAULT value (um or micron) used for :'
                 +str(missing_unit)+'!', file=sys.stderr)        
        
        #Set default Unit values if not in metadata
        #Remember that if unit value is missing an exception is raised
        for un in missing_unit:
            out_dict['ChA Image'].metadata[un]='um'
            out_dict['ChB Image'].metadata[un]='um'
        
        print('Physical voxel volume is : '
              +str(out_dict['ChA Image'].metadata['PhysicalSizeX']*out_dict['ChA Image'].metadata['PhysicalSizeY']*out_dict['ChA Image'].metadata['PhysicalSizeZ'])
              +' '+out_dict['ChA Image'].metadata['PhysicalSizeXUnit']+'*'+out_dict['ChA Image'].metadata['PhysicalSizeYUnit']+'*'+out_dict['ChA Image'].metadata['PhysicalSizeZUnit'])
        
  
    else:
        ovl_settings['voxelCount'] = settings.pop('volume')
    
    
    
    print(ovl_settings)
    out_dict['Ovl'] = ovl_settings    
    
    #out_dict['FileName']=a3.inputs['FileName']

    return out_dict    
    
def module_main(ctx):
    
    try:   
        #Inizialization
        tstart = time.clock()
        print(SEPARATOR)
        print('Colocalization analysis started!')
        
        #Read Parameters
        print('Reading input parameters!')
        params = read_params()
        
        output=colocalize(params['ChA Image'],
                   params['ChB Image'],
                   params['ChA'],
                   params['ChB'],
                   params['Ovl'], 
                   params['Path'],
                   to_text=params['to_text'], remove_filtered=params['remove_filtered'])
        
        a3.outputs['Overlapping Image'] = to_multidimimage(output[0])
        a3.outputs['Overlapping Binary'] = to_multidimimage(VividImage(output[0].image>0,output[0].metadata))
        a3.outputs['Overlapping DataBase'] =output[0].database
        
        path=a3.Url()
        path.path=output[1]
        a3.outputs['Overlapping Path'] = path

        #Finalization
        tstop = time.clock()
        print('Processing finished in ' + str((tstop - tstart)) + ' seconds! ')
        print('Object analysis was run successfully!')
        print(SEPARATOR)
        quote(verbose=True)  
        print(SEPARATOR)
    
    except IOError as e:
        print("Warning: Failed to write to file!!", file=sys.stderr)
        print(str(e), file=sys.stderr)
    
    except Exception as e:
        error("Error occured while executing '"+str(ctx.type())+"' module '"+str(ctx.name())+"' !", exception=e)

 
def generate_config(filters=FILTERS):

    #Set Outputs and inputs
    config=[a3.Input('Path', a3.types.url),
        a3.Input('ChA Image', a3.types.ImageFloat), 
        a3.Input('ChA DataBase', a3.types.GeneralPyType), 
        a3.Input('ChB Image', a3.types.ImageFloat),
        a3.Input('ChB DataBase', a3.types.GeneralPyType),              
        a3.Output('Overlapping Image', a3.types.ImageFloat),
        a3.Output('Overlapping Binary', a3.types.ImageFloat),
        a3.Output('Overlapping DataBase', a3.types.GeneralPyType),
        a3.Output('Overlapping Path', a3.types.url)]    
    
    #Set parameters
    for f in filters:
        for m in ['min', 'max']:
            print(DEFAULT_VALUE[f])
            config.append(
                a3.Parameter('{} {}'.format(TRANSLATE[f], m), a3.types.float)
                .setFloatHint('default', 0 if m == 'min' else DEFAULT_VALUE[f])
                .setFloatHint('default', 0 if m == 'min' else DEFAULT_VALUE[f])
                .setFloatHint('unusedValue',0 if m == 'min' else DEFAULT_VALUE[f]))
    
    
    switch_list=[a3.Parameter('Keep/Remove filtered objects', a3.types.bool).setBoolHint("default", False),
             a3.Parameter('Volume in pixels/um\u00B3', a3.types.bool).setBoolHint("default", False),
             a3.Parameter('Save to xlsx/text', a3.types.bool)]
    config.extend(switch_list)
 
    
    return config





a3.def_process_module(generate_config(), module_main)
