# -*- coding: utf-8 -*-
"""
Created on Tue May 15 11:28:54 2018
@author: pongor.csaba
"""
import PythImage.ImageClass as PythImage

#from modules.a3dc_modules.a3dc.utils import SEPARATOR, error, print_line_by_line, warning
import numpy as np
import os
import copy
import pandas as pd

def load(path='C:\\Users\\pongor.csaba\\Desktop\\aaaaaaaaaaaaaaaa.ome.tif'):

    output = PythImage.load(path, file_type='ome')
    
    
    return output


        
if __name__ == "__main__":
   

    img=load('C:/Users/pongor.csaba/Desktop/TestImages/Output/icstestProbe_Ex_647nm_Probe_Ex_488nm.ome.tiff')
    #img=load('C:/Users/pongor.csaba/Desktop/TestImages/Output/ometest.omeCh1_Ch2.ome.tiff')
    #img=load('C:\\Users\\pongor.csaba\\Desktop\\aaaaaaaaaaaaaaaa.ome.tif')
    #img=load('C:\\Users\\pongor.csaba\\Desktop\\Output\\aaaaaaaaaaaaaaaa.ome_Ch0_Ch0_overlap.tiff.tiff')
    #img=load('C:\\Users\\pongor.csaba\\Desktop\\Output\\LALALALALALA.tif')
    #img=load()

    
    img2=PythImage(copy.deepcopy(img.image), copy.deepcopy(img.metadata))
    img2.save('C:\\Users\\pongor.csaba\\Desktop\\TestImages\\', 'LALALALALALAnd.tif')

    print(str(img))
