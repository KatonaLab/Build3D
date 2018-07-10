import numpy as np
import copy

from . import utils
from .RoiClass import Roi 
from .error import PythImageError as PythImageError
from .io import imagej_tiff, ome_tiff, tiff
#from .io.imagej_tiff import load, convert_metadata #imagej_tiff, 
#from .io.imagej import load_imagej, convert_imagej_metadata
#from .io.ome import load_ome, convert_ome_metadata, metadata_to_ome, save_image  
#from .io.tiff import load_tiff, convert_tiff_metadata

##Add channel, remove channel, extract channel etc.
class Image(object):
    
    '''TOBE FIXED return objects cpy or deepcopy etc.
    
    '''
    
    __protected=['SizeT', 'SizeC','SizeZ', 'SizeX','SizeY', 'SamplesPerPixel', 'Type', 'DimensionOrder']
    __dim_translate={'T':'SizeT', 'C':'SizeC', 'Z':'SizeZ', 'X':'SizeX', 'Y': 'SizeY'}#, 'S':'SamplesPerPixel'}
   
   
    
    def __init__(self, image, metadata):
        
        #Check if image and metadata is valid
        self.__validate(image, metadata)
        self.__metadata=metadata
        self.__image=image
        #Reshape image so it contains all dimensions
        #self.__image=self.__expand_singleton_dimensions(image, metadata)
        
    def __call__(self, T=None, C=None, Z=None):
        
        if not (isinstance(T, int) or isinstance(C, int) or isinstance(Z, int)):
            raise PythImageError("T, C and Z can only be called with inteber values", TypeError)
  
        local_dict=locals()    
        arg_list = [arg for arg in  ('T', 'C', 'Z') if arg in local_dict.keys() if local_dict[arg]!=None ]
        
        indexes=[slice(local_dict[arg],local_dict[arg]+1,1)  for arg in arg_list]
        #indexes.append(slice(1,2,1))

        dimensions=list(self.__metadata['DimensionOrder'])
        dimension_order=[dim for dim in dimensions if dim not in arg_list]+arg_list
   
        self.reorder(dimension_order)
        
        shape=tuple(reversed(list(self.__image.shape)))
        
        metadata_new=copy.deepcopy(self.__metadata)
        
        for dim in dimension_order:
            if dim in ('T', 'C', 'Z'):
                if isinstance(local_dict[dim], int):
                    metadata_new[self.__dim_translate[dim]]=1
                if isinstance(local_dict[dim], list):    
                    metadata_new[self.__dim_translate[dim]]=len(local_dict[dim])
        
        if 'C' in arg_list:
            
            metadata_new['Name']=metadata_new['Name'][C]
            metadata_new['SamplesPerPixel']=metadata_new['SamplesPerPixel'][C]

   
        image_new=copy.deepcopy(self.__image.copy())
      
        image_new=image_new[indexes]
        
        
        print("Metadata: "+str(metadata_new['DimensionOrder']))
        print("Call: "+str(image_new.shape))
  
        a=self.__expand_singleton_dimensions(image_new, metadata_new)
        print("a shape: "+str(a.shape))

        return self.__init__(image=image_new,metadata=metadata_new)
    
    
    def __getitem__(self, key):
        

        shape=self.__image.shape
        metadata=copy.deepcopy(self.__metadata)
        image=copy.deepcopy(self.__image[key])
        order=tuple(reversed(list(metadata['DimensionOrder'])))
         
        if isinstance(key, (int,slice)):
            key=[key]
        if len(key)>len(shape):
            raise ValueError('Invalid slice object!')
     
        for i in range(len(key)):
            
            length=Image.__slice_length(key[i], shape[i])
            dim_current=order[i]
            
            if dim_current=='C':
                metadata['Name']=np.squeeze(metadata['Name'][key[i]]).tolist()
                metadata['SamplesPerPixel']=np.squeeze(metadata['SamplesPerPixel'][key[i]]).tolist()
            metadata[self.__dim_translate[dim_current]]=length
        
        return Image(image=image ,metadata=metadata)
    
    def __repr__(self):
        return utils.dict_to_string(self.__metadata)     
    
    def __getattr__(self, atr):
        raise AttributeError("Attribute: "+str(atr)+" is not available!")           
   
    @property
    def image(self):
        return self.__image
    
    @image.setter
    def image(self, value):
        #Check if new image is compattible with metadata
        self.__validate(value, self.__metadata)
        #Set new image
        self.__image=value
          
    @property
    def metadata(self):
        #raise PythImageError('Metadata is a protected attribute. Please use: get_metadata, set_metadata to get/set keys!','')
        return self.__metadata
    
    @metadata.setter
    def metadata(self, value):
        #Check if new metadata is compattible with image
        self.__validate(self.__image, value)
        #Set metadata
        self.__metadata=value
  
    @classmethod              
    def load(cls, path, file_type='imageJ', **kwargs):
        '''
        Load image stack from path. RGB images are not supported currently and only first frame is returned.
        '''
        
        kwargs=dict(**kwargs)
        
        if file_type=='ome':
        
            #Load image and create simplified metadata dictionary
            image, ome_metadata=ome_tiff.load_image(path)
            metadata=ome_tiff.convert_metadata(ome_metadata)
             
        elif file_type=='imagej':
            
            #Load image and create simplified metadata dictionary
            image, imagej_metadata=imagej_tiff.load_image(path)
            
            metadata=imagej_tiff.convert_metadata(imagej_metadata)
            
            '''        
            dim_order_final='XYZCT'
            #Change dimension order to the one specified in dim_order_final
            dim_order=metadata['DimensionOrder']
            image=ImageClass.reorder(dim_order, 'SXYZCT')
             
            #Reshape image so output image has separate channels for S dimension
            if metadata['SamplesPerPixel']>1:
                image=ImageClass.merge_axes(image, 5, 1)
            metadata['DimensionOrder']=dim_order_final
            
            #For hyperstacks remove S singleton dimension
            if image.shape[-1]==1:
                image=np.squeeze(image, axis=5)
            '''    
        
        elif file_type=='tiff':
             #Load image and create simplified metadata dictionary
            image, metadata=tiff.load(path)
            metadata=tiff.convert_metadata(metadata, order=kwargs['order'], shape=kwargs['shape'])
        
        #Return first timeframe
        return cls(image=image, metadata=metadata)
    
              
    def save(self, path, file_type='ome'):
        '''
        Load image stack from path. RGB images are not supported currently and only first frame is returned.
        '''
        
        
        if file_type=='ome':

            #Load image and create simplified metadata dictionary
            self.reorder('XYCZT')
            ome_tiff.save_image(self.image,self.metadata, path)
           
             
        elif file_type=='imagej':
            ome_tiff.save_image(self.image,self.metadata, path)
            
            
    def __validate(self, image, metadata):

        #Check if image is numpy array and metadata is dict
        if not isinstance(metadata, dict):
            raise TypeError('metadata must be a dictionary!')
        if not isinstance(image, np.ndarray):
            raise TypeError('image must be a a NumPy array!')
        
        #Check if metadata 'Type' field matches the type of the image
        
        if metadata['Type']!=image.dtype:
             raise TypeError('image data type does not mach the one specified in the metadata!')
             
        #Check if number of channels and length of the name list is the same
        if 'SizeC' in metadata.keys():
           
            if isinstance(metadata['SamplesPerPixel'], list):
                sample_cnt=sum(metadata['SamplesPerPixel'])
            elif isinstance(metadata['SamplesPerPixel'], int):
                sample_cnt=metadata['SamplesPerPixel']
      

            
            if sample_cnt!=metadata['SizeC']:
                raise PythImageError('Invalid SamplesPerPixel list!','')  
            
            if utils.length(metadata['Name'])!=metadata['SizeC']:
                raise PythImageError('Length of Name list is invalid!','')
            
  
        #Check if image shape confers with the one in the metadata
        #Remove singleton dimensions
        shape_image=[val for val in image.shape if val!=1]
        available_keys=[self.__dim_translate[dim] for dim in reversed(metadata['DimensionOrder']) if self.__dim_translate[dim] in metadata.keys()]
 
        shape_metadata=[metadata[key] for key in available_keys if metadata[key]!=1]
      
        if shape_image!=shape_metadata:
            raise PythImageError('shape information in metadata is not compattible to the image shape!','')
    
    
    def __expand_singleton_dimensions(self, image, metadata):

        dim_order_list=metadata['DimensionOrder']
     
        shape=[1]*len(dim_order_list)
        for i, dim in enumerate(dim_order_list):
            if dim in self.__dim_translate.keys() and dim!='S':
                key=self.__dim_translate[dim]
             
                if key in metadata.keys():
                    shape[i]=int(metadata[key])
                else:
                    shape[i]=1
        shape.reverse()
    
        return np.reshape(image, tuple(shape))
    
    @staticmethod
    def __slice_length(slice_object, object_length):
        '''Calculate length of slice from slice object.
        '''
        if isinstance(slice_object, int):
            slice_length=1 
        if isinstance(slice_object, slice):
            slice_length=len(range(*slice_object.indices(object_length)))
        
        return  slice_length
    
    
    def get_metadata(self, key):
        return self.__metadata[key]
    
    
    def set_metadata(self, key, value):

        if key not in self.__protected:

            if key in self.__metadata:
                
                if type(self.__metadata[key])!=type(value):
                    raise TypeError("The type of the given value does not match the metadata type!")
                
                if hasattr(value, '__len__'):

                    if len(self.__metadata[key])!=len(value):
                  
                        raise TypeError("The length of the given value does not match that of metadata type!")
                
                    else:
                        self.__metadata[key]=value
            
            else:
                self.__metadata[key]=value 

        else:
            raise KeyError('Key is not meant for direct access!')

    

    def append_to_dimension(self, image, dim):
        
      
        #Append names
        self.__metadata['Name']=utils.concatenate(self.__metadata['Name'],image.get_metadata('Name'))
        self.__metadata['SamplesPerPixel']=utils.concatenate(self.__metadata['SamplesPerPixel'], image.get_metadata('SamplesPerPixel'))
 
        #Get original dimension order
        original_order=self.__metadata['DimensionOrder']
    
        #reshape image so the two confer
        image.reorder(original_order)
        
        #Get the index of the given dimension in the shape of the image
        ind=len(original_order)-original_order.index('C')-1
        
        self.__metadata[self.__dim_translate[dim]]=self.__metadata[self.__dim_translate[dim]]+image.get_metadata(self.__dim_translate[dim])

        self.image=np.concatenate((self.image,image.image), axis=ind)
       
       
        
    def roi_to_channel(self, index, value=1):
      

        if 'ROI' in self.__metadata.keys():
          
            
            #Generate roi list. If image has multiple ROI-s, metadata['ROI'] is a list.
            if isinstance(self.__metadata['ROI'], dict):
                roi_list=[self.__metadata['ROI']]
            elif isinstance(self.__metadata['ROI'], list):
                roi_list=self.__metadata['ROI']
            
            #Check if index is within possible range
            if index>=len(roi_list):
                raise IndexError('Only '+str(len(roi_list))+' ROI(s) available!')
            
            #Get reversed dimension order
            order=self.__metadata['DimensionOrder'][::-1]

            #create metadata dictionary for roi and set channel to 1
            roi_metadata=self.__metadata.copy()
            roi_metadata['SizeC']=1
            roi_metadata['Name']=roi_list[index]['ID']
            roi_metadata['SamplesPerPixel']=1
            del roi_metadata['ROI']
            
            #Create shape tuple
            shape=[1]*len(order)
            for idx, dim in enumerate(order):
                if dim!='C':
                    shape[idx]=self.__metadata[self.__dim_translate[dim]]
                if dim=='C':
                    shape[idx]=1
            
            shape=tuple(shape)

            img=np.zeros(shape, dtype=self.__metadata['Type'])
            
            rr, cc = RoiClass(roi_list[index]).coordinates
            
            #Create slice object to set value of ROI pixels
            slice_object_list=[1]*len(order)
            for idx, dim in enumerate(order):
                if dim=='X':
                    slice_object_list[idx]=rr
                elif dim=='Y':
                    slice_object_list[idx]=cc
                else:    
                    slice_object_list[idx]=slice(None,None,None)
       
            #Set roi pixel values
            img[slice_object_list] = value
            #img[:,:,:,cc,rr] = value

            #Create new ImageClass object
            roi=Image(img, roi_metadata )

            
            self.append_to_dimension(roi, dim='C')

            
        else:
            raise PythImageError('No ROI available!', '')
                
    def reorder(self, order):
        '''
        Reordet dimension order to the one specified in final_dim_order.Both have to 
        be the same length and confer with number of axes in ndarray!
        Reshape array so it has all the axes
        '''
     
        order=list(order)
        dim_order_list=list(self.__metadata['DimensionOrder'])
        order_final=order.copy()
        
        #Remove singleton dimensions
        dim_translate={'T':self.__metadata['SizeT'], 'C':self.__metadata['SizeC'], 'Z':self.__metadata['SizeZ'], 'X':self.__metadata['SizeX'], 'Y': self.__metadata['SizeY']}
        singleton_dim=[key for key in dim_translate if int(dim_translate[key])<=1]
        
        for dim in singleton_dim:
            order.remove(dim)
            dim_order_list.remove(dim)
      
        #Cycle through image dimension order, check for differences in final order and replace
        axis_number=len(order)-1
        for i in range(len(dim_order_list)):
           
            if dim_order_list[i]!=order[i] :
                axis_index_current=i
                axis_index_final=order.index(dim_order_list[i])
                self.__image=self.__image.swapaxes(axis_number-axis_index_current, axis_number-axis_index_final)
                dim_order_list[axis_index_current], dim_order_list[axis_index_final] = dim_order_list[axis_index_final], dim_order_list[axis_index_current]
           
      
        #Set final dimension order
        self.__metadata['DimensionOrder']=''.join(order_final)
    
    def merge_axes(ndarray, axis1, axis2):
        '''
        Merge two axes. First the axes are swaped so the two axes are besides each other
        then the ndarray is linearized and reshaped.
        '''
        #Determine which axes is larger
        largest_axis=max(axis1, axis2)
        smallest_axis=min((axis1, axis2))
        
        #Generate final shape
        shape=list(ndarray.shape)
        shape[smallest_axis]=shape[smallest_axis]*shape[largest_axis]
        del shape[largest_axis]
        
        #Swap axes so the two axes are besides each other
        ax=largest_axis
        while ax>smallest_axis:
            ndarray=ndarray.swapaxes(ax,ax-1)
            ax-=1
        #Ravel, reshape and return result
        return np.reshape(ndarray.ravel(), shape)
    
    def merge_axes2(ndarray, dim_order='SXYCZT', axis1='S', axis2='C'):
        '''
        Merge two axes. First the axes are swaped so the two axes are besides each other
        then the ndarray is linearized and reshaped.
        '''
        #Determine which axes is larger
        largest_axis=max(axis1, axis2)
        smallest_axis=min((axis1, axis2))
        
        #Generate final shape
        shape=list(ndarray.shape)
        shape[smallest_axis]=shape[smallest_axis]*shape[largest_axis]
        del shape[largest_axis]
        
        #Swap axes so the two axes are besides each other
        ax=largest_axis
        while ax>smallest_axis:
            ndarray=ndarray.swapaxes(ax,ax-1)
            ax-=1
        #Ravel, reshape and return result
        return np.reshape(ndarray.ravel(), shape)        


    

    
    def z_projection (self):
        '''Needs to be implemented
        '''
        pass
      

      
    
    
    
