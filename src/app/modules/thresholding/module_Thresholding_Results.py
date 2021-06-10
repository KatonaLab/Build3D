import a3dc_module_interface as a3
from modules.packages.a3dc.ImageClass import ImageClass
from modules.a3dc_interface_utils import error
 

def module_main(ctx):
    
    try:
        a3.outputs['ChA Image']=ImageClass(a3.inputs['ChA Image'].image, a3.inputs['ChA Image'].metadata).to_multidimimage()
        a3.outputs['ChB Image']=ImageClass(a3.inputs['ChB Image'].image, a3.inputs['ChB Image'].metadata).to_multidimimage()
        a3.outputs['ChA Thresholded']=ImageClass(a3.inputs['ChA Thresholded'].image, a3.inputs['ChA Thresholded'].metadata).to_multidimimage()
        a3.outputs['ChB Thresholded']=ImageClass(a3.inputs['ChB Thresholded'].image, a3.inputs['ChB Thresholded'].metadata).to_multidimimage()


    except Exception as e:
        error("Error occured while executing '"+str(ctx.type())+"' module '"+str(ctx.name())+"' !", exception=e)

 
def generate_config():

    #Set Outputs and inputs
    config=[a3.Input('ChA Image', a3.types.GeneralPyType), 
        a3.Input('ChB Image', a3.types.GeneralPyType), 
        a3.Input('ChA Thresholded', a3.types.GeneralPyType),
        a3.Input('ChB Thresholded', a3.types.GeneralPyType),
        a3.Output('ChA Image', a3.types.ImageFloat), 
        a3.Output('ChB Image', a3.types.ImageFloat), 
        a3.Output('ChA Thresholded', a3.types.ImageFloat),
        a3.Output('ChB Thresholded', a3.types.ImageFloat)] 
    
    
    return config

a3.def_process_module(generate_config(), module_main)

