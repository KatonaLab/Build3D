import xlsxwriter
import numpy as np
import pandas as pd
import copy
import os
import matplotlib.pyplot as plt
from matplotlib import colors, is_interactive, interactive
import random
from io import BytesIO
from.constants import SHAPE_DESCRIPTORS, INTENSITY_DESCRIPTORS, OTHER_DESCRIPTORS, NUMERIC_DTYPES
from .utils import reorder_list


        
def save_image(img_list, path, file_name):
    '''
    '''
        
    #Combine images
    for idx, img in  enumerate(img_list):
        
        #Remove 'Path' key
        if 'Path' in img.metadata.keys():
            del img.metadata['Path']
            
        if idx==0:
            output=copy.deepcopy(img)
        else:
            output.append_to_dimension(img)
    
    #save image
    output.save(path, file_name)

def save_data(img_list, path, file_name, to_text=True):
    '''
    :param dict_list: Save dictionaries in inputdict_list
    :param path: path where file is saved
    :param to_text: if True data are saved to text
    :param fileName: fileneme WITHOUT extension
    :return:
    '''

    dataframe_list=[]
    key_order_list=[]

                
    dict_list=[x.database for x in img_list]
    name_list = [x.metadata['Name'] for x in img_list]

    for dic in  dict_list:

        # Convert to Pandas dataframe
        df=pd.DataFrame(dic)
        
        dataframe_list.append(df)

        # Sort dictionary with numerical types first (tag, volume, voxelCount,  first) and all others after (centroid, center of mass, bounding box first)
        numeric_keys = []
        other_keys = []
        for key in list(dic.keys()):

            if str(df[key].dtype) in NUMERIC_DTYPES:
                numeric_keys.append(key)

            else:
                other_keys.append(key)

        #Rearange keylist
        numeric_keys=reorder_list(numeric_keys,['tag', 'volume', 'voxelCount', 'filter'])
        
        other_keys=reorder_list(other_keys,['centroid'])
        
        key_order_list.append(numeric_keys+other_keys)
        

    if to_text==False:
        
        
        
        # Create a Pandas Excel writer using XlsxWriter as the engine.
        writer = pd.ExcelWriter(os.path.join(path, file_name+'.xlsx'), engine='xlsxwriter')
        
        for i in range(len(dataframe_list)):
            
            #If no names are given ore name_list is shorter generate worksheet name
            if name_list==None or i>len(name_list):
                name='Data_'+str(i)
            else:
               name =str(name_list[i]) 

            # Convert the dataframe to an XlsxWriter Excel object. Crop worksheet name if too long
            if len(name) > 30:
                name=(str(name)[:30] + '_')
            
            dataframe_list[i].to_excel(writer, sheet_name=name, columns=key_order_list[i], header=True)
            
            #Get workbook, worksheet
            workbook = writer.book
            
            worksheet=writer.sheets[name]
            worksheet.set_zoom(90)        
            worksheet.freeze_panes(1, 0)
        
            #Set column widths based on header
            cell_format=workbook.add_format()
            cell_format.set_shrink('auto')
            cell_format.set_align('center')
            cell_format.set_align('vcenter')
            cell_format.set_text_wrap()
            cell_format.set_shrink()
            
            #Set format of the first column
            worksheet.set_column(1, 1, 20, cell_format) 
            
            #Set column format from the second onwards
            for j in range(len(key_order_list[i])):
                worksheet.set_column(j+1, j+1, len(key_order_list[i][j])*1.5, cell_format)  
            
            #Set formatt of the first row
            row_format=workbook.add_format()
            row_format.set_align('center')
            row_format.set_align('vcenter')
            row_format.set_bottom(1)
            worksheet.set_row(0, 70, row_format)

            #Add comments to excel cells
            for j in range(len(key_order_list[i])):
                
                comment_string='N/A'
                key=key_order_list[i][j]
                
                if key in SHAPE_DESCRIPTORS.keys():
                    comment_string=SHAPE_DESCRIPTORS[key]
                else:
                    int_key=key.split(' ')[0]
                    if int_key in INTENSITY_DESCRIPTORS.keys():
                        comment_string=INTENSITY_DESCRIPTORS[int_key]
                    if int_key in OTHER_DESCRIPTORS.keys():
                        comment_string=OTHER_DESCRIPTORS[int_key]
                        
                worksheet.write_comment(0, j+1, comment_string, {'height': 80, 'width':300, 'color': '#00ffffcc'})
                                                       
        #Create statistical report
        #data_dic={name_list[i]:dict_list[i] for i in range(len(dict_list))}
        #rep=report(data_dic, [], ['volume', 'voxelCount', 'totalOverlappingRatio', 'meanIntensity'])
        #report_to_xls(rep,  workbook)
    
        #Create graphical report
        #graphical_report(data_dic, workbook)
   
        
        
        # Close the Pandas Excel writer and save Excel file.
        writer.save()

    elif to_text==True:

        with open(os.path.join(path, file_name + '.txt'), 'w') as outputFile:

            for i in range(len(dataframe_list)):
                #if dataframe_list[i] != None:
                outputFile.write('name= '+name_list[i]+'\n')
                outputFile.write(dataframe_list[i].to_csv(sep='\t', columns=key_order_list[i], index=False, header=True))    
    
       


