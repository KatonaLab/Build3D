import unittest
import segmentation
import numpy as np
import time
import cProfile
import pstats 
import io
import sys
from tests import tests_segmentation


class SegmentationTest(unittest.TestCase):
        
    def setUp(self):
        self.__start = time.time()
        print('Running: {}'.format(self.id()))
        #print('{} ({}s)'.format(self.id(), round(elapsed, 2))) 
        
    def tearDown(self):
        elapsed = time.time() - self.__start
        print('Finished: {} ({}s)\n'.format(self.id(), round(elapsed, 4)))    
    
    def test_tag_image(self):
        
        for idx, cs in enumerate(tests_segmentation.tag_image_case_list):
            
            output_array=segmentation.tag_image(cs['input'])
            result_array=cs['result']
            

            case_text="Case "+str(idx)
            
            #Test if data types are equal
            with self.subTest():
                self.assertEqual(output_array.dtype,  result_array.dtype, msg=case_text) 
            
            #Test if array values are equal
            #If data type is float use assert_almost_equal or assert_allclose
            with self.subTest():
                np.testing.assert_array_equal(output_array,result_array, verbose=True,err_msg=case_text)        
        

    def test_overlap_image(self):
    
        
        for idx, cs in enumerate(tests_segmentation.overlap_image_case_list):
            
            output_array=segmentation.overlap_image(cs['input'])
            result_array=cs['result']
            

            case_text="Case "+str(idx)
            
            #Test if data types are equal
            with self.subTest():
                self.assertEqual(output_array.dtype,  result_array.dtype, msg=case_text) 
            
            #Test if array values are equal
            #If data type is float use assert_almost_equal or assert_allclose
            with self.subTest():
                np.testing.assert_array_equal(output_array,result_array, verbose=True,err_msg=case_text) 

        
    def test_threshold_manual(self):
    
    

        for idx, cs in enumerate(tests_segmentation.threshold_manual_case_list):
            
            output_array=segmentation.threshold_manual(cs['input'], upper=cs['upper'], lower=cs['lower'])
            result_array=cs['result']
            

            case_text="Case "+str(idx)+" with upper="+str(cs['upper'])+" lower="+str(cs['lower'])
            
            #Test if data types are equal
            with self.subTest():
                self.assertEqual(output_array.dtype,  result_array.dtype, msg=case_text) 
            
            #Test if array values are equal
            #If data type is float use assert_almost_equal or assert_allclose
            with self.subTest():
                np.testing.assert_array_equal(output_array,result_array, verbose=True,err_msg=case_text)
        

    def test_threshold_auto(self):
         
        
        for idx, cs in enumerate(tests_segmentation.threshold_auto_case_list):
            
            case_text="Case "+str(idx+1)+" with method="+str(cs['method'])+" mode="+str(cs['mode'])
            
            output=segmentation.threshold_auto(cs['input'], method=cs['method'], mode=cs['mode'])
            
            #Test if data types are equal
            with self.subTest():
                self.assertEqual(output[0].dtype,  cs['result_array'].dtype, msg=case_text) 
            
            #Test if array values are equal
            #If data type is float use assert_almost_equal or assert_allclose
            

            with self.subTest():
                np.testing.assert_array_equal(output[0], cs['result_array'], verbose=True, err_msg=case_text)
            #Test if thresholds are equal
            with self.subTest():
                self.assertEqual(output[1], cs['result_threshold'], msg='Threshold values do not match '+case_text)                


    def test_threshold_adaptive(self):
        

  
        for idx, cs in enumerate(tests_segmentation.threshold_adaptive_case_list):
            
            output_array=segmentation.threshold_adaptive(cs['input'], method=cs['method'], blocksize=cs['blocksize'], offset=cs['offset'])
            result_array=cs['result']
            

            case_text="Case "+str(idx)+" with method="+str(cs['method'])+" blocksize="+str(cs['blocksize'])+" offset="+str(cs['offset'])
            
            #Test if data types are equal
            with self.subTest():
                self.assertEqual(output_array.dtype,  result_array.dtype, msg=case_text) 
            
            #Test if array values are equal
            #If data type is float use assert_almost_equal or assert_allclose
            with self.subTest():
                np.testing.assert_array_equal(output_array,result_array, verbose=True,err_msg=case_text)
        
        
    
    

