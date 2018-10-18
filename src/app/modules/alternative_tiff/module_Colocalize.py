import os
import math
import sys
import time
import a3dc_module_interface as a3
from modules.packages.a3dc.interface import colocalization, save_data, save_image, apply_filter
#from modules.packages.a3dc.core import filter_database
from modules.packages.a3dc.utils import quote, error, warning, get_next_filename, value_to_key, dictinary_equal,  rename_duplicates
from modules.packages.a3dc.utils import VividImage
from modules.packages.a3dc.constants import SEPARATOR


CHFILTERS=['ChA totalOverlappingRatio', 'ChB totalOverlappingRatio']#,'ChA colocalizationCount','ChB colocalizationCount']#['Ch1 totalOverlappingRatio', 'Ch2 totalOverlappingRatio','Ch1 colocalizationCount','Ch2 colocalizationCount']
OVLFILTERS=[ 'volume']#,'Ch1 overlappingRatio','Ch2 overlappingRatio']

TRANSLATE={'volume':'Overlapping volume', 'ChA totalOverlappingRatio':'ChA Overlapping ratio', 'ChB totalOverlappingRatio':'ChB Overlapping ratio'}
DEFAULT_VALUE={'volume':float(math.inf), 'ChA totalOverlappingRatio':1.0, 'ChB totalOverlappingRatio':1.0}

#Generate filter list. Sort so the input fields come in the appropriate order
FILTERS = sorted(OVLFILTERS+CHFILTERS, key=str.lower)

def colocalize(ch1_img, ch2_img, ch1_settings, ch2_settings, ovl_settings, path, show=True, to_text=False, remove_filtered=False):
    
    tagged_img_list=[ch1_img, ch2_img]
    
    #Gennerate list of names without duplicates and change 'Name' field in image metadata
    name_list = rename_duplicates([x.metadata['Name'] for x in tagged_img_list])
    
    for idx, value in enumerate(name_list):
        tagged_img_list[idx].metadata['Name']=name_list[idx]
        
        
    print('Processing the following channels: '+ str([img.metadata['Name'] for img in tagged_img_list]))
    print('Filter settings: ' + str(ovl_settings))
    
    #Run colocaliyation analysis
    ovl_img, _=colocalization(tagged_img_list, overlapping_filter=ovl_settings, remove_filtered=remove_filtered)
    
    #Run filtering steps
    #ch1_img.database=filter_database(ch1_img.database, ch1_settings, overwrite=True)
    #ch2_img.database=filter_database(ch2_img.database, ch2_settings, overwrite=True)
    ch1_img, _ =apply_filter(ch1_img, ch1_settings, overwrite=False, remove_filtered=False)
    ch2_img, _ =apply_filter(ch2_img, ch2_settings, overwrite=False, remove_filtered=False)
    
    #Print number of objects to logText
    print('Number of Overlapping Objects: '+str(len(ovl_img.database['tag'])))            
    
    
    #Set path and filename
    output_path=os.path.join(path, 'Output')
    if not os.path.exists(output_path):
        os.makedirs(output_path)            
        
    #Generate output filename
    filename_1=os.path.basename(ch1_img.metadata['Path'])
    filename_2=os.path.basename(ch2_img.metadata['Path'])
    if filename_1!=filename_2:
        
        basename=os.path.splitext(filename_1)[0]+'_'+os.path.splitext(filename_2)[0]
    else:
        basename=os.path.splitext(filename_1)[0]
        
    #Save data and give output path
    print('Saving object dataBases!')
    data_basename=basename+'_'+ch1_img.metadata['Name']+'_'+ch2_img.metadata['Name']

    #Get extension
    if to_text==True:
        extension=".txt"    
    else:
        extension=".xlsx"
    
    #If filename exists generate a neme that is not used
    file_name=get_next_filename(output_path, data_basename+extension)

    #If filename exists issue warning
    if file_name!=data_basename+extension:
        warning('Warning: Filename already exists!! Data will be saved to '+file_name)

    #Save to file
    save_data([ch1_img, ch2_img ,ovl_img], path=output_path, file_name=os.path.splitext(file_name)[0], to_text=to_text)
    
    
    #Save images
    print('Saving output images!')
    image_list=[ch1_img, ch2_img, ovl_img]
    name_img = basename+'_{}_{}.ome.tiff'.format(ch1_img.metadata['Name'],ch2_img.metadata['Name'])
    
    save_image(image_list, output_path, name_img)
    
    return ovl_img, ch1_img, ch2_img,  os.path.join(output_path, file_name)    


