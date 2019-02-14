import os
import sys
import subprocess
import random
import traceback
import numpy as np
#from .constants import SEPARATOR, QUOTE_LIST
from constants import SEPARATOR, QUOTE_LIST

def reorder_list(lst, val_list):

    for element in reversed(val_list):
        if element in lst:
            lst.remove(element)
            lst.insert(0, element)

    return lst


def round_up_to_odd(f):
    #Round to add as blocksize has to be odd
    return int(np.ceil(f) // 2 * 2 + 1)


def os_open(path):
    '''Open file using its associated program in an os (MacOS, Linux, Windows) 
    dependent manner. Exceptions are raised as warnings using a try statement.
    
    path(str): The path of the file to be opened
    '''
    try:
        #Windows
        if sys.platform == "win32":
            os.startfile(path)
        #MacOS
        elif sys.platform == "darwin":
           subprocess.call(["open", path]) 
        #Linux/Unix
        else:
            subprocess.call(["xdg-open", path])
    
    except Exception as e:
        raise Warning(str(e))


def quote(verbose=False):
	
    '''Generates a random quote (most of which are from Gaussian03).
    '''
	 #Generate random index
    index=random.randint(0,len(QUOTE_LIST)-1)
    
    #Get quote
    quote=QUOTE_LIST[index]
    
    #Print if verbose is set to true
    if verbose:
        print_line_by_line(quote)
    
    return quote


def print_line_by_line(string, file=sys.stdout):
    
    string_list=string.split("\n")
    for i in string_list:
        print(i, file)


def warning(string):
    
    print(string, file=sys.stderr)
        

def error(message, exception=None, verbose=True):

    if verbose==False:
        len=1
    else:
        len=10
    
    print(SEPARATOR, file=sys.stderr)
    
    print("Traceback:",file=sys.stderr)
    print(traceback.format_exc(len), file=sys.stderr)
    
    print(SEPARATOR, file=sys.stderr)
    print(message, file=sys.stderr) 
    print(SEPARATOR, file=sys.stderr) 

    raise Exception(message, exception)


def value_to_key(dictionary, val):
    
    #Get the ocurrences of val among dictionary values
    count=sum(value == val for value in dictionary.values())
    #version 2: count=sum(map((val).__eq__, dictionary.values()))
    
    #If value is not in dictionary.values raise exception
    if count==0:
        raise LookupError('Value %s is not in dictionary'.forma(str(val)))
    if count>1:
        raise LookupError('More than one key have value %s!'.forma(str(val)))
    
    #get value
    #version 2: list(dictionary.keys())[list(dictionary.values()).index(val)]
    for key, value in dictionary.items():
        if value == val:
            return key 


def dictinary_equal(dict_1,dict_2):
    '''Compares two dictionaries and returns true is dictionaries have the same
    keys and values are equal (key and value objects have to be comparable)
    '''
    flag=True
    for key, value in dict_1.items():
        if key in dict_2.keys():
            if dict_2[key]!=value:
                flag=False
        else:
            flag=False
            
    return flag

def rename_duplicates(string_list):
    '''Processes a list of strings. If list has duplicate elements an index is 
    added to it.
    '''
    if isinstance(string_list, str) or not isinstance(string_list, list):
        raise Exception('Object must be list of strings!')
    
    output = []
    for idx, val in enumerate(string_list):
        totalcount = string_list.count(val)
        count = string_list[:idx].count(val)
        output.append(val +'_'+ str(count + 1) if totalcount > 1 else val)
    
    return output


def get_next_filename(output_path, file_name):
    
    basename, extension=os.path.splitext(file_name)
    
    #If filename exists generate a neme that is not used
    i=1
    final_basename=basename
    while os.path.exists(os.path.join(output_path, final_basename+extension)):
         final_basename=basename+'_'+str('{:03d}'.format(i))
         i += 1

    return final_basename+extension   

def convert_array_type(array, dtype):

    array=array.astype(dtype)
    
    return array
        
#Class for error handling
class VividException(Exception):
    def __init__(self, message, errors):
			
        super(VividException, self).__init__(message)
        self.errors = errors
    
        print(traceback.format_exc(10), file=sys.stderr)	
        
    def __str__(self):
        
        return repr(self.errors)
    