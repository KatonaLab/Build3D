#ifndef _core_multidim_image_platform_MultiDimImage_h_
#define _core_multidim_image_platform_MultiDimImage_h_

#include <algorithm>
#include <chrono>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <typeinfo>
#include <map>
#include <utility>
#include <vector>

#include <iomanip>

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
        std::string get(const std::string& tag);
        bool has(const std::string& tag);
        void remove(const std::string& tag);
        void clear();
    private:
        std::map<std::string, std::string> m_items;
    };

    // TODO: consider using Eigen matrices instead of MultiDimImage implementation
    // or using Eigen matrices for m_planes e.g.
    template <typename T>
    class MultiDimImage {
        template <typename U> friend class MultiDimImage;
    public:
        MultiDimImage(std::vector<std::size_t> dims = {});

        MultiDimImage(const MultiDimImage&) = default;
        MultiDimImage(MultiDimImage&&) = default;
        MultiDimImage& operator=(const MultiDimImage&) = default;
        MultiDimImage& operator=(MultiDimImage&&) = default;

        template <typename U>
        void convertCopyFrom(const MultiDimImage<U>& other);
        template <typename U>
        void saturateCopyFrom(const MultiDimImage<U>& other,
            T minValue = std::numeric_limits<T>::lowest(),
            T maxValue = std::numeric_limits<T>::max());
        template <typename U>
        void scaledCopyFrom(const MultiDimImage<U>& other);

        bool empty() const;
        std::size_t size() const;
        std::size_t dims() const;
        std::size_t dim(std::size_t d) const;
        std::vector<std::size_t> dimList() const;
        std::size_t byteSize() const;
        Type type() const;
        void clear();
        T& at(std::vector<std::size_t> coords);
        std::vector<std::vector<T>>& unsafeData();
        const std::vector<std::vector<T>>& unsafeData() const;
        void reorderDims(std::vector<std::size_t> dims);
        void removeDims(std::vector<std::size_t> dims);
        std::vector<MultiDimImage<T>> splitDim(std::size_t dim);
        virtual ~MultiDimImage();

        // TODO: make getter/setter
        Meta meta;
    protected:
        std::vector<std::size_t> m_dims;
        std::vector<std::vector<T>> m_planes;
        std::vector<std::size_t> m_planeDims;
        std::vector<std::size_t> m_restDims;
        std::size_t m_planeSize;
        std::size_t m_restSize;
        std::vector<std::size_t> m_planeDimsProducts;
        std::vector<std::size_t> m_restDimsProducts;
        Type m_type;

        void initUtilsFromDim();
        T& unsafeAt(std::vector<std::size_t> coords);

        template <typename U>
        void transformCopy(const MultiDimImage<U>& other,
            std::function<T(const U&)> unary);

        std::pair<std::size_t, std::size_t> planeCoordinatePair(const std::vector<std::size_t>& coords);
    };

    namespace detail {

        // TODO: depricated, remove
        std::size_t flatCoordinate(const std::vector<std::size_t>& coords,
            const std::vector<std::size_t>& dims);

        template <typename It1, typename It2>
        void stepCoords(It1 begin, It1 end, It2 limitsBegin);

        // TODO: depricated, remove
        bool stepCoords(std::vector<std::size_t>& coords,
            const std::vector<std::size_t>& limits);

        std::vector<std::size_t> reorderCoords(std::vector<std::size_t>& coords,
            const std::vector<std::size_t>& order);

        template <typename T, typename U,
            bool T_is_integral = std::is_integral<T>::value,
            bool U_is_integral = std::is_integral<U>::value,
            bool T_is_signed = std::is_signed<T>::value,
            bool U_is_signed = std::is_signed<U>::value>
        struct SafeLessThanCompare {
            static bool less(T a, U b)
            {
                return a < b;
            }
        };

        template <typename T, typename U>
        struct SafeLessThanCompare<T, U, true, true, true, false> {
            static bool less(T a, U b)
            {
                return (a < 0) || (static_cast<U>(a) < b);
            }
        };

        template <typename T, typename U>
        struct SafeLessThanCompare<T, U, true, true, false, true> {
            static bool less(T a, U b)
            {
                return (b > 0) && (a < static_cast<T>(b));
            }
        };

        template <typename X, typename Y,
            bool X_is_integral = std::is_integral<X>::value,
            bool Y_is_integral = std::is_integral<Y>::value>
        struct TypeScaleHelper {
            static constexpr double typeScaleParamA()
            {
                return ((double)std::numeric_limits<Y>::max() - (double)std::numeric_limits<Y>::min())
                    / ((double)std::numeric_limits<X>::max() - (double)std::numeric_limits<X>::min());
            }
            static constexpr double typeScaleParamB()
            {
                return 0.5 * (
                    (double)std::numeric_limits<Y>::min()
                    + (double)std::numeric_limits<Y>::max() 
                    - TypeScaleHelper<X, Y>::typeScaleParamA() * ((double)std::numeric_limits<X>::min() + (double)std::numeric_limits<X>::max()));
            }
            static Y scale(X x)
            {
                const double a = TypeScaleHelper<X, Y>::typeScaleParamA();
                const double b = TypeScaleHelper<X, Y>::typeScaleParamB();
                return static_cast<Y>(x * a + b);
            }
        };
    }

    #include "MultiDimImage.ipp"

}}

#endif
