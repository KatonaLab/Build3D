#include "MultiDimFile.h"

MultiDimFile::MultiDimFile(const std::string &filename)
    : m_filename(filename)
{}

std::string MultiDimFile::filename() const
{
    return m_filename;
}

MultiDimFile::~MultiDimFile()
{}