################################Functions######################################
def report(dictionary, measurement_list, parameter_list ):
    """Generate a statistical reports for databases in 'dict_list'. Statistical 
    descriptors are listed in 'measurement_list' and has to be among the keys 
    in 'STAT_FUNCTIONS' (a dictionary of functions). Parameters to be included 
    in the analyzed are listed'parameter_list'. Report is a standard 
    dictionary with a key for each elements in 'dict_list'. Each dictionary 
    contains a dictionary for each parameter. Parameter dictionaries contain a 
    key:value pair for each measurement.
    
    @param: parameter_list list of parameters in dict_list to be analyzed
    @param: measurement_list list of statistical descriptors eg. mean 
            (see 'STAT_FUNCTIONS' that contains)
    """
    
    
    database_list=[dictionary[key] for key in dictionary.keys()]
    name_list=list(dictionary.keys())
    
    #If no stat_list hase been given common keys will be lsited first and
    #all other keys appended to the list
    if parameter_list==None or parameter_list==[] :
        parameter_list=common_keys(database_list)


    #Remove elements of stat_list not in STAT_FUNCTIONS
    if measurement_list==None or measurement_list==[] :
        measurement_list=STAT_FUNCTIONS.keys()
    else:    
        for st in measurement_list:
            if st not in STAT_FUNCTIONS.keys():
                del st
    
    #Generate descriptive statistics
    results={}
    for idx, dic in enumerate(database_list):

        parameters={}
        for key in parameter_list:
            
            measurements={}
            for idx2, meas_key in enumerate(measurement_list):
      
                measurements[meas_key]='n/a'
                
                if key in dic:
                    try:
                        measurements[meas_key]=STAT_FUNCTIONS[meas_key](dic[key])
                    except:
                        pass
                    
            parameters[key]=measurements
          
     
        results[str(name_list[idx])]=parameters
    
    #Generate violin plots and add to 
    #graphs=[]
    #for ind , key in enumerate(parameter_list):
        #data_list=[database[key]  for database in database_list if key in database.keys()]
        #graphs.append(violin_plot(data_list, data_name_list=name_list, labelx='', labely=key))      
    
    return results#, graphs

    
def report_to_xls(dictionary, workbook ):
    """Print report dictionary to table form. Parameters are listed so the 
    common elements come first in the table.
    
    @param: dictionary Dictionary from report function
    @param: workbook xlsxwriter Workbook object
    @param: parameter_list
    """
    
    database_list=[dictionary[key] for key in dictionary.keys()]
    name_list=list(dictionary.keys())


    
    #Get list of parameters and statistical descriptors for each parameter
    #Lists have to start with common keys
    stats=[]
    parameters= common_keys(database_list)
    for sub_dic in database_list:
        
        for k in sub_dic.keys():
            if k not in parameters:
                parameters.append(k)
        
        stat_list=[sub_dic[element] for element in sub_dic]
        stats_element=common_keys(stat_list)

        stats_element= common_keys(stat_list)
        for stat_dict in stat_list:
            for key in stat_dict.keys():
                if key not in stats_element:
                    stats_element.append(key)
        stats.append(list(stats_element))
    
    #Generate a new worksheet named 'Report'
    worksheet_name=next_worksheet_name(workbook, 'Report') 
    worksheet = workbook.add_worksheet(worksheet_name)

    
    # Create a format to use in the merged range
    merge_format = workbook.add_format({
        'bold': 1,
        'border': 1,
        'align': 'center',
        'valign': 'vcenter'})
    
    #Add data and formatting 
    column_start=0;column_end=0
    for i, db in enumerate(database_list):
        
        column_end=column_start+(len(stats[i]))+1
        
        #add header lines
        worksheet.merge_range(0,column_start, 0, column_end-1, name_list[i] , merge_format)
        worksheet.write_row(1,column_start+1, stats[i])
        
        #Add Formattiong
        for idx, elm in enumerate(stats[i]):
            worksheet.set_column(column_start+1+idx, column_start+1+idx, 5+len(elm)*0.7, merge_format)
        
        worksheet.set_column(column_start, column_start, 5+longest_element(list(db.keys()))*0.7, merge_format)
        
        for j, key in enumerate(parameters):
            
            row=[str(key)]
           
            for k, meas_key in enumerate(stats[i]):
               
                value='n/a'
                if key in sub_dic.keys():
                    if meas_key in db[key].keys():
                        value=str(db[key][meas_key])
                
                row.append(value)
            
            worksheet.write_row(j+2,column_start, row)
   
        column_start+=((len(stats[i])+2))   
    
    #Add violin plots of measured parameters
    #if  isinstance(graphs, list) and len(graphs)>0:
        #for ind, gra in enumerate(graphs):
            #worksheet.insert_image(ind*27, column_end+1, 'Hist.png', {'image_data':figure_to_stream(gra),'x_scale': 1, 'y_scale': 1, 'x_offset': 0, 'y_offset': 0}) 

           
