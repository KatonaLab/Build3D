#ifndef _core_multidim_image_platform_MultiDimImage_h_
#define _core_multidim_image_platform_MultiDimImage_h_

#include <algorithm>
#include <chrono>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace core {
namespace multidim_image_platform {

    struct Type {
        std::size_t size;
        std::size_t hash;

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
        GetType()
        {
            size = sizeof(T);
            hash = typeid(T).hash_code();
        }
    };

    class Meta {
    public:
        void add(const std::string& tag, const std::string& value);
        const std::string& get(const std::string& tag);
        bool has(const std::string& tag);
        void remove(const std::string& tag);
        void clear();
    private:
        std::unordered_map<std::string, std::string> m_items;
    };

    template <typename T>
    class MultiDimImage {
    public:
        // TODO: consider making inheritance between MultiDimImage and View
        class View {
            friend class MultiDimImage<T>;
        public:
            View(MultiDimImage<T>* parent,
                std::vector<std::size_t> offset,
                std::vector<std::size_t> dims);
            View(const View& other);
            View& operator=(const View& other);
            bool empty() const;
            std::size_t size() const;
            std::size_t dims() const;
            std::size_t dim(std::size_t d) const;
            std::size_t byteSize() const;
            bool valid() const;
            T& at(std::vector<std::size_t> coords);
            // TODO: do not expose a raw pointer
            MultiDimImage<T>* parent();
            ~View();
        protected:
            std::vector<std::size_t> m_offsets;
            std::vector<std::size_t> m_dims;
            std::vector<std::size_t> m_trueDims;
            MultiDimImage<T>* m_parent;
        };
    public:
        MultiDimImage(std::vector<std::size_t> dims = {});
        template <typename U>
        void convertCopy(const MultiDimImage<U>& other);

        bool empty() const;
        std::size_t size() const;
        std::size_t dims() const;
        std::size_t dim(std::size_t d) const;
        std::size_t byteSize() const;
        Type type() const;
        void clear();
        T& at(std::vector<std::size_t> coords);
        View plane(std::vector<std::size_t> coords);
        View volume(std::vector<std::size_t> coords);
        void reorderDims(std::vector<std::size_t> dims);
        virtual ~MultiDimImage();
        Meta meta;
    protected:
        std::vector<std::size_t> m_dims;
        std::vector<std::vector<T>> m_planes;
        std::vector<std::size_t> m_planeDims;
        std::vector<std::size_t> m_restDims;
        Type m_type;

        T& unsafeAt(std::vector<std::size_t> coords);
        View subDimView(std::vector<std::size_t> coords, std::size_t firstNDims);

        struct ViewRegistry {
            ViewRegistry(MultiDimImage<T>* owner): m_owner(owner) {}
            void add(View* view)
            {
                m_views.push_back(view);
                view->m_parent = m_owner;
            }
            void remove(View* view)
            {
                auto it = std::find(m_views.begin(), m_views.end(), view);
                if (it != m_views.end()) {
                    (*it)->m_parent = nullptr;
                    m_views.erase(it);
                }
            }
            void clear()
            {
                for (auto x : m_views) {
                    x->m_parent = nullptr;
                }
                m_views.clear();
            }
            ~ViewRegistry()
            {
                clear();
            }
            std::list<View*> m_views;
            MultiDimImage<T>* m_owner;
        };
        ViewRegistry m_viewRegistry;
    };

    namespace detail {
        std::size_t flatCoordinate(const std::vector<std::size_t>& coords,
            const std::vector<std::size_t>& dims);

        bool stepCoords(std::vector<std::size_t>& coords,
            const std::vector<std::size_t>& limits);
    }

    #include "MultiDimImage.ipp"

}}

#endif
