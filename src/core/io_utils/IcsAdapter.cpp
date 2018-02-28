#include "IcsAdapter.h"

#include <libics.h>

using namespace std;
using namespace core;
using namespace core::io_utils;

#define ICS_EC(call) errorCheck(call, std::string(#call) + " - " + m_filename)

void IcsAdapter::errorCheck(Ics_Error error, const std::string message)
{
    if (error != IcsErr_Ok) {
        throw std::runtime_error(message);
    }
}

bool IcsAdapter::open(const std::string& filename)
{
    {
        IcsAdapter freshAdapter;
        swap(freshAdapter, *this);
    }

    m_filename = filename;
    
    if (IcsOpen(&m_ip, filename.c_str(), "r") != IcsErr_Ok) {
        m_ip = nullptr;
        throw std::runtime_error("can not open ics file '" + filename + "'");
    }
    int n = 0;
    if (IcsGetLayout(m_ip, &m_dt, &n, m_dims.data()) != IcsErr_Ok) {
        throw std::runtime_error("can not read ics layout '" + filename + "'");
    }
    m_dims.resize(n);
    m_dims.shrink_to_fit();
    // throw unsupported dtype if needed
    dataType();
    return true;

    // for (int i = 0; i < m_dims.size(); ++i) {
    //     cout << "dim " << i << endl;
    //     cout << " size " << m_ip->dim[i].size << endl;
    //     cout << " origin " << m_ip->dim[i].origin << endl;
    //     cout << " scale " << m_ip->dim[i].scale << endl;
    //     cout << " order " << m_ip->dim[i].order << endl;
    //     cout << " label " << m_ip->dim[i].label << endl;
    //     cout << " unit " << m_ip->dim[i].unit << endl;

    //     // char order[ICS_STRLEN_TOKEN];
    //     // char label[ICS_STRLEN_TOKEN];
    //     // ICS_EC(IcsGetOrder(m_ip, i, order, label));
    //     // cout << "order: " << order << ", label: " << label << endl;
    // }

    // double origin;
    // double scale;
    // char units[ICS_STRLEN_TOKEN];
    // IcsGetImelUnits(m_ip, &origin, &scale, units);


    // cout << origin << " " << scale << " " << units << endl;

    // char coord[ICS_STRLEN_TOKEN];
    // IcsGetCoordinateSystem(m_ip, &coord);
    // cout << coord << endl;

    // int n;
    // IcsGetNumHistoryStrings(m_ip, &n);
    // cout << n << endl;
    // IcsGetOrder(m_ip, )
    // IcsGetSensorChannels

    // ICS_EC(IcsGetLayout(ip, &dt, &ndims, dims));
    // if (typeMap.count(dt) == 0) {
        // throw ICSError("not supported data type format");
    // }

    // for (int i = 0; i < ndims; ++i) {
    //     char order[ICS_STRLEN_TOKEN];
    //     char label[ICS_STRLEN_TOKEN];
    //     ICS_EC(IcsGetOrder(ip, i, order, label));
    //     channelLabels.push_back(label);
    //     orderLabels.push_back(order);
    // }

    // if (orderLabels[0] == "x") {
    //     cout << "x is the first dimension" << endl;
    //     fillChannelData();
    // } else if (orderLabels[0] == "ch") {
    //     cout << "ch is the first dimension" << endl;
    //     // ch, x, y, z
    //     swap(dims[0], dims[3]); // z, x, y, ch
    //     swap(dims[0], dims[2]); // y, x, z, ch
    //     swap(dims[0], dims[1]); // x, y, z, ch

    //     swap(channelLabels[0], channelLabels[3]);
    //     swap(channelLabels[0], channelLabels[2]);
    //     swap(channelLabels[0], channelLabels[1]);

    //     fillChannelDataWithChannelFirstDim();
    // } else {
    //     throw ICSError("not supported data dimension order");
    // }
}
    
std::type_index IcsAdapter::dataType() const
{
    switch (m_dt) {
        case Ics_uint8: return type_index(typeid(uint8_t));
        case Ics_sint8: return type_index(typeid(int8_t));
        case Ics_uint16: return type_index(typeid(uint16_t));
        case Ics_sint16: return type_index(typeid(int16_t));
        case Ics_uint32: return type_index(typeid(uint32_t));
        case Ics_sint32: return type_index(typeid(int32_t));
        case Ics_real32: return type_index(typeid(float));
        case Ics_real64: return type_index(typeid(double));
        case Ics_complex32:
        case Ics_complex64:
        case Ics_unknown:
        default: throw std::runtime_error("unsupported data type for '" + m_filename + "'");
    }
}

bool IcsAdapter::valid() const
{
    return m_ip != nullptr;
}

void IcsAdapter::close()
{
    if (m_ip) {
        IcsClose(m_ip);
        m_ip = nullptr;
    }
}

IcsAdapter::~IcsAdapter()
{
    close();
}

#undef ICS_EC