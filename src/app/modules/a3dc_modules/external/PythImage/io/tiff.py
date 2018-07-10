from skimage.external.tifffile import TiffFile, TiffWriter

def load(path, frames_per_sec=1, z_increment=1 ):

    
    with TiffFile(path) as tif:
     
        #Load metadata 
        metadata={'fps':frames_per_sec, 'spacing':z_increment }
        #for page in tif:
        for tag in tif[0].tags.values():

             metadata[tag.name]=tag.value

        
        images = tif.asarray() 

    return images, metadata


def convert_metadata(metadata_dict, shape, order):
    
    '''
    
    '''
    
    #ImageJ dimension order is'SXYCZT'
    metadata_dict_out={'DimensionOrder':order, 'SizeX':metadata_dict['image_width'], 'SizeY':metadata_dict['image_length']}
    
    #Add bitdepth annd SamplesPerPixel data to dictionary
    bit_depth_lookup={8:'uint8', 16:'uint16', 32:'float'}
    
    bits_per_sample=metadata_dict['bits_per_sample']
    if isinstance( bits_per_sample, int):
         metadata_dict_out['Type']=bit_depth_lookup[bits_per_sample]
         metadata_dict_out['SamplesPerPixel']=1
    
    elif isinstance(bits_per_sample, tuple):
        #Convert to set to determine if all values are equal
        if len(set(bits_per_sample))== 1:
            metadata_dict_out['Type']=bit_depth_lookup[bits_per_sample[0]]
        else:
            raise TypeError('All samples have to have the same bitdepth!')
        metadata_dict_out['SamplesPerPixel']=len(bits_per_sample)
    
    
    #Add remaining shape information
    key_lookup={'T':'SizeT', 'C':'SizeC', 'Z':'SizeZ', 'X':'SizeX', 'Y':'SizeY', 'S':'SamplesPerPixel'}
    for dim in order:
        if dim=='S':

            if metadata_dict_out['SamplesPerPixel']!=shape[-1*(1+order.index(dim))]:
              raise PythImageError('Invalid shape for dataset','')
            
        if dim=='X':                
            if metadata_dict_out['SizeX']!=shape[-1*(1+order.index(dim))]:
              raise PythImageError('Invalid shape for dataset','') 
    
        if dim=='Y':                
            if metadata_dict_out['SizeY']!=shape[-1*(1+order.index(dim))]:
              raise PythImageError('Invalid shape for dataset','')  
        
        metadata_dict_out[key_lookup[dim]]=shape[-1*(1+order.index(dim))]
            
            
    #Add physicalSize and units in X, Y and T to dictionary
    if 'x_resolution' in metadata_dict.keys():
       metadata_dict_out['PhysicalSizeX']=metadata_dict['x_resolution'][1]/metadata_dict['x_resolution'][0]     
       
    if 'y_resolution' in metadata_dict.keys():
       metadata_dict_out['PhysicalSizeY']=metadata_dict['y_resolution'][1]/metadata_dict['y_resolution'][0]
       
    if 'resolution_unit' in metadata_dict.keys():
        unit_dict={1:'None', 2:'inch', 3:'mm'}
        metadata_dict_out['PhysicalSizeXUnit']=unit_dict[metadata_dict['resolution_unit']]
        metadata_dict_out['PhysicalSizeYUnit']=unit_dict[metadata_dict['resolution_unit']]
    else:
        metadata_dict_out['PhysicalSizeXUnit']='inch'
        metadata_dict_out['PhysicalSizeYUnit']='inch'
    
    if 'fps' in metadata_dict.keys():
        metadata_dict_out['TimeIncrement']=1/metadata_dict['fps']
        metadata_dict_out['TimeIncrementUnit']='s'
   
    return metadata_dict_out