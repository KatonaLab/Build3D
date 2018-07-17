from skimage.external.tifffile import TiffFile, TiffWriter
from ..RoiClass.imagej_tiff_meta import imagej_parse_overlay, imagej_create_roi, imagej_prepare_metadata
from ..RoiClass.roi import Roi

from .. import utils
import numpy as np

def load_image(path):
    '''
    Read imageJ Tiff file. ImageJ supports maximum 6D images with the following dimension order SXYCZT 
    (in order of incresing speed). Here S is samples per pixel  (for rgb images S=3). Returns ndarray
    with normalized shape where axes of unit length are also marked. Note that order of axis will be 
    from the slowest to the fastest changing as returned by TiffFile.
    '''
          
    with TiffFile(path) as tif:
       


        
        if not tif.is_imagej:
            raise TypeError('The file is corrupt or not an ImageJ tiff file!')
        
        #Load metadata
        metadata={}
        
        #load tags from the first page  in the tiff file
        for tag in tif[0].tags.values():
            
            # Unpack ImageJ image_description and imagej_metadata tiff tags
            if tag.name=='image_description':
                description={}
                #Convert bytes type to string, split
                lines=str(tag.value, 'utf-8').split()
                for ln in lines:
                    lines_split=ln.split('=')  
                    description[lines_split[0]]=lines_split[1]
                
                metadata['image_description']=description
              
            elif tag.name=='imagej_metadata':
            
                #imagej_parse_overlay accepts list of ndarrays 
                overlays=tif.pages[0].imagej_tags.overlays
                if isinstance(overlays, np.ndarray):
                    overlays=np.array([overlays], dtype='uint8')
                    metadata['imagej_metadata']={'overlays':imagej_parse_overlay(overlays)}
                else:
                    metadata['imagej_metadata']={'overlays':[imagej_parse_overlay(element) for element in overlays]}
            
            #Tiff tags            
            else:
 
                metadata[tag.name]=tag.value

        images = tif.asarray()
        '''
        #images = np.squeeze(tif.asarray())
        print('########################################')
        print(tif.pages[0].imagej_tags.overlays[0])
        print(parse(tif.pages[0].imagej_tags.overlays[0]))
        print(prepare([tif.pages[0].imagej_tags.overlays[0],tif.pages[0].imagej_tags.overlays[1],tif.pages[0].imagej_tags.overlays[2],tif.pages[0].imagej_tags.overlays[3],tif.pages[0].imagej_tags.overlays[4]]))
        print('########################################')
        print(tif.pages[0].imagej_tags.overlays[1])
        print(parse(tif.pages[0].imagej_tags.overlays[1]))
        print('########################################')
        print(tif.pages[0].imagej_tags.overlays[2])
        print(parse(tif.pages[0].imagej_tags.overlays[2]))
        print('########################################')
        print(tif.pages[0].imagej_tags.overlays[3])
        print(parse(tif.pages[0].imagej_tags.overlays[3]))
        print('########################################')
        print(tif.pages[0].imagej_tags.overlays[4])
        print(parse(tif.pages[0].imagej_tags.overlays[4]))
        print('########################################')
        print(tif.pages[0].imagej_tags.overlays[5])
        print(parse(tif.pages[0].imagej_tags.overlays[5]))
        print('########################################')
        '''
        '''
        dim_order='SXYCZT'
        dim_order_list=list(dim_order)
        lookup={'S':'samples_per_pixel','C':'channels', 'T':'frames', 'Y':'image_length', 'X':'image_width', 'Z': 'slices'}
        shape=[0]*len(dim_order_list)
        for i in range(len(dim_order_list)):
            if dim_order_list[i] in lookup.keys():
                key=lookup[dim_order_list[i]]
                if key in metadata.keys():
                    shape[i]=int(metadata[key])
                else:
                    shape[i]=1
        shape.reverse()
       
        images=np.reshape(images, tuple(shape))
        '''

    return images, metadata

   
