# -*- coding: utf-8 -*-
"""
Created on Tue May 15 11:28:54 2018
@author: pongor.csaba
"""
import PythImage

def load_ome(path='2048_2048_50_4.ome.tif'):

    output = PythImage.Image.load(path, file_type='ome')
    
    
    return output.image
 


if __name__ == "__main__":
    
    print(type(load_ome()))
    print(load_ome().shape)