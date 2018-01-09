#ifndef _multi_dim_file_h_
#define _multi_dim_file_h_

#include <string>

namespace core {

    class MultiDimFile {
    public:
        MultiDimFile(const std::string &filename);
        virtual std::string filename() const;
        virtual ~MultiDimFile();
    protected:
        virtual bool open() = 0;
        virtual void close() = 0;
    protected:
        const std::string m_filename;
    };
}

#endif