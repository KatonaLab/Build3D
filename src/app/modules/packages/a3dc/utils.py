import os
import sys
import subprocess

import traceback
import numpy as np

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


def print_line_by_line(string, file=sys.stdout):
    
    string_list=string.split("\n")
    for i in string_list:
        print(i, file)


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
    

    array=array.astype(np.dtype(dtype), casting='safe')
    
    return array
        
#Class for error handling
class a3dcException(Exception):
    def __init__(self, message, errors):
			
        super(a3dcException, self).__init__(message)
        self.errors = errors
    
        print(traceback.format_exc(10), file=sys.stderr)	
        
    def __str__(self):
        
        return repr(self.errors)
    