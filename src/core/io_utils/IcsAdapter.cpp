#include "IcsAdapter.h"

#include <libics.h>
#include <map>

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

    map<char, int> orderMap;
    for (int i = 0; i < n; ++i) {
        char order[ICS_STRLEN_TOKEN];
        char label[ICS_STRLEN_TOKEN];
        IcsGetOrder(m_ip, i, order, label);
        char d = tolower(order[0]);
        switch (d) {
            case 'x':
            case 'y':
            case 'z':
            case 'c':
            case 't':
                orderMap[d] = i;
                break;
            default:
                throw std::runtime_error("unknown dimension '" + string(order) + "'");
        }
    }

    vector<char> dstOrder = {'x', 'y', 'z', 't', 'c'};
    for (int i = 0; i < (int)dstOrder.size(); ++i) {
        char d = dstOrder[i];
        if (orderMap.count(d)) {
            m_reorder.push_back(orderMap[d]);
        }
    }

    // check valid type
    dataType();

    // test for valid .ids
    uint32_t dummy;
    if (IcsGetDataBlock(m_ip, &dummy, sizeof(uint32_t)) != IcsErr_Ok) {
        close();
        throw std::runtime_error("ics/ids data read error '" + filename + "', check the .ics file and also the corresponding .ids file");
    }
    std::rewind(((Ics_BlockRead*)m_ip->blockRead)->dataFilePtr);

    return true;
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
