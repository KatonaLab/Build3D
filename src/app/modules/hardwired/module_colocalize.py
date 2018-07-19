import a3dc_module_interface as a3
from modules.a3dc_modules.a3dc.interface import colocalize
import numpy as np

FILTERS = ['volume', 'voxelCount', 'centroid',
           'pixelsOnBorder', 'meanIntensity',
           'variance']


def module_main(_):
    params = {'ch1': read_params('ch1'),
              'ch2': read_params('ch2'),
              'ovl': read_params('ovl', use_images=False)}

    colocalize(params['ch1']['intensity'],
               params['ch1']['mask'],
               params['ch1']['intensity_meta'],
               params['ch1']['settings'],
               params['ch2']['intensity'],
               params['ch2']['mask'],
               params['ch2']['intensity_meta'],
               params['ch2']['settings'],
               params['ovl']['settings'])


def add_input_fields(name, use_images=True, filters=FILTERS):
    config = []
    if use_images:
        config = [
            a3.Input('{} intensity'.format(name), a3.types.ImageFloat),
            a3.Input('{} mask'.format(name), a3.types.ImageUInt32)]

    for f in filters:
        for m in ['min', 'max']:
            config.append(
                a3.Parameter('{} {} {}'.format(name, f, m), a3.types.float)
                .setIntHint('min', 0)
                .setIntHint('max', 1000)
                .setIntHint('default', 0 if m == 'min' else 1000))
    return config


def read_params(name, use_images=True, filters=FILTERS):
    out_dict = {}
    if use_images:
        intensity_field = '{} intensity'.format(name)
        mask_field = '{} mask'.format(name)

        intensity_im = a3.MultiDimImageFloat_to_ndarray(
            a3.inputs[intensity_field])
        mask_im = a3.MultiDimImageUInt32_to_ndarray(a3.inputs[mask_field])

        intensity_meta = {
            'Name': name + '_intensity',
            'Path': '.',
            'Type': np.float}

        mask_meta = {
            'Name': name + '_mask',
            'Path': '.',
            'Type': np.uint32}

        out_dict = {'intensity': intensity_im,
                    'intensity_meta': intensity_meta,
                    'mask': mask_im,
                    'mask_meta': mask_meta}

    settings = {}
    for f in filters:
        settings[f] = {}
        for m in ['min', 'max']:
            settings[f][m] = a3.inputs['{} {} {}'.format(name, f, m)]

    out_dict['settings'] = settings
    return out_dict


config = []
config.extend(add_input_fields('ch1'))
config.extend(add_input_fields('ch2'))
config.extend(add_input_fields('ovl', use_images=False))

a3.def_process_module(config, module_main)
