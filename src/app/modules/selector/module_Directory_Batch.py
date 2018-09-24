import a3dc_module_interface as a3
from a3dc_module_interface import def_process_module
from glob import glob
import os


def module_main(ctx):
    path = os.path.abspath(a3.inputs['path'].path)
    # getting the file extension like '.tif'
    ext = os.path.splitext(a3.inputs['path'].path)[1]
    print('input path', path)
    print('extension', ext)

    if os.path.isfile(path):
        base_dir = os.path.dirname(path)
    else:
        base_dir = path

    print('base dir', base_dir)

    # globbing all the files with matching extensions
    file_list = [os.path.abspath(x) for x in glob(base_dir + '/*' + ext)]

    if os.path.isfile(path):
        file_list.remove(path)
        file_list.insert(0, path)

    if len(file_list) == 0:
        raise Warning('path {} is empty'.format(base_dir))

    index = ctx.run_id()
    if index < len(file_list) - 1:
        ctx.set_require_next_run(True)
    else:
        ctx.set_require_next_run(False)

    print(file_list, index)

    url = a3.Url()
    url.path = file_list[index]
    print('current', url.path)
    a3.outputs['file'] = url


config = [
    a3.Parameter('path', a3.types.url),
    a3.Output('file', a3.types.url)]

def_process_module(config, module_main)
