# -*- coding: utf-8 -*-
"""
Created on Tue May 15 11:28:54 2018
@author: pongor.csaba
"""
import PythImage

def load(path='aaaaaaaaaaaaaaaa.ome.tif'):

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