def plot_hist(data,  title=None, label='Data', colormap='viridis', binning='fd', 
              histtype='stepfilled', facecolor='green', alpha=0.9, grid=True, dpi=300):
    """Generate a histogram as matplotlib Figure.
    
    @param: data list or array of data.
    @param: label str to be displyed as label of the y axis
    @param: title str to be displyed as graph title
    @param: colormap Name (str) of matplotlib colormap eg. 'coolwarm',
                    'inferno', 'Reds', 'OrRd', 'RdBu', 'magma', 'viridis', 
                    'summer', spectral (see docs of matplotlib colormaps).
    @param: binning If numeric it is the number of bins, if string then an 
                    automatic estimator is used 'auto', 'sturges', 'fd', 
                    'doane', 'scott', 'rice', 'sturges' or 'sqrt'.
    @param: facecolor set the color of histogram. Overwritten if colormap is
                     not None             
    @param: alpha  The alpha value of the histogram
    @param: histtype Type of histogram eg. 'bar', 'barstacked', 'step', 
            'stepfilled' (see docs fro matplotlib.pyplot.hist)               
    @param: grid Show or hide grid
    @param: dpi Resolution of the figure
    @param: aspect ration numeric or 'auto', 'equal' (see matplotlib.axes.Axes.set_aspect documentation))
    
    """
    
    #Get interactive mode and torn off if on
    current_mode=is_interactive()
    if current_mode:
        plt.ioff()
    
    # Create figure and histogram
    fig, axs = plt.subplots(1, 1, sharey=True, tight_layout=True)
    fig.set_dpi(dpi)   

    N, bins, patches =axs.hist(data, binning, normed=1, facecolor=facecolor, alpha=alpha)
  
    bin_text='No.='+str(len(bins))+' width='+str(round(bins[1]-bins[0]))
    #Add colormap if needed
    if colormap!=None:
        
        # Normalize bar height to 0..1 to the full range of the colormap
        rel_freq= N / N.max()
        
        norm = colors.SymLogNorm(linthresh=0.03, linscale=1, vmin=rel_freq.min(), vmax=rel_freq.max())
        colormap=plt.get_cmap(colormap)
        
        # Add colors
        for frq, patch in zip(rel_freq, patches):
            color = colormap(norm(frq))
            patch.set_facecolor(color)
    
    if title==None:
        title='Histogram: '+label+': Bins('+bin_text+')'
   
    # Add formatting
    #axs.yaxis.set_major_formatter(PercentFormatter(xmax=1))
    axs.set_xlabel(label)
    axs.set_ylabel('Rel. Freq.')
    axs.set_title(title)
    axs.axis([bins.min()*0.1-bins.min(), bins.max()+bins.max()*0.1, 0,  N.max()+N.max()*0.20])
    axs.set_axisbelow(True)
    axs.grid(grid)

    #Set interactive mode to its starting state
    interactive(current_mode)
    
    return fig


