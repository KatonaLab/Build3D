
import xlsxwriter
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import colors
from matplotlib.ticker import PercentFormatter
from io import BytesIO



#Statistical functions
def q1(x):
    return np.percentile(x, 25)

def q3(x):
    return np.percentile(x, 75)

def longest_element(lst):
    return max(len(str(s)) for s in lst)

def common_keys(dict_list):
   
    keylists=[list(dic.keys()) for dic in dict_list]
   
    return list(set.intersection(*map(set,keylists)))

#Utilities
def get_worksheet_names(workbook):
    
    wsheets=workbook.worksheets()
    
    return [sheet.get_name() for sheet in wsheets ]

    
def get_next_name(name, name_list):
    
    i=1
    final_name=name
    while name in name_list:
         final_name=name+'_'+str('{:03d}'.format(i))
         i += 1

    return final_name



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
    
#Constants
STAT_FUNCTIONS={'Mean':np.mean,
       'Median':np.median,
       'First Quartile (Q\u2081)':q1,
       'Third Quartile (Q\u2083)':q3}





def report(dict_list, measurement_list, parameter_list ):
    
    #If no stat_list hase been given common keys will be analyzed
    if parameter_list==None or parameter_list==[] :
        parameter_list=common_keys(dict_list)
        
    #Remove elements of stat_list not in STAT_FUNCTIONS
    if measurement_list==None or measurement_list==[] :
        measurement_list=STAT_FUNCTIONS.keys()
    else:    
        for st in measurement_list:
            if st not in STAT_FUNCTIONS.keys():
                del st

    results={}
    for idx, dic in enumerate(dict_list):

        parameters={}
        for key in parameter_list:
            
            measurements={}
            for idx2, meas_key in enumerate(measurement_list):
                
                if key in dic:
                    measurements[meas_key]=STAT_FUNCTIONS[meas_key](dic[key])
                else:
                    measurements[meas_key]='n/a'
            
            parameters[key]=measurements
          
     
        results[str(idx)]= parameters
       
    return results

def graphical_report(dict_list, workbook ):
    
    
    #Generate a new worksheet named 'Report'
    name='Report_Graphical'
    name_list=get_worksheet_names(workbook)
    
    #If worksheet ''Reports_Graphical' exists generate a name with counter
    if name in name_list:
        name=get_next_name(name, name_list)
        
    worksheet = workbook.add_worksheet(name)
    
    # Create a format to use in the merged range.
    merge_format = workbook.add_format({
        'bold': 1,
        'border': 1,
        'align': 'center',
        'valign': 'vcenter'})
    

    parameters= common_keys(dict_list)
    for dic in dict_list:
        for k in dic.keys():
            if k not in parameters:
                parameters.append(k)
   
    y_spacing=20
    y_offset=0
    
    x_spacing=15
    x_offset=0
    for i, dic in enumerate(dict_list):
        
        for j, param in enumerate(parameters):
            
            if param in dic.keys():
                print(param)
                print(dic[param])
                figure= plot_hist(dic[param], str(param))
                worksheet.insert_image(x_offset,y_offset , 'Hist.png', {'image_data':figure_to_stream(figure, width=8, height=6),'x_scale': 1, 'y_scale': 1})
                print(y_offset)
                print(x_offset)
            y_offset+=j*y_spacing

        x_offset+=i*x_spacing

    plt.close('all')
    


       
   




