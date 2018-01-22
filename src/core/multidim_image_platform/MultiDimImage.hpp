#ifndef _core_multidim_image_platform_MultiDimImage_h_
#define _core_multidim_image_platform_MultiDimImage_h_

#include <initializer_list>
#include <functional>
#include <typeinfo>
#include <unordered_map>

namespace core {
namespace multidim_image_platform {

    struct Type {
        typedef void type;
        size_t size;
        size_t hash;

        bool operator==(const Type& other) const
        {
            return other.hash == hash;
        }

        bool operator!=(const Type& other) const
        {
            return !(*this == other);
        }
    };

    template <typename T>
    struct GetType : public Type {
        typedef T type;
        GetType()
        {
            size = sizeof(T);
            hash = typeid(T).hash_code();
        }
    };

    class Meta {
    public:
        template <typename T>
        void add(const std::string& tag, T value);

        template <typename T>
        T get(const std::string& tag);

        template <typename T>
        bool has(const std::string& tag);

        template <typename T>
        void remove(const std::string& tag);
    private:
        struct Item {
            Type type;
            std::unique_ptr<char> data;
        };
        std::unordered_map<std::string, Item> m_items;
    };

    template <typename T>
    class MultiDimImage {
    public:
        class View {
        public:
            bool empty() const;
            size_t size() const;
            size_t dims() const;
            size_t dim(size_t d) const;
            size_t byteSize() const;
            bool valid() const;
            T& at(std::initializer_list<size_t> coords);
            // TODO: do not expose a raw pointer
            MultiDimImage<T>* parent();
        };
    public:
        MultiDimImage(std::initializer_list<size_t> dims = {0});
        MultiDimImage(const MultiDimImage& other);
        MultiDimImage& operator=(const MultiDimImage& other);
        template <typename U>
        MultiDimImage& operator=(const MultiDimImage<U>& other); // indicate error
        template <typename U>
        void convertCopy(const MultiDimImage<U>& other);
        bool empty() const;
        size_t size() const;
        size_t dims() const;
        size_t dim(size_t d) const;
        size_t byteSize() const;
        Type type() const;
        void clear();
        T& at(std::initializer_list<size_t> coords);
        View plane(std::initializer_list<size_t> coords);
        View volume(std::initializer_list<size_t> coords);
        void reorderDims(std::initializer_list<size_t> dims);
        virtual ~MultiDimImage();
        Meta meta;
    };

    #include "MultiDimImage.ipp"

}}

#endif
