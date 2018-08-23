#include "IcsAdapter.h"

#include <libics.h>
#include <libics_sensor.h>
#include <map>

using namespace std;
using namespace core;
using namespace core::io_utils;
using namespace core::multidim_image_platform;

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
        m_ip = nullptr;
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
                m_ip = nullptr;
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
    m_orderMap = orderMap;

    // check valid type
    dataType();

    // test for valid .ids
    uint32_t dummy;
    if (IcsGetDataBlock(m_ip, &dummy, sizeof(uint32_t)) != IcsErr_Ok) {
        close();
        m_ip = nullptr;
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

Meta IcsAdapter::getMeta()
{
    Meta meta;
    string typeName;
    switch (m_dt) {
        case Ics_uint8: typeName = "uint8"; break;
        case Ics_sint8: typeName = "int8"; break;
        case Ics_uint16: typeName = "uint16"; break;
        case Ics_sint16: typeName = "int16"; break;
        case Ics_uint32: typeName = "uint32"; break;
        case Ics_sint32: typeName = "int32"; break;
        case Ics_real32: typeName = "float"; break;
        case Ics_real64: typeName = "double"; break;
        case Ics_complex32:
        case Ics_complex64:
        case Ics_unknown:
        default: typeName = "unknown";
    }
    meta.add("type", typeName);
    meta.add("path", m_filename);

    if (valid()) {

        // general info
        size_t nbits = 0;
        IcsGetSignificantBits(m_ip, &nbits);
        meta.add("IcsGetSignificantBits", to_string(nbits));

        // image metadata
        char coord[256];
        IcsGetCoordinateSystem(m_ip, coord);
        meta.add("IcsGetCoordinateSystem", string(coord));

        double origin = 0, scale = 0;
        char units[256];
        IcsGetImelUnits(m_ip, &origin, &scale, units);
        meta.add("IcsGetCoordinateSystem:origin", to_string(origin));
        meta.add("IcsGetCoordinateSystem:scale", to_string(scale));
        meta.add("IcsGetCoordinateSystem:units", string(units));

        for (auto order: m_orderMap) {
            int dimension = order.second;
            string postfix = string(":") + order.first;
            IcsGetPosition(m_ip, dimension, &origin, &scale, units);
            meta.add("IcsGetPosition:origin" + postfix, to_string(origin));
            meta.add("IcsGetPosition:scale" + postfix, to_string(scale));
            meta.add("IcsGetPosition:units" + postfix, string(units));
        }

        // sensor info
        int n = IcsGetSensorChannels(m_ip);
        meta.add("IcsGetSensorChannels", to_string(n));
        meta.add("IcsGetSensorModel", string(IcsGetSensorModel(m_ip)));

        #define PER_SENSOR(fn) meta.add(string(#fn), to_string(fn(m_ip)));
        PER_SENSOR(IcsGetSensorMediumRI)
        PER_SENSOR(IcsGetSensorLensRI)
        PER_SENSOR(IcsGetSensorNumAperture)
        PER_SENSOR(IcsGetSensorPinholeSpacing)
        #undef PER_SENSOR

        for (int i = 0; i < n; ++i) {
            string postfix = ":" + to_string(i);
            #define PER_CHANNEL(fn) meta.add(string(#fn) + postfix, to_string(fn(m_ip, i)));
            #define PER_CHANNEL_STR(fn) meta.add(string(#fn) + postfix, string(fn(m_ip, i)));
            PER_CHANNEL_STR(IcsGetSensorType)
            PER_CHANNEL_STR(IcsGetSensorSTEDDepletionMode)
            PER_CHANNEL(IcsGetSensorPhotonCount)
            PER_CHANNEL(IcsGetSensorPinholeRadius)
            PER_CHANNEL(IcsGetSensorExcitationWavelength)
            PER_CHANNEL(IcsGetSensorEmissionWavelength)
            PER_CHANNEL(IcsGetSensorSTEDLambda)
            PER_CHANNEL(IcsGetSensorSTEDSatFactor)
            PER_CHANNEL(IcsGetSensorSTEDImmFraction)
            PER_CHANNEL(IcsGetSensorSTEDVPPM)
            PER_CHANNEL(IcsGetSensorDetectorPPU)
            PER_CHANNEL(IcsGetSensorDetectorBaseline)
            PER_CHANNEL(IcsGetSensorDetectorLineAvgCnt)
            #undef PER_CHANNEL_STR
            #undef PER_CHANNEL
        }

    }
    return meta;
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