def graphical_report(dictionary, workbook):
    """Generate a graphical report. A new workshhet is generated with a 
    histogram for each parameter.
    
    @param: dict_list list or array of databases.
    @param: workbook xlsxwriter Workbook object
    """
    database_list=[dictionary[key] for key in dictionary.keys()]
    name_list=list(dictionary.keys())
    
    #Generate a new worksheet named 'Report'
    worksheet_name=next_worksheet_name(workbook, 'Report_Graphical')  
    worksheet = workbook.add_worksheet(worksheet_name)

    #Create a list of parameters that starts with common elements
    parameters= common_keys(database_list)
    for db in database_list:
        for k in db.keys():
            if k not in parameters:
                parameters.append(k)
   
    #Cell formating
    cell_format = workbook.add_format({
        'font_size':14,
        'bold':True,
        'underline':True,
        'align': 'center',
        'valign': 'vcenter'})    
    cell_width=longest_element(parameters)+15  
    
    #Create graphical result
    y_spacing=32 
    x_spacing=13
    x_offset=0
    for i, dic in enumerate(database_list):
        
        y_offset=0
        
        for j, param in enumerate(parameters):
            
            worksheet.write(y_offset, x_offset, 'Histogram: '+str(name_list[i])+' : '+str(param),cell_format)
            worksheet.set_column(y_offset, y_offset, cell_width, cell_format)
            
            if param in dic.keys():
                figure= plot_hist(dic[param],str(name_list[i])+':'+str(param))
                worksheet.insert_image(y_offset+2, x_offset, 'Hist.png', {'image_data':figure_to_stream(figure),'x_scale': 1, 'y_scale': 1, 'x_offset': 0, 'y_offset': 0})
                plt.close(figure)
                
            y_offset+=y_spacing

        x_offset+=x_spacing
        
    #plt.close('all')

def database_to_xls(dictionary, workbook):
    """ 
    
    @param: dict_list list or array of databases.
    @param: workbook xlsxwriter Workbook object
    """
    database_list=[dictionary[key] for key in dictionary.keys()]
    name_list=list(dictionary.keys())
    
    
def violin_plot(data_list, color_list=None,  data_name_list=None, alpha=0.5 , rotation=60, labelx='', labely=''):
    
    def set_violinplot_style(violin, color_list, alpha):
        
        for partname in ('cbars','cmins','cmaxes','cmeans','cmedians'):
            
            if partname in violin:
                #violin[partname].set_facecolor('black')
                violin[partname].set_edgecolor(color_list)
                violin[partname].set_linewidth(1)
               
        for i in range(len(violin['bodies'])):
            
            #violin['bodies'][i].set_facecolor(lineColors)
            violin['bodies'][i].set_edgecolor(color_list[i])
            violin['bodies'][i].set_alpha(alpha)
            violin['bodies'][i].set_facecolor(color_list[i])
        
    def add_violinplotStats(ax, violin, data_list, color_list):
       
       for i in range(len(data_list)):
           
           quartile1, median, quartile3 = np.percentile(data_list[i], [25,50,75])
           whiskers1=quartile1-1.5*(quartile3-quartile1)
           whiskers3=quartile3+1.5*(quartile3-quartile1)
    
           ax.scatter([i+1]*5, [whiskers1, quartile1, median,quartile3,whiskers3], marker='_', color=color_list[i], s=100, zorder=3)
           ax.vlines(i+1, quartile1, quartile3, color=color_list[i], linestyle='-', lw=5)
           ax.vlines(i+1, whiskers1, whiskers3, color=color_list[i], linestyle='-', lw=1)
    
    # generate name lsit
    if not isinstance(data_name_list, list):
        data_name_list=['Series '+str(i+1) for i in range(len(data_list))]
    else:
         data_name_list=[str(data_name_list[i]) for i in range(len(data_name_list))]
    
    if len(data_name_list)<len(data_list):
        length=len(data_name_list)
        for i in range(len(data_list)-len(data_name_list)):
            data_name_list.append('Series '+str(length+i+1))   

    #Generate or correct color list
    colormap=plt.get_cmap('gist_rainbow')#'tab20b'   
    
    if not isinstance(color_list, list):
        color_list=COLOR_LIST

    else:
        for i in range(len(color_list)):
            if not colors.is_color_like(color_list[i]):
                color_list[i]=colormap(random.uniform(0, 1))
    
    if len(color_list)<len(data_list):
        for i in range(len(data_list)-len(color_list)):
            color_list.append(colormap(random.uniform(0, 1)))
    
    
    #Get interactive mode and torn off if on
    current_mode=is_interactive()
    if current_mode:
        plt.ioff()

    #Create Figure
    fig, axes = plt.subplots(nrows=1, ncols=1)
    fig.set_facecolor('white')
    
    # Violin plot
    violin=axes.violinplot(data_list,
                       showmeans=False,
                       showmedians=False, showextrema=False,points=100, widths=0.3)
    
    #Add violinplot
    set_violinplot_style(violin, color_list, alpha)
    add_violinplotStats(axes, violin, data_list, color_list)
    

    #Format axes
    axes.yaxis.grid(True)
   
    axes.set_xticks([y+1 for y in range(len(data_list))])
    
    for tick in axes.get_xticklabels():
        tick.set_rotation(rotation)
    
    axes.set_xlabel(labelx)
    axes.set_ylabel(labely)

    # add x-tick labels    
    plt.setp(axes, xticks=[y+1 for y in range(len(data_list))], xticklabels=data_name_list)
    
    #set layout
    plt.tight_layout()
    
    #Set interactive mode to its starting state
    interactive(current_mode)
        
    return fig