def plot_hist(data,  label='Data', colormap='viridis', binning='fd', 
              histtype='stepfilled', facecolor='green', alpha=0.9, grid=True, dpi=300):
    
    """Generate a histogram as matplotlib figure.
    
    @param: data list or array of data.
    
    @param: label str to be displyed as label of the y axis

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
    
    #Disable matplotlib interactive mode
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
    
    
    title='Histogram of '+label+': Bins('+bin_text+')'
   
    # Add formatting
    #axs.yaxis.set_major_formatter(PercentFormatter(xmax=1))
    axs.set_xlabel(label)
    axs.set_ylabel('Rel. Freq.')
    axs.set_title(title)
    axs.axis([bins.min()*0.1-bins.min(), bins.max()+bins.max()*0.1, 0,  N.max()+N.max()*0.20])
    axs.set_axisbelow(True)
    axs.grid(grid)

    plt.ion()
    
    return fig


def figure_to_stream(fig, width=8, height=6):
    """
    Generate a png Byte stream from matplotlib figure
    
    @param: fig matplotlib figure
    @param: width image width in inches
    @param: height image height in inches
    """
    stream=BytesIO()
    
    plt.ioff()
    
    fig.set_size_inches(width,height)
    fig.savefig(stream, format='png', quality=100, dpi=400)
    
    
    
    plt.ion()
    
    return stream


def report_to_xls(workbook, dic):
    
    dic_list=[dic[element] for element in dic]
    dic_name_list=[element for element in dic]
    
    #Get list of parameters and statistical descriptors for each parameter
    #Lists have to start with common keys
    stats=[]
    parameters= common_keys(dic_list)
    for sub_dic in dic_list:
        
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
    name='Report'
    name_list=get_worksheet_names(workbook)
    
    #If worksheet ''Reports' exists generate a name with counter
    if name in name_list:
        name=get_next_name(name, name_list)
        
    worksheet = workbook.add_worksheet(name)
    
    # Create a format to use in the merged range.
    merge_format = workbook.add_format({
        'bold': 1,
        'border': 1,
        'align': 'center',
        'valign': 'vcenter'})
    
    #Add data and formatting 
    column_start=0;column_end=0
    for i, sub_dic in enumerate(dic_list):
        
        column_end=column_start+(len(stats[i]))+1
        
        #add header lines
        worksheet.merge_range(0,column_start, 0, column_end-1, dic_name_list[i] , merge_format)
        worksheet.write_row(1,column_start+1, stats[i])
        
        #Add Formattiong
        for idx, elm in enumerate(stats[i]):
            worksheet.set_column(column_start+1+idx, column_start+1+idx, 5+len(elm)*0.7, merge_format)
        
        worksheet.set_column(column_start, column_start, 5+longest_element(list(sub_dic.keys()))*0.7, merge_format)
        
        for j, key in enumerate(parameters):
            
            row=[str(key)]
           
            for k, meas_key in enumerate(stats[i]):
               
                value='n/a'
                if key in sub_dic.keys():
                    if meas_key in sub_dic[key].keys():
                        value=str(sub_dic[key][meas_key])
                
                row.append(value)
            
            worksheet.write_row(j+2,column_start, row)
   
        column_start+=((len(stats[i])+2))   
       
           
    
    
        


#Test Dictionary
dictionary_ch1={'egyketto':[1,1,2,2] ,'kettoharom':[2,2,3,3]}
dictionary_ch2={'egyketto':[1,1,2,2] ,'kettoharom':[2,2,3,3],'haromnegy':[3,3,4,4]}



# Create an new Excel file and add a worksheet.
workbook = xlsxwriter.Workbook('merge1.xlsx')

res=report([dictionary_ch1, dictionary_ch2, dictionary_ch1], [], [] )
#res={'0': {'kettoharom': {'Mean': 2.5,'FASZSAG': 'fasz', 'Median': 2.5, 'First Quartile (Q₁)': 2.0, 'Third Quartile (Q₃)': 3.0}, 'egyketto': {'Mean': 1.5, 'Median': 1.5, 'First Quartile (Q₁)': 1.0, 'Third Quartile (Q₃)': 2.0}}, '1': {'kettoharom': {'Mean': 2.5, 'Median': 2.5, 'First Quartile (Q₁)': 2.0, 'Third Quartile (Q₃)': 3.0}, 'egyketto': {'Mean': 1.5, 'Median': 1.5, 'First Quartile (Q₁)': 1.0, 'Third Quartile (Q₃)': 2.0}}, '2': {'kettoharom': {'Mean': 2.5, 'Median': 2.5, 'First Quartile (Q₁)': 2.0, 'Third Quartile (Q₃)': 3.0}, 'egyketto': {'Mean': 1.5, 'Median': 1.5, 'First Quartile (Q₁)': 1.0, 'Third Quartile (Q₃)': 2.0}, 'futyi': {'Mean': 1.5, 'Median': 1.5, 'First Quartile (Q₁)': 1.0, 'Third Quartile (Q₃)': 2.0}}}

report_to_xls(workbook,res)
graphical_report([dictionary_ch1, dictionary_ch2, dictionary_ch1], workbook)
#report(workbook, [dictionary_ch1, dictionary_ch2, dictionary_ch1] , measurement_list=['egyketto','kettoharom','haromnegy']).close() #[dictionary_ch1, dictionary_ch1]


workbook.close()