def read_params(filters=FILTERS):
    
    out_dict = {}
    
    #Get Path. If "Output Path" is not set or does not exist use "File Path".
    if  os.path.isdir(a3.inputs['Output Path'].path):
        out_dict['Path']=a3.inputs['Output Path'].path
    else:
        out_dict['Path']=os.path.dirname(a3.inputs['File Path'].path)

    out_dict['ChA Image']=VividImage(a3.inputs['ChA Image'].image, a3.inputs['ChA Image'].metadata ,a3.inputs['ChA DataBase'])
    out_dict['ChB Image']=VividImage(a3.inputs['ChB Image'].image, a3.inputs['ChB Image'].metadata ,a3.inputs['ChB DataBase'])
    
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
            prefix=key.split(' ', 1)[0]
            filter_key=key.split(' ', 1)[-1]
            
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
            
            prefix=key.split(' ', 1)[0]
            filter_key=key.split(' ', 1)[-1]
            
            if prefix=='ChA':
                ovl_settings[filter_key+' in '+str(a3.inputs['ChA MetaData']['Name'])] = settings[key]
     
            if prefix=='ChB':
                ovl_settings[filter_key+' in '+str(a3.inputs['ChB MetaData']['Name'])] = settings[key]

        else:
            ovl_settings[key] = settings[key]
    
    if a3.inputs['Volume in pixels/um\u00B3'] and ('volume' in settings.keys()):
        #Get size and unit metadata and check if the two channels have the same 
        #metadata and raise error if not available         
        
        
        #Create a dictionary of pixel sizes for the two cahnnels
        size_list=['PhysicalSizeX','PhysicalSizeY', 'PhysicalSizeZ']
        ch1_size={key:out_dict['ChA Image'].metadata[key] for key in size_list if key in out_dict['ChA Image'].metadata.keys()}
        ch2_size={key:out_dict['ChB Image'].metadata[key] for key in size_list if key in out_dict['ChB Image'].metadata.keys()}
        #Add defaul unit if not available
        for key in size_list:
            if key not in ch1_size:
                ch1_size[key]=1.0
                warning('Channel 1 '+key+' unavailable! The default value of 1.0 will be used!')
            if key not in ch1_size:
                ch2_size[key]=1.0
                warning('Channel 2 '+key+' unavailable! The default value of 1.0 will be used!')
        #Check if the two channels have the same unit dictionary
        if not dictinary_equal(ch1_size,ch2_size): #or len(ch1_size.keys())!=3 or len(ch1_size.keys())!=3:
            raise Exception('Two channels have to have the same size metadata! Channel 1: {}  and Channel 2: {}'.format(ch1_size, ch2_size))                
                
                
        #Create a dictionary of units for the two cahnnels
        unit_list=['PhysicalSizeZUnit', 'PhysicalSizeZUnit', 'PhysicalSizeZUnit']
        ch1_unit={key:out_dict['ChA Image'].metadata[key] for key in unit_list if key in out_dict['ChA Image'].metadata.keys()}
        ch2_unit={key:out_dict['ChB Image'].metadata[key] for key in unit_list if key in out_dict['ChB Image'].metadata.keys()}
        
        #Add defaul unit if not available
        for key in unit_list:
            if key not in ch1_unit:
                ch1_unit[key]='um'
                warning('Channel 1 '+key+' unavailable! The default unit um will be used!')
            if key not in ch1_unit:
                ch2_unit[key]='um'
                warning('Channel 2 '+key+' unavailable! The default unit um will be used!')
        #Check if the two channels have the same unit dictionary
        if not dictinary_equal(ch1_unit, ch2_unit):
            raise Exception('Two channels have to have the same unit metadata! Channel 1: {}  and Channel 2: {}'.format(ch1_unit, ch2_unit))
               
        print('Physical voxel volume is : '
              +str(out_dict['ChA Image'].metadata['PhysicalSizeX']*out_dict['ChA Image'].metadata['PhysicalSizeY']*out_dict['ChA Image'].metadata['PhysicalSizeZ'])
              +' '+out_dict['ChA Image'].metadata['PhysicalSizeXUnit']+'*'+out_dict['ChA Image'].metadata['PhysicalSizeYUnit']+'*'+out_dict['ChA Image'].metadata['PhysicalSizeZUnit'])
                
        ovl_settings['volume']= settings.pop('volume')
        
    else:
        ovl_settings['voxelCount'] = settings.pop('volume')
    
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
            
        a3.outputs['Overlapping Image'] = output[0]
        a3.outputs['Overlapping Binary'] = VividImage(output[0].image>0,output[0].metadata)
        a3.outputs['Overlapping DataBase'] =output[0].database
        #a3.outputs['Channel A Image']=to_multidimimage(VividImage(output[1].image>0,output[1].metadata))
        #a3.outputs['Channel B Image']=to_multidimimage(VividImage(output[2].image>0,output[2].metadata))        
        
        path=a3.Url()
        path.path=output[3]
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
    config=[a3.Input('File Path', a3.types.url),
        a3.Input('Output Path', a3.types.url),
        a3.Input('ChA Image', a3.types.GeneralPyType), 
        a3.Input('ChA DataBase', a3.types.GeneralPyType), 
        a3.Input('ChB Image', a3.types.GeneralPyType),
        a3.Input('ChB DataBase', a3.types.GeneralPyType),
        #a3.Output('Channel A Image', a3.types.ImageFloat),
        #a3.Output('Channel B Image', a3.types.ImageFloat),
        a3.Output('Overlapping Image', a3.types.GeneralPyType),
        a3.Output('Overlapping Binary', a3.types.GeneralPyType),
        a3.Output('Overlapping DataBase', a3.types.GeneralPyType),
        a3.Output('Overlapping Path', a3.types.url)] 
    
    
    #Set parameters
    for f in filters:
        for m in ['min', 'max']:
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
