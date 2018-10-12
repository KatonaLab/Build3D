import a3dc_module_interface as a3

def module_main(ctx):
    
    a3.outputs['Directory']=a3.inputs['Directory']
    #if os.path.isdir(a3.inputs['Directory'].path):
        #a3.outputs['Directory']=a3.inputs['Directory']
    #else:
        #error("Error occured while executing '"+str(ctx.type())+"' module '"+str(ctx.name())+"' !", OSError('Path is not a directory!'))
    
config = [
    a3.Parameter('Directory', a3.types.url).setBoolHint('folder', True),
    a3.Output('Directory',  a3.types.url)]

a3.def_process_module(config, module_main)
