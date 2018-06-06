# -*- coding: utf-8 -*-
"""
Created on Fri May 18 12:15:35 2018

@author: pongor.csaba
"""
import traceback

#Class for error handling
class A3dcError(Exception):
    
    def __init__(self, message, errors):

        super(A3dcError, self).__init__(message)
    
        self.traceback=str(traceback.format_exc()).replace('\n', '\n\t\t')
        self.message = message
        self.errors = errors
        
    def __str__(self):
        
        
        return repr(self.message)+"\n\nERROR:"+repr(self.errors)+"\n\nTRACEBACK:"+str(self.traceback)