def box_plot(data_list, color_list=None, data_name_list=None, alpha=0.5,  rotation=60, labelx='', labely=''):

    
    def set_boxplot_style(box, color_list, alpha):
    
        for i in range(len(box['boxes'])):
            box['boxes'][i].set_facecolor(color_list[i])
            box['boxes'][i].set_alpha(alpha)
            for element in ['whiskers', 'fliers', 'medians', 'caps']:
                plt.setp(box[element][i], color='black')       
        
    
    def add_scatter_data(ax, data_list, color_list):
           
        for j in range(len(data_list)):
            ax.scatter([j+1]*len(data_list[j]), data_list[j], marker='*', color=color_list[j], s=30, zorder=3)

    # generate name lsit
    if not isinstance(data_name_list, list):
        data_name_list=['Series '+str(i+1) for i in range(len(data_list))]
    else:
         data_name_list=[str(data_name_list[i]) for i in range(len(data_name_list))]
    
    if len(data_name_list)<len(data_list):
        length=len(data_name_list)
        for i in range(len(data_list)-len(data_name_list)):
            data_name_list.append('Series '+str(length+i+1))   

    #Generate or correct color list
    colormap=plt.get_cmap('gist_rainbow')#'tab20b'   
    
    if not isinstance(color_list, list):
        color_list=COLOR_LIST

    else:
        for i in range(len(color_list)):
            if not colors.is_color_like(color_list[i]):
                color_list[i]=colormap(random.uniform(0, 1))

    if len(color_list)<len(data_list):
        for i in range(len(data_list)-len(color_list)):
            color_list.append(colormap(random.uniform(0, 1)))
  

    #Get interactive mode and torn off if on
    current_mode=is_interactive()
    if current_mode:
        plt.ioff()

    #Create figure
    fig, axes = plt.subplots(nrows=1, ncols=1)
    fig.set_facecolor('white')
    
    # Boxplot
    box=axes.boxplot(data_list,notch=False,  # notch shape
                             vert=True,   # vertical box aligmnent
                             patch_artist=True,showmeans=False, meanline=False,)
    
    set_boxplot_style(box, color_list, alpha)
        
        
    add_scatter_data(axes, data_list, color_list)
    
    #Format axes
    # adding horizontal grid lines
    axes.yaxis.grid(True)
   
    axes.set_xticks([y+1 for y in range(len(data_list))])
    
    for tick in axes.get_xticklabels():
        tick.set_rotation(rotation)
    
    axes.set_xlabel(labelx)
    axes.set_ylabel(labely)
    
    # add x-tick labels    
    plt.setp(axes, xticks=[y+1 for y in range(len(data_list))], xticklabels=data_name_list)
    
    #set layout
    plt.tight_layout()
    
    #Set interactive mode to its starting state
    interactive(current_mode)
    

   
    return fig
       
       


        

    
###############################Utilities####################################### 


def longest_element(lst):
    return max(len(str(s)) for s in lst)


def common_keys(dict_list):
   
    keylists=[list(dic.keys()) for dic in dict_list]
   
    return list(set.intersection(*map(set,keylists)))    


def worksheet_names(workbook):
    
    wsheets=workbook.worksheets()
    
    return [sheet.get_name() for sheet in wsheets ]

    
def next_name(name, name_list):
    
    i=1
    final_name=name
    while name in name_list:
         final_name=name+'_'+str('{:03d}'.format(i))
         i += 1

    return final_name

