# -*- coding: utf-8 -*-
"""
Created on Wed Jul  4 14:59:19 2018

@author: pongor.csaba
"""

class ImagejRoi(object):
    
    
    def __init__(self, overlay):
                
        self.overlay=overlay
        
    
    def roi_from_overlay(self, overlay):
                
        print(overlay) 