if __name__ == '__main__':
    
    #Ansi Escap characers:
    ANSI_DICT={'close':'\u001b[0m', 'open':u'', 'underlined':'\u001b[4m', 'red':'\u001b[31m', 'green':'\u001b[32m'}
    
    input_array_1 = np.array([[[0,0,0,0,0,0,0,0,0,0],
                               [0,200,200,200,0,0,0,0,0,0], 
                               [0,200,255,200,0,0,0,0,0,0],
                               [0,200,200,200,0,0,0,0,0,0],
                               [0,0,0,0,0,0,50,50,50,0],
                               [150,150,150,150,150,0,50,100,50,0],
                               [150,50,100,50,150,0,50,50,50,0],
                               [150,100,150,100,150,0,0,0,0,0],
                               [150,50,100,50,150,0,0,255,0,255],
                               [150,150,150,150,150,0,0,0,0,0]],
                            [[0,0,0,0,0,0,0,0,0,0],
                               [0,200,200,200,0,0,0,0,0,0], 
                               [0,200,255,200,0,0,0,0,0,0],
                               [0,200,200,200,0,0,0,0,0,0],
                               [0,0,0,0,0,0,50,50,50,0],
                               [150,150,150,150,150,0,50,100,50,0],
                               [150,50,100,50,150,0,50,50,50,0],
                               [150,100,150,100,150,0,0,0,0,0],
                               [150,50,100,50,150,0,0,255,0,255],
                               [150,150,150,150,150,0,0,0,0,0]] ],dtype=np.uint16)


    input_array_2 = np.array([[[0,0,0,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,0,0,0], 
                               [0,0,0,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,0,0,0]],
                            [[0,0,0,0,0,0,0,0,0,0],
                               [0,200,200,200,0,0,0,0,0,0], 
                               [0,200,255,200,0,0,0,0,0,0],
                               [0,200,200,200,0,0,0,0,0,0],
                               [0,0,0,0,0,0,50,50,50,0],
                               [150,150,150,150,150,0,50,100,50,0],
                               [150,50,100,50,150,0,50,50,50,0],
                               [150,100,150,100,150,0,0,0,0,0],
                               [150,50,100,50,150,0,0,255,0,255],
                               [150,150,150,150,150,0,0,0,0,0]] ],dtype=np.uint16)


    input_array_3 = np.array([[[0,0,0,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,0,0,0], 
                               [0,0,0,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,0,0,0]],
                            [[0,0,0,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,0,0,0], 
                               [0,0,255,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,100,0,0],
                               [0,0,0,0,0,0,0,0,0,0],
                               [0,0,150,0,0,0,0,0,0,0],
                               [0,0,0,0,0,0,0,255,0,255],
                               [0,0,0,0,0,0,0,0,0,0]] ],dtype=np.uint16)
    
    
    
    print(list([segmentation.overlap_image([input_array_1,input_array_2])]))
    
     

    print(ANSI_DICT["open"]+ANSI_DICT["red"]+ANSI_DICT["underlined"]+'\nRunning Tests:\n'+ANSI_DICT["close"])

    
    #Redirest stderr to dummy stream
    #Some SimpleITK functions output warnings to stderr
    sys.stderr = io.StringIO()


    #Run cProfiles without saving to files
    #Using cProfile.run('fun()') convention prints full report
    pr=cProfile.Profile()#
    pr.enable()
    
    
    test_suite = unittest.TestLoader().loadTestsFromTestCase(SegmentationTest)


    unittest.TestResult()
    test_stream=io.StringIO()
    test_result=unittest.TextTestRunner(stream=test_stream, verbosity=4).run(test_suite)
    
    pr.disable()

    #Create stream and generate profile stats report form profile
    s=io.StringIO()
    ps = pstats.Stats(pr, stream=s)
    
    #Format and sort stats report
    #Sort by cumulative time and only show results for functions containing 'tests'
    ps.strip_dirs().sort_stats('cumulative').print_stats('test')
    
    #Print results
    print(ANSI_DICT["open"]+ANSI_DICT["red"]+ANSI_DICT["underlined"]+'\nTest Results:\n'+ANSI_DICT["close"])
    
    print(test_stream.getvalue())
        
    #Print formatted stats report
    print(ANSI_DICT["open"]+ANSI_DICT["red"]+ANSI_DICT["underlined"]+'\nProfile Report:\n'+ANSI_DICT["close"])
    print(s.getvalue())


    if test_result.wasSuccessful():
        print(ANSI_DICT["open"]+ANSI_DICT["green"]+ANSI_DICT["underlined"]+'RUN WAS SUCCESSFUL!'+ANSI_DICT["close"])
    else:
        print(ANSI_DICT["open"]+ANSI_DICT["red"]+ANSI_DICT["underlined"]+'RUN WAS UNSUCCESSFUL!'+ANSI_DICT["close"])
    
    
    