def convert_metadata(metadata_dict):
    '''
    Create a metadata dictionary from imageJ metadata dictionary that contains the most relevant image properties.
    Keys are a subset of the keywords used in OME-Tiff xml files.
    '''
    
    
    #ImageJ dimension order is'SXYCZT'
    metadata_dict_out={'DimensionOrder':'XYCZT'}
    
    #Add bitdepth data to dictionary
    bit_depth_lookup={8:'uint8', 16:'uint16', 32:'float'}
    
    bits_per_sample=metadata_dict['bits_per_sample']
    if isinstance( bits_per_sample, int):
         metadata_dict_out['Type']=bit_depth_lookup[bits_per_sample]
    
    
    
    #Add key:value pairs from image_descriptor tiff tag with direct corespondence
    shape_keys=['SizeT', 'SizeC', 'SizeZ', 'SizeX', 'SizeY']
    key_lookup={'frames':'SizeT', 'channels':'SizeC', 'slices':'SizeZ', 'spacing':'PhysicalSizeZ'}
    for key in key_lookup.keys():
        if key in metadata_dict['image_description'].keys():
     
            if key_lookup[key] in shape_keys:
      
                metadata_dict_out[key_lookup[key]]=int(metadata_dict['image_description'][key])
            else:    
                metadata_dict_out[key_lookup[key]]=metadata_dict['image_description'][key]
        
        else:
            metadata_dict_out[key_lookup[key]]=1
    
    #Add key:value pairs from non imagej specific tiff tags with direct corespondence
    key_lookup={ 'image_width':'SizeX', 'image_length':'SizeY'}
    for key in key_lookup.keys():
        if key in metadata_dict.keys():
     
            if key_lookup[key] in shape_keys:
      
                metadata_dict_out[key_lookup[key]]=int(metadata_dict[key])
            else:    
                metadata_dict_out[key_lookup[key]]=metadata_dict[key]
        
        else:
            metadata_dict_out[key_lookup[key]]=1


    #Create Samples per pixel and channel name list    
    metadata_dict_out['SamplesPerPixel']=[metadata_dict['samples_per_pixel']]*metadata_dict_out['SizeC']
    metadata_dict_out['Name']=['Ch'+str(i) for i in range(metadata_dict_out['SizeC'])]

    #Create image type list and check if all bitdepths are equal
    if not isinstance(metadata_dict['bits_per_sample'], int) :
        if utils.length(set(metadata_dict['bits_per_sample'])) != utils.length(metadata_dict['bits_per_sample']):
            metadata_dict_out['Type']=bit_depth_lookup[metadata_dict['bits_per_sample'][0]]
        else:
            raise PythImageError('Each sample has to have the same bitdepth!','')
    
          
    #Add physicalSize and units in X, Y and T to dictionary
    if 'unit' in metadata_dict.keys():
       metadata_dict_out['PhysicalSizeZUnit']=metadata_dict['unit']
    if 'x_resolution' in metadata_dict.keys():
       metadata_dict_out['PhysicalSizeX']=metadata_dict['x_resolution'][1]/metadata_dict['x_resolution'][0]
       metadata_dict_out['PhysicalSizeXUnit']=metadata_dict['image_description']['unit']
    if 'y_resolution' in metadata_dict.keys():
       metadata_dict_out['PhysicalSizeY']=metadata_dict['y_resolution'][1]/metadata_dict['y_resolution'][0]
       metadata_dict_out['PhysicalSizeYUnit']=metadata_dict['image_description']['unit']
    
    #Get time interval and unit
    if 'finterval' in metadata_dict['image_description'].keys():
        metadata_dict_out['TimeIncrement']=metadata_dict['image_description']['finterval']
        if 'tunit' in metadata_dict['image_description'].keys():
            metadata_dict_out['TimeIncrementUnit']=metadata_dict['image_description']['tunit']
    elif 'fps' in metadata_dict.keys():
        metadata_dict_out['TimeIncrement']=1/metadata_dict['image_description']['fps']
        metadata_dict_out['TimeIncrementUnit']='s'
         
    #convert overlay to roi
    #Roi.roi_from_overlay(metadata_dict['imagej_metadata']['overlays'][0])
    
    return metadata_dict_out



    
def metadata_to_imageJ(metadata, rgb=None, colormaped=False, version='1.11a',
                       hyperstack=None, mode=None, loop=None, **kwargs):
    """Return ImageJ image description from data shape.

    """
    if colormaped:
        raise NotImplementedError('ImageJ colormapping not supported')

   
    
    #Create list of imagej specific descriptors for imagej_descriptors tiff tag
    descriptors = []
    if hyperstack is None:
        hyperstack = True
        descriptors.append('hyperstack=true')
    else:
        descriptors.append('hyperstack=%s' % bool(hyperstack))
    
    if mode is None and not rgb:
        mode = 'grayscale'
    if hyperstack and mode:
        descriptors.append('mode=%s' % mode)
    
    if loop is not None:
        descriptors.append('loop=%s' % bool(loop))
    
    for key, value in kwargs.items():
        descriptors.append('%s=%s' % (key.lower(), value))
        
    
    #Create list of general descriptors for imagej_descriptors tiff tag
    imagej_metadata = ['ImageJ=%s' % version]
    imagej_metadata.append('images=%i' % metadata['SizeC']*metadata['SizeZ']*metadata['SizeT'])
    
    if 'SizeC' in metadata:
        imagej_metadata.append('channels=%i' % metadata['SizeC'])    
    
    if 'SizeZ' in metadata:
        imagej_metadata.append('slices=%i' % metadata['SizeZ'])         
        
    
    if 'SizeT' in metadata:
        imagej_metadata.append('frames=%i' % metadata['SizeT'])
        if loop is None:
            descriptors.append('loop=false')
    
 
   
    # return imagej_metadata tiff tag string 
    return '\n'.join(imagej_metadata + descriptors + [''])
                  
                   
def save_image(image, metadata, path):
    '''
    Save Image. Metadata not saved currently!!
    '''
    
    if metadata["DimensionOrder"]!='XYCZT':
        image.reorder('XYCZT')
    
    with TiffWriter(path) as tif:
        #for index, (i, j, k) in enumerate(product(range(metadata['SizeC']), range(metadata['SizeT']), range(metadata['SizeZ']))):
        for i, j, k in product(range(image.shape[0]),range(image.shape[1]),range(image.shape[2])):
             if i==0 and j==0 and j==0:
                 tif.save(image[i][j][k], description=metadata_to_imageJ(metadata))
             else:
                 tif.save(image[i][j][k])

