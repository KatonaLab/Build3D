# -*- coding: utf-8 -*-
"""
Created on Tue May 15 11:28:54 2018
@author: pongor.csaba
"""
import PythImage
import numpy as np

def load(path='C:\\Users\\pongor.csaba\\Desktop\\aaaaaaaaaaaaaaaa.ome.tif'):

    output = PythImage.ImageClass.load(path, file_type='ome')
    
    
    return output
 


if __name__ == "__main__":
    

    img=load()
    dim_order='ZXYTC'
    print('Start',img.image.shape)
    print('Start',img.metadata['DimensionOrder'])
    
    print('Aim: ', dim_order)
    img.reorder(dim_order)
    
    print('End',img.metadata['DimensionOrder'])
    print('Start',img.image.shape)
    
    print(axis)
    
    index=1
    axis=len(img.image.shape)-img.metadata['DimensionOrder'].index('Z')
    slices=img.metadata['SizeZ']
    i=1
    for i in range(slices):
        a=np.take(img.image, i, axis=index):
        i+=1
    print(i)