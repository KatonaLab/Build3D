import a3dc_module_interface as a3
from a3dc_module_interface import def_process_module
from glob import glob


def module_main(ctx):
    basepath = a3.inputs['directory'].path
    files = glob(basepath + '/*')
    if len(files) == 0:
        raise Warning('path {} is empty'.format(basepath))

    index = ctx.run_id()
    if index < len(files) - 1:
        ctx.set_require_next_run(True)
    else:
        ctx.set_require_next_run(False)

    url = a3.Url()
    url.path = files[index]
    print('file:', files[index])
    a3.outputs['directory'] = url


config = [
    a3.Parameter('directory', a3.types.url).setBoolHint('folder', True),
    a3.Output('directory', a3.types.url)]

def_process_module(config, module_main)
