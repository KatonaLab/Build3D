import numpy as np
from skimage.filters import threshold_otsu

def filter(params):
    w = int(params['width'])
    h = int(params['height'])
    # print(w, h)
    # x = np.array(params['data']).reshape((h, w))
    # t = threshold_otsu(x)
    # x = (x >= 50) * 255.
    # r = list(x.ravel())
    # print('done')
    # return r
    return params['data']