def next_worksheet_name(workbook, name):
    
    name_list=worksheet_names(workbook)
        
    #If worksheet ''Reports_Graphical' exists generate a name with counter
    if name in name_list:
        name=next_name(name, name_list)
        
    return name 


def db_list_to_dict(database_list, name_list=None):
    
    
    #Create default names if not supplied
    if not isinstance(name_list, list):
        name_list=['Database_'+str(i) for i in range(len(database_list))]
    
    #Check if the length of name_list and database_list matches
    if len(database_list)!=len(name_list):
        raise Exception('Number of databases does not match the number of names!')
    
    #Create output dictionary
    output={}
    for idx, dic in enumerate(database_list):
        output[name_list[idx]]=database_list[idx]
        
    return output
  
    
def figure_to_stream(fig, width=10, height=5):
    """
    Generate a png Byte stream from matplotlib figure
    
    @param: fig matplotlib figure
    @param: width image width in inches
    @param: height image height in inches
    """
    stream=BytesIO()
    

    
    fig.set_size_inches(width,height)
    fig.savefig(stream, facecolor=fig.get_facecolor(), format='png', quality=100, dpi=400, edgecolor='Black')
    

    return stream    
        
############################Statistical Function###############################
def q1(x):
    return np.percentile(x, 25)

def q3(x):
    return np.percentile(x, 75)


STAT_FUNCTIONS={'Mean':np.mean,
       'Median':np.median,
       'First Quartile (Q\u2081)':q1,
       'Third Quartile (Q\u2083)':q3}

COLOR_LIST=['green', 'red', 'darkorange', 'blue', 'magenta', 'crimson' ]

if __name__ == '__main__':
    
    #Test Dictionary
    dictionary_ch1={'egyketto':[1,1,2,2] ,'kettoharom':[2,2,3,3]}
    dictionary_ch2={'egyketto':[1,1,2,2] ,'kettoharom':[2,2,3,3],'haromnegy':[3,3,4,4]}
    
    test_dict={'ch1g':dictionary_ch1, 'ch2g':dictionary_ch2, 'ovl':dictionary_ch1}
    
    # Create an new Excel file and add a worksheet.
    workbook = xlsxwriter.Workbook('merge1.xlsx')
    
    #Create statistical report
    rep=report(test_dict, [], [] )
    report_to_xls(rep[0], rep[1], workbook)
    
    #Create graphical report
    graphical_report(test_dict, workbook)
   
    #Close
    workbook.close()
    
    '''
    #Data
    A_colocalized=[926,	574,	1342,	477,	714,	1728,	1486,	2805,	2563,	868,	723,	347,	734,	2665,	1939,	1269,	1149]
    A_nonColocalized=[926,	574,	1342,	477,	714,	1728,	1486,	2805,	2563,	868,	723,	347,	734,	2665,	1939,	1269,	1149]
    B_colocalized=[521,	604,	642,	389,	437,	400,	369,	464,	742,	726,	773,	822,	819,	836,	990,	1065,	865,	833,	1385,	1747,	1278,	1101,	1210,	1104]
    B_nonColocalized=[521,	604,	642,	389,	437,	400,	369,	464,	742,	726,	773,	822,	819,	836,	990,	1065,	865,	833,	1385,	1747,	1278,	1101,	1210,	1104]
    
    #Settings
    data =[[1237, 1217, 903, 457, 312, 1077, 977, 892, 1208, 1453, 411, 181, 302, 350, 1080, 843, 522], [275, 399, 534, 620, 265, 618, 429, 757, 1263, 1231, 1288, 1078, 741, 617, 1225, 1506, 1212, 1172, 318, 364, 807, 647, 508, 722]]#[A_colocalized, A_nonColocalized, B_colocalized, B_nonColocalized]

    color_list=['green', 'red', 'green','red']
    color_list=['red']
    alpha=0.5
    data_name_list=['A', 'B', 'B Colocalized', 'B non-Colocalized']
    data_name_list=['A']
    rotation=60
    
    labelx='Data'
    labely='Number of non-Colocalizing vGlut Objects'
    
    
    
    outputpath=outputPath='E:\Results\Results_ovlVol1_REDO'
    saveFormat='png'
    
    #main(data, faceColor, lineColor, alpha, labels, rotation, labelx, labely, outputpath, saveFormat)   

    violin_plot(data, labelx=labelx, labely=labelx)
    box_plot(data, labelx=labelx, labely=labelx)
    '''

    


