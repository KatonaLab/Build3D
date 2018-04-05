#include "catch.hpp"

#include <chrono>

#include <core/multidim_image_platform/MultiDimImage.hpp>
#include <iomanip>

using namespace core::multidim_image_platform;
using namespace std;

SCENARIO("indexing test", "[core/multidim_image_platform]")
{
    using namespace core::multidim_image_platform::detail;

    REQUIRE(flatCoordinate({0, 0}, {2, 1024}) == 0);
    REQUIRE(flatCoordinate({1, 0}, {2, 1024}) == 1);
    REQUIRE(flatCoordinate({0, 1}, {2, 1024}) == 2);
    REQUIRE(flatCoordinate({0, 2}, {2, 1024}) == 4);
    REQUIRE(flatCoordinate({0, 1023}, {2, 1024}) == 2 * 1023);
    REQUIRE(flatCoordinate({1, 1}, {2, 1024}) == 3);
    REQUIRE(flatCoordinate({1, 2}, {2, 1024}) == 5);
    REQUIRE(flatCoordinate({1, 1023}, {2, 1024}) == 2 * 1024 - 1);

    REQUIRE(flatCoordinate({0, 0}, {1024, 20}) == 0);
    REQUIRE(flatCoordinate({1, 0}, {1024, 20}) == 1);
    REQUIRE(flatCoordinate({2, 0}, {1024, 20}) == 2);
    REQUIRE(flatCoordinate({0, 1}, {1024, 20}) == 1024);
    REQUIRE(flatCoordinate({0, 2}, {1024, 20}) == 2 * 1024);
    REQUIRE(flatCoordinate({0, 19}, {1024, 20}) == 19 * 1024);
    REQUIRE(flatCoordinate({1, 1}, {1024, 20}) == 1025);
    REQUIRE(flatCoordinate({2, 1}, {1024, 20}) == 1026);
    REQUIRE(flatCoordinate({1023, 19}, {1024, 20}) == 20 * 1024 - 1);
}

SCENARIO("multidim image basic usage", "[core/multidim_image_platform]")
{
    GIVEN("two images, an empty and a 2d image with some content") {
        MultiDimImage<uint32_t> emptyImage;
        REQUIRE(emptyImage.size() == 0);
        REQUIRE(emptyImage.byteSize() == 0);
        REQUIRE(emptyImage.dims() == 0);
        REQUIRE_THROWS(emptyImage.dim(0));
        REQUIRE_THROWS(emptyImage.dim(23));
        REQUIRE(emptyImage.empty() == true);

        MultiDimImage<uint32_t> image2d({16, 32});
        
        REQUIRE(image2d.size() == 16 * 32);
        REQUIRE(image2d.byteSize() == 16 * 32 * 4);
        REQUIRE(image2d.dims() == 2);
        REQUIRE(image2d.dim(0) == 16);
        REQUIRE(image2d.dim(1) == 32);
        REQUIRE_THROWS(image2d.dim(2));
        REQUIRE(image2d.empty() == false);
        REQUIRE(image2d.at({0, 0}) == 0);
        image2d.at({0, 0}) = 42;
        REQUIRE(image2d.at({0, 0}) == 42);

        WHEN("the image2d is assigned to the empty one") {
            emptyImage = image2d;
            image2d.at({0, 0}) = 0;
            THEN("it is truly copied") {
                REQUIRE(emptyImage.size() == 16 * 32);
                REQUIRE(emptyImage.byteSize() == 16 * 32 * 4);
                REQUIRE(emptyImage.dims() == 2);
                REQUIRE(emptyImage.dim(0) == 16);
                REQUIRE(emptyImage.dim(1) == 32);
                REQUIRE_THROWS(emptyImage.dim(2));
                REQUIRE(emptyImage.empty() == false);
                REQUIRE(emptyImage.at({0, 0}) == 42);
            }
        }

        WHEN("a new image is copy constructed from image2d") {
            image2d.at({0, 0}) = 17;
            MultiDimImage<uint32_t> newImage(image2d);
            image2d.at({0, 0}) = 0;
            THEN("it is truly copied") {
                REQUIRE(newImage.size() == 16 * 32);
                REQUIRE(newImage.byteSize() == 16 * 32 * 4);
                REQUIRE(newImage.dims() == 2);
                REQUIRE(newImage.dim(0) == 16);
                REQUIRE(newImage.dim(1) == 32);
                REQUIRE_THROWS(newImage.dim(2));
                REQUIRE(newImage.empty() == false);
                REQUIRE(newImage.at({0, 0}) == 17);
            }
        }
    }

    GIVEN("three images") {
        MultiDimImage<uint8_t> im1({1, 1});
        MultiDimImage<uint8_t> im2({1, 1});
        MultiDimImage<float> im3({1, 1});

        REQUIRE(im1.type() == im2.type());
        REQUIRE(im1.type() != im3.type());
        REQUIRE(im2.type() != im3.type());

        WHEN("when assigned (copied)") {
            im3.convertCopyFrom(im2);
            THEN("the type matches") {
                REQUIRE(im1.type() != im3.type());
                REQUIRE(im2.type() != im3.type());
                REQUIRE(im3.type() == GetType<float>());
            }
        }
    }
}

template <typename T>
using ImageViewPair = 
    std::pair<
        std::vector<typename MultiDimImage<T>::View>,
        std::vector<typename MultiDimImage<T>::View>
    >;

template <typename T>
ImageViewPair<T> weakPointerToImageData(MultiDimImage<T>& image)
{
    std::vector<typename MultiDimImage<T>::View> planePtrs;
    std::vector<typename MultiDimImage<T>::View> volumePtrs;

    REQUIRE(image.dims() == 4);

    for (size_t c = 0; c < image.dim(3); ++c) {
        auto volume = image.volume({c});
        volumePtrs.push_back(volume);
        for (size_t d = 0; d < image.dim(2); ++d) {
            auto plane = image.plane({d, c});
            planePtrs.push_back(plane);
        }
    }

    return std::make_pair(planePtrs, volumePtrs);
}

template <typename T>
void checkWeakPtrNull(ImageViewPair<T>& wpp)
{
    for (auto wPtr : wpp.first) {
        REQUIRE(wPtr.valid() == false);
    }
    for (auto wPtr : wpp.second) {
        REQUIRE(wPtr.valid() == false);
    }
}

SCENARIO("huge multidim image usage", "[core/multidim_image_platform]")
{
    auto __start = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> __elapsed = std::chrono::high_resolution_clock::now() - __start;
    #define TIC __start = std::chrono::high_resolution_clock::now();
    #define TAC(tag) __elapsed = std::chrono::high_resolution_clock::now() - __start; \
        std::cout << tag << ": \t" << __elapsed.count() << "s" << std::endl;

    std::cout << "testing huge data create/copy/destroy" << std::endl;
    
    std::size_t n = 2048;
    GIVEN("the demand for a huge volume with 4 channels, 2048x2048x64x4 float") {

        // create
        TIC;
        MultiDimImage<float> huge({n, n, 64, 4});
        TAC("allocate 2048x2048x64x4 float data");

        ImageViewPair<float> wpp = weakPointerToImageData(huge);

        REQUIRE(huge.size() == n * n * 64 * 4);
        REQUIRE(huge.byteSize() == n * n * 64 * 4 * sizeof(float));
        REQUIRE(huge.dims() == 4);
        REQUIRE(huge.dim(0) == n);
        REQUIRE(huge.dim(1) == n);
        REQUIRE(huge.dim(2) == 64);
        REQUIRE(huge.dim(3) == 4);
        REQUIRE(huge.type() == GetType<float>());
        REQUIRE(huge.empty() == false);

        std::shared_ptr<MultiDimImage<float>> hugeCopy = std::make_shared<MultiDimImage<float>>();

        ImageViewPair<float> wppCpy;

        // copy
        TIC;
        *hugeCopy = huge;
        TAC("copy 2048x2048x64x4 float data")

        wppCpy = weakPointerToImageData(*hugeCopy);

        REQUIRE(hugeCopy->size() == n * n * 64 * 4);
        REQUIRE(hugeCopy->byteSize() == n * n * 64 * 4 * sizeof(float));
        REQUIRE(hugeCopy->dims() == 4);
        REQUIRE(hugeCopy->dim(0) == n);
        REQUIRE(hugeCopy->dim(1) == n);
        REQUIRE(hugeCopy->dim(2) == 64);
        REQUIRE(hugeCopy->dim(3) == 4);
        REQUIRE(hugeCopy->type() == GetType<float>());
        REQUIRE(hugeCopy->empty() == false);

        // reset
        TIC;
        hugeCopy.reset();
        TAC("free 2048x2048x64x4 float data")

        checkWeakPtrNull<float>(wppCpy);

        // clear
        TIC;
        huge.clear();
        TAC("clear 2048x2048x64x4 float data")

        checkWeakPtrNull<float>(wpp);
    }

    std::cout << "testing huge data create/copy/destroy finished" << std::endl;

    #undef TIC
    #undef TAC
}

SCENARIO("multidim metadata usage", "[core/multidim_image_platform]")
{
    GIVEN("an image") {
        MultiDimImage<uint8_t> im;
        WHEN("meta is added") {
            im.meta.add("int_meta_test", "42");
            im.meta.add("float_meta_test", "2024.2024");
            im.meta.add("string_meta_test", "testing 1, 2, 3...");
            im.meta.add("to be removed", "0");
            THEN("it is stored correctly") {
                REQUIRE(im.meta.get("int_meta_test") == "42");
                REQUIRE(im.meta.get("float_meta_test") == "2024.2024");
                REQUIRE(im.meta.get("string_meta_test") == "testing 1, 2, 3...");
                REQUIRE(im.meta.get("to be removed") == "0");
            }
            
            AND_WHEN("one is removed") {
                im.meta.remove("to be removed");
                THEN("it is no longer reachable") {
                    REQUIRE(im.meta.has("to be removed") == false);
                    REQUIRE_THROWS(im.meta.get("to be removed"));
                }
            }
        }
    }
}

SCENARIO("multidim subdata from multidim", "[core/multidim_image_platform]")
{
    GIVEN("an image") {
        MultiDimImage<uint8_t> image({16, 16, 4, 2});
        image.at({0, 8, 0, 0}) = 42;
        image.at({1, 9, 1, 0}) = 43;
        image.at({2, 10, 2, 0}) = 44;
        image.at({3, 11, 3, 0}) = 45;
        image.at({4, 12, 0, 1}) = 82;
        image.at({5, 13, 1, 1}) = 83;
        image.at({6, 14, 2, 1}) = 84;
        image.at({7, 15, 3, 1}) = 85;
        WHEN("a plane is viewed through a MultiDimImageView") {
            auto view1 = image.plane({0, 0});
            auto view2 = image.plane({1, 0});
            auto view3 = image.plane({2, 0});
            auto view4 = image.plane({3, 0});
            auto view5 = image.plane({0, 1});
            auto view6 = image.plane({1, 1});
            auto view7 = image.plane({2, 1});
            auto view8 = image.plane({3, 1});

            REQUIRE(view1.dims() == 2);
            REQUIRE(view1.dim(0) == 16);
            REQUIRE(view1.dim(1) == 16);
            REQUIRE(view1.size() == 16 * 16);
            REQUIRE(view1.empty() == false);
            REQUIRE(view1.valid() == true);
            REQUIRE(view1.parent() == &image);

            REQUIRE(view7.dims() == 2);
            REQUIRE(view7.dim(0) == 16);
            REQUIRE(view7.dim(1) == 16);
            REQUIRE(view7.size() == 16 * 16);
            REQUIRE(view7.empty() == false);
            REQUIRE(view7.valid() == true);
            REQUIRE(view7.parent() == &image);

            THEN("it points to the right data") {
                REQUIRE(view1.at({0, 8}) == 42);
                REQUIRE(view2.at({1, 9}) == 43);
                REQUIRE(view3.at({2, 10}) == 44);
                REQUIRE(view4.at({3, 11}) == 45);
                REQUIRE(view5.at({4, 12}) == 82);
                REQUIRE(view6.at({5, 13}) == 83);
                REQUIRE(view7.at({6, 14}) == 84);
                REQUIRE(view8.at({7, 15}) == 85);
            }

            AND_WHEN("a view id modified") {
                view1.at({0, 0}) = 224;
                view7.at({0, 0}) = 225;
                THEN("then the original MultiDimImage is modified too") {
                    REQUIRE(image.at({0, 0, 0, 0}) == 224);
                    REQUIRE(image.at({0, 0, 2, 1}) == 225);
                }
            }
        }

        WHEN("a volume is viewed through a MultiDimImageView") {
            MultiDimImage<uint8_t>::View vol1 = image.volume({0});
            MultiDimImage<uint8_t>::View vol2 = image.volume({1});
            THEN("the dimensions match") {
                REQUIRE(vol1.dims() == 3);
                REQUIRE(vol1.dim(0) == 16);
                REQUIRE(vol1.dim(1) == 16);
                REQUIRE(vol1.dim(2) == 4);
                REQUIRE(vol1.empty() == false);
                REQUIRE(vol1.size() == 16 * 16 * 4);
                REQUIRE(vol1.valid() == true);
                REQUIRE(vol1.parent() == &image);

                REQUIRE(vol2.dims() == 3);
                REQUIRE(vol2.dim(0) == 16);
                REQUIRE(vol2.dim(1) == 16);
                REQUIRE(vol2.dim(2) == 4);
                REQUIRE(vol2.empty() == false);
                REQUIRE(vol2.size() == 16 * 16 * 4);
                REQUIRE(vol2.valid() == true);
                REQUIRE(vol2.parent() == &image);
            }

            AND_WHEN("view is modified") {
                vol1.at({15, 15, 0}) = 127;
                vol2.at({15, 15, 0}) = 78;
                THEN("the original is modified too") {
                    REQUIRE(image.at({15, 15, 0, 0}) == 127);
                    REQUIRE(image.at({15, 15, 0, 1}) == 78);
                }
            }
        }
    }
}

SCENARIO("multidim dimension reordering", "[core/multidim_image_platform]")
{
    size_t n = 100;
    GIVEN("an image") {
        MultiDimImage<uint8_t> image({n, n, 16, 3});
        image.at({n-1, 0, 0, 1}) = 42;
        image.at({78, 96, 7, 1}) = 47;
        WHEN("first two dimensions reordered") {
            image.reorderDims({1, 0, 2, 3});
            THEN("the data is reordered") {
                REQUIRE(image.size() == n * n * 16 * 3);
                REQUIRE(image.dims() == 4);
                REQUIRE(image.dim(0) == n);
                REQUIRE(image.dim(1) == n);
                REQUIRE(image.dim(2) == 16);
                REQUIRE(image.dim(3) == 3);
                REQUIRE(image.at({n-1, 0, 0, 1}) == 0);
                REQUIRE(image.at({78, 96, 7, 1}) == 0);
                REQUIRE(image.at({0, n-1, 0, 1}) == 42);
                REQUIRE(image.at({96, 78, 7, 1}) == 47);
            }
        }

        WHEN("the reorder parametrs are bad") {
            THEN("the funciton throws") {
                REQUIRE_THROWS(image.reorderDims({1, 0}));
                REQUIRE_THROWS(image.reorderDims({0, 1, 2, 3, 4}));
                REQUIRE_THROWS(image.reorderDims({0, 1, 2, 4}));
                REQUIRE_THROWS(image.reorderDims({}));
            }
        }

        WHEN("complex reordeing happens #1") {
            image.reorderDims({0, 1, 3, 2});
            THEN("the data is reordered") {
                REQUIRE(image.size() == n * n * 16 * 3);
                REQUIRE(image.dims() == 4);
                REQUIRE(image.dim(0) == n);
                REQUIRE(image.dim(1) == n);
                REQUIRE(image.dim(2) == 3);
                REQUIRE(image.dim(3) == 16);
                REQUIRE(image.at({n-1, 0, 0, 1}) == 0);
                REQUIRE_THROWS(image.at({78, 96, 7, 1}));
                REQUIRE(image.at({n-1, 0, 1, 0}) == 42);
                REQUIRE(image.at({78, 96, 1, 7}) == 47);
            }
        }

        WHEN("complex reordeing happens #2") {
            image.reorderDims({3, 0, 1, 2});
            THEN("the data is reordered") {
                REQUIRE(image.size() == n * n * 16 * 3);
                REQUIRE(image.dims() == 4);
                REQUIRE(image.dim(0) == 3);
                REQUIRE(image.dim(1) == n);
                REQUIRE(image.dim(2) == n);
                REQUIRE(image.dim(3) == 16);
                REQUIRE_THROWS(image.at({n-1, 0, 0, 1}));
                REQUIRE_THROWS(image.at({78, 96, 7, 1}));
                REQUIRE(image.at({1, n-1, 0, 0}) == 42);
                REQUIRE(image.at({1, 78, 96, 7}) == 47);
            }
        }

        WHEN("the reordering is the same") {
            image.reorderDims({0, 1, 2, 3});
            THEN("the data is the same") {
                REQUIRE(image.size() == n * n * 16 * 3);
                REQUIRE(image.dims() == 4);
                REQUIRE(image.dim(0) == n);
                REQUIRE(image.dim(1) == n);
                REQUIRE(image.dim(2) == 16);
                REQUIRE(image.dim(3) == 3);
                REQUIRE(image.at({n-1, 0, 0, 1}) == 42);
                REQUIRE(image.at({78, 96, 7, 1}) == 47);
            }
        }
    }
}

SCENARIO("multidim with 1d arrays", "[core/multidim_image_platform]")
{
    GIVEN("a 1d image") {
        MultiDimImage<std::uint64_t> image({128});
        REQUIRE(image.size() == 128);
        REQUIRE(image.byteSize() == 128 * 8);
        REQUIRE(image.dims() == 1);
        REQUIRE(image.dim(0) == 128);
        WHEN("writing a pixel") {
            image.at({17}) = 42;
            THEN("it is written") {
                REQUIRE(image.at({17}) == 42);
            }

            AND_WHEN("it is copied constructed") {
                MultiDimImage<std::uint64_t> imageCopy(image);
                THEN("it is copied right") {
                    REQUIRE(imageCopy.size() == 128);
                    REQUIRE(imageCopy.byteSize() == 128 * 8);
                    REQUIRE(imageCopy.dims() == 1);
                    REQUIRE(imageCopy.dim(0) == 128);
                    REQUIRE(imageCopy.at({17}) == 42);
                }
            }

            AND_WHEN("it is copy assigned") {
                MultiDimImage<std::uint64_t> imageCopy;
                imageCopy = image;
                THEN("it is copied right") {
                    REQUIRE(imageCopy.size() == 128);
                    REQUIRE(imageCopy.byteSize() == 128 * 8);
                    REQUIRE(imageCopy.dims() == 1);
                    REQUIRE(imageCopy.dim(0) == 128);
                    REQUIRE(imageCopy.at({17}) == 42);
                }
            }
        }

        WHEN("trying to view through a plane view") {
            THEN("it is prohibited") {
                REQUIRE_THROWS(image.plane({}));
                REQUIRE_THROWS(image.plane({0}));
                REQUIRE_THROWS(image.plane({0, 0}));
            }
        }

        WHEN("trying to view through a volume view") {
            THEN("it is prohibited") {
                REQUIRE_THROWS(image.volume({}));
                REQUIRE_THROWS(image.volume({0}));
                REQUIRE_THROWS(image.volume({0, 0}));
                REQUIRE_THROWS(image.volume({0, 0, 0}));
            }
        }
    }
}

SCENARIO("converting between types", "[core/multidim_image_platform]")
{
    GIVEN("different types of images") {
        MultiDimImage<std::int8_t> im8s({16, 24});
        MultiDimImage<std::uint64_t> im64u({24, 32});
        MultiDimImage<float> imf({64, 32});
        MultiDimImage<double> imd({32, 64});

        im8s.at({7, 9}) = -2;
        im8s.at({7, 10}) = 127;
        im8s.at({7, 11}) = 92;
        im8s.at({7, 12}) = 0;

        im64u.at({17, 13}) = 42000;
        im64u.at({17, 14}) = 0;
        im64u.at({17, 15}) = 18446744073709551615ULL;
        im64u.at({17, 16}) = 78;

        imf.at({17, 23}) = 0.0f;
        imf.at({17, 24}) = 42.0f;
        imf.at({17, 25}) = 42.4242f;
        imf.at({17, 26}) = 1042.4242f;

        imd.at({11, 47}) = 2024.0;
        imd.at({11, 48}) = 0.0;
        imd.at({11, 49}) = 2024.202420242024;
        imd.at({11, 50}) = 42.0;

        WHEN("copy converting from im8s to im64u") {
            im64u.convertCopyFrom(im8s);
            THEN("it is correct") {
                REQUIRE(im64u.size() == 16 * 24);
                REQUIRE(im64u.byteSize() == 16 * 24 * sizeof(std::uint64_t));
                REQUIRE(im64u.dims() == 2);
                REQUIRE(im64u.dim(0) == 16);
                REQUIRE(im64u.dim(1) == 24);
                REQUIRE(im64u.at({7, 10}) == 127);
                REQUIRE(im64u.at({7, 11}) == 92);
                REQUIRE(im64u.at({7, 12}) == 0);
            }
        }

        WHEN("copy converting from im64u to im8s") {
            im8s.convertCopyFrom(im64u);
            THEN("it is correct") {
                REQUIRE(im8s.size() == 24 * 32);
                REQUIRE(im8s.byteSize() == 24 * 32 * sizeof(std::int8_t));
                REQUIRE(im8s.dims() == 2);
                REQUIRE(im8s.dim(0) == 24);
                REQUIRE(im8s.dim(1) == 32);
                REQUIRE(im8s.at({17, 14}) == 0);
                REQUIRE(im8s.at({17, 16}) == 78);
            }
        }

        WHEN("copy converting from im8s to imf") {
            imf.convertCopyFrom(im8s);
            THEN("it is correct") {
                REQUIRE(imf.size() == 16 * 24);
                REQUIRE(imf.byteSize() == 16 * 24 * sizeof(float));
                REQUIRE(imf.dims() == 2);
                REQUIRE(imf.dim(0) == 16);
                REQUIRE(imf.dim(1) == 24);
                REQUIRE(imf.at({7, 9}) == -2.f);
                REQUIRE(imf.at({7, 10}) == 127.f);
                REQUIRE(imf.at({7, 11}) == 92.f);
                REQUIRE(imf.at({7, 12}) == 0.f);
            }
        }

        WHEN("copy converting from imf to im8s") {
            im8s.convertCopyFrom(imf);
            THEN("it is correct") {
                REQUIRE(im8s.size() == 64 * 32);
                REQUIRE(im8s.byteSize() == 64 * 32 * sizeof(std::int8_t));
                REQUIRE(im8s.dims() == 2);
                REQUIRE(im8s.dim(0) == 64);
                REQUIRE(im8s.dim(1) == 32);
                REQUIRE(im8s.at({17, 23}) == 0);
                REQUIRE(im8s.at({17, 24}) == 42);
                REQUIRE(im8s.at({17, 25}) == 42);
            }
        }

        WHEN("copy converting from imd to im8s") {
            im8s.convertCopyFrom(imd);
            THEN("it is correct") {
                REQUIRE(im8s.size() == 64 * 32);
                REQUIRE(im8s.byteSize() == 64 * 32 * sizeof(std::int8_t));
                REQUIRE(im8s.dims() == 2);
                REQUIRE(im8s.dim(0) == 32);
                REQUIRE(im8s.dim(1) == 64);
                REQUIRE(im8s.at({11, 48}) == 0);
                REQUIRE(im8s.at({11, 50}) == 42);
            }
        }

        
    }
}

SCENARIO("safe less compare test", "[core/multidim_image_platform]")
{
    using namespace core::multidim_image_platform::detail;
    REQUIRE(SafeLessThanCompare<char, char>::less(2, 4) == true);
    REQUIRE(SafeLessThanCompare<char, char>::less(4, 2) == false);
    REQUIRE(SafeLessThanCompare<char, char>::less(-8, -3) == true);
    REQUIRE(SafeLessThanCompare<char, char>::less(-3, -8) == false);
    REQUIRE(SafeLessThanCompare<char, char>::less(-3, -3) == false);
    REQUIRE(SafeLessThanCompare<unsigned char, char>::less(2, -2) == false);
    REQUIRE(SafeLessThanCompare<unsigned char, char>::less(2, 4) == true);
    REQUIRE(SafeLessThanCompare<unsigned char, char>::less(2, 2) == false);
    REQUIRE(SafeLessThanCompare<char, unsigned char>::less(-2, 2) == true);
    REQUIRE(SafeLessThanCompare<char, unsigned char>::less(2, 4) == true);
    REQUIRE(SafeLessThanCompare<char, unsigned char>::less(4, 2) == false);

    REQUIRE(SafeLessThanCompare<char, int>::less(2, 400) == true);
    REQUIRE(SafeLessThanCompare<char, int>::less(4, 2) == false);
    REQUIRE(SafeLessThanCompare<char, int>::less(-7, -5) == true);
    REQUIRE(SafeLessThanCompare<int, char>::less(-300, -5) == true);
    REQUIRE(SafeLessThanCompare<unsigned char, int>::less(2, -400) == false);
    REQUIRE(SafeLessThanCompare<unsigned char, int>::less(2, 400) == true);
    REQUIRE(SafeLessThanCompare<char, unsigned int>::less(-2, 400) == true);
    REQUIRE(SafeLessThanCompare<char, unsigned int>::less(2, 400) == true);
    REQUIRE(SafeLessThanCompare<char, unsigned int>::less(4, 2) == false);

    REQUIRE(SafeLessThanCompare<long, double>::less(2203, 4000.324) == true);
    REQUIRE(SafeLessThanCompare<long, double>::less(23412, 23411.9) == false);
    REQUIRE(SafeLessThanCompare<long, double>::less(23412, 23412.0000001) == true);
    REQUIRE(SafeLessThanCompare<long, double>::less(-1354, 23411.9) == true);
    REQUIRE(SafeLessThanCompare<unsigned long, double>::less(2, -400) == false);
    REQUIRE(SafeLessThanCompare<unsigned long, double>::less(2, 400) == true);
    REQUIRE(SafeLessThanCompare<long, double>::less(-2, 400) == true);
    REQUIRE(SafeLessThanCompare<long, double>::less(2, 400) == true);
    REQUIRE(SafeLessThanCompare<long, double>::less(4, 2) == false);
    REQUIRE(SafeLessThanCompare<double, double>::less(3.141519, 3.141519) == false);
}

SCENARIO("converting between types with saturation", "[core/multidim_image_platform]")
{
    GIVEN("different types of images") {
        MultiDimImage<std::int8_t> im8s({16, 24});
        MultiDimImage<std::uint64_t> im64u({24, 32});
        MultiDimImage<float> imf({64, 32});
        MultiDimImage<double> imd({32, 64});

        im8s.at({7, 9}) = -2;
        im8s.at({7, 10}) = 127;
        im8s.at({7, 11}) = 92;
        im8s.at({7, 12}) = 0;

        im64u.at({17, 13}) = 42000;
        im64u.at({17, 14}) = 0;
        im64u.at({17, 15}) = 18446744073709551615ULL;
        im64u.at({17, 16}) = 78;

        imf.at({17, 23}) = 0.0f;
        imf.at({17, 24}) = 42.0f;
        imf.at({17, 25}) = 42.4242f;
        imf.at({17, 26}) = 1042.4242f;

        imd.at({11, 47}) = 2024.0;
        imd.at({11, 48}) = -700.0;
        imd.at({11, 49}) = 2024.202420242024;
        imd.at({11, 50}) = 42.0;

        WHEN("copy saturate converting from im8s to im64u") {
            im64u.saturateCopyFrom(im8s);
            THEN("it is correct") {
                REQUIRE(im64u.size() == 16 * 24);
                REQUIRE(im64u.byteSize() == 16 * 24 * sizeof(std::uint64_t));
                REQUIRE(im64u.dims() == 2);
                REQUIRE(im64u.dim(0) == 16);
                REQUIRE(im64u.dim(1) == 24);

                REQUIRE(im64u.at({7, 9}) == 0);
                REQUIRE(im64u.at({7, 10}) == 127);
                REQUIRE(im64u.at({7, 11}) == 92);
                REQUIRE(im64u.at({7, 12}) == 0);
            }
        }

        WHEN("copy saturate converting from im64u to im8s") {
            im8s.saturateCopyFrom(im64u);
            THEN("it is correct") {
                REQUIRE(im8s.size() == 24 * 32);
                REQUIRE(im8s.byteSize() == 24 * 32 * sizeof(std::int8_t));
                REQUIRE(im8s.dims() == 2);
                REQUIRE(im8s.dim(0) == 24);
                REQUIRE(im8s.dim(1) == 32);
                REQUIRE(im8s.at({17, 13}) == 127);
                REQUIRE(im8s.at({17, 14}) == 0);
                REQUIRE(im8s.at({17, 15}) == 127);
                REQUIRE(im8s.at({17, 16}) == 78);
            }
        }

        WHEN("copy saturate converting from im8s to imf") {
            imf.saturateCopyFrom(im8s);
            THEN("it is correct") {
                REQUIRE(imf.size() == 16 * 24);
                REQUIRE(imf.byteSize() == 16 * 24 * sizeof(float));
                REQUIRE(imf.dims() == 2);
                REQUIRE(imf.dim(0) == 16);
                REQUIRE(imf.dim(1) == 24);

                REQUIRE(imf.at({7, 9}) == -2.f);
                REQUIRE(imf.at({7, 10}) == 127.f);
                REQUIRE(imf.at({7, 11}) == 92.f);
                REQUIRE(imf.at({7, 12}) == 0.f);
            }
        }

        WHEN("copy saturate converting from imf to im8s") {
            im8s.saturateCopyFrom(imf);
            THEN("it is correct") {
                REQUIRE(im8s.size() == 64 * 32);
                REQUIRE(im8s.byteSize() == 64 * 32 * sizeof(std::int8_t));
                REQUIRE(im8s.dims() == 2);
                REQUIRE(im8s.dim(0) == 64);
                REQUIRE(im8s.dim(1) == 32);

                REQUIRE(im8s.at({17, 23}) == 0);
                REQUIRE(im8s.at({17, 24}) == 42);
                REQUIRE(im8s.at({17, 25}) == 42);
                REQUIRE(im8s.at({17, 26}) == 127);
            }
        }

        WHEN("copy saturate converting from imd to im8s") {
            im8s.saturateCopyFrom(imd);
            THEN("it is correct") {
                REQUIRE(im8s.size() == 64 * 32);
                REQUIRE(im8s.byteSize() == 64 * 32 * sizeof(std::int8_t));
                REQUIRE(im8s.dims() == 2);
                REQUIRE(im8s.dim(0) == 32);
                REQUIRE(im8s.dim(1) == 64);

                REQUIRE(im8s.at({11, 47}) == 127);
                REQUIRE(im8s.at({11, 48}) == -128);
                REQUIRE(im8s.at({11, 49}) == 127);
                REQUIRE(im8s.at({11, 50}) == 42);
            }
        }
    }
}

SCENARIO("numeric type scaler test", "[core/multidim_image_platform]")
{
    using namespace core::multidim_image_platform::detail;

    REQUIRE(TypeScaleHelper<double, double>::scale(0.200000000000000) == Approx(0.200000000000000));
    REQUIRE(TypeScaleHelper<double, double>::scale(0.500000000000000) == Approx(0.500000000000000));
    REQUIRE(TypeScaleHelper<double, double>::scale(0.700000000000000) == Approx(0.700000000000000));
    REQUIRE(TypeScaleHelper<double, float>::scale(0.200000000000000) == Approx(0.200000000000000));
    REQUIRE(TypeScaleHelper<double, float>::scale(0.500000000000000) == Approx(0.500000000000000));
    REQUIRE(TypeScaleHelper<double, float>::scale(0.700000000000000) == Approx(0.700000000000000));
    REQUIRE(TypeScaleHelper<float, double>::scale(0.200000000000000) == Approx(0.200000000000000));
    REQUIRE(TypeScaleHelper<float, double>::scale(0.500000000000000) == Approx(0.500000000000000));
    REQUIRE(TypeScaleHelper<float, double>::scale(0.700000000000000) == Approx(0.700000000000000));
    REQUIRE(TypeScaleHelper<float, float>::scale(0.200000000000000) == Approx(0.200000000000000));
    REQUIRE(TypeScaleHelper<float, float>::scale(0.500000000000000) == Approx(0.500000000000000));
    REQUIRE(TypeScaleHelper<float, float>::scale(0.700000000000000) == Approx(0.700000000000000));
    REQUIRE(TypeScaleHelper<int16_t, double>::scale(-10873) == Approx(0.334096284428168));
    REQUIRE(TypeScaleHelper<int16_t, double>::scale(-190) == Approx(0.497108415350576));
    REQUIRE(TypeScaleHelper<int16_t, double>::scale(-42) == Approx(0.499366750591287));
    REQUIRE(TypeScaleHelper<int16_t, double>::scale(0) == Approx(0.500007629510948));
    REQUIRE(TypeScaleHelper<int16_t, double>::scale(255) == Approx(0.503898680094606));
    REQUIRE(TypeScaleHelper<int16_t, double>::scale(27) == Approx(0.500419623102159));
    REQUIRE(TypeScaleHelper<int16_t, double>::scale(42) == Approx(0.500648508430610));
    REQUIRE(TypeScaleHelper<int16_t, float>::scale(-10873) == Approx(0.334096284428168));
    REQUIRE(TypeScaleHelper<int16_t, float>::scale(-190) == Approx(0.497108415350576));
    REQUIRE(TypeScaleHelper<int16_t, float>::scale(-42) == Approx(0.499366750591287));
    REQUIRE(TypeScaleHelper<int16_t, float>::scale(0) == Approx(0.500007629510948));
    REQUIRE(TypeScaleHelper<int16_t, float>::scale(255) == Approx(0.503898680094606));
    REQUIRE(TypeScaleHelper<int16_t, float>::scale(27) == Approx(0.500419623102159));
    REQUIRE(TypeScaleHelper<int16_t, float>::scale(42) == Approx(0.500648508430610));
    REQUIRE(TypeScaleHelper<int16_t, int16_t>::scale(-10873) == -10873);
    REQUIRE(TypeScaleHelper<int16_t, int16_t>::scale(-190) == -190);
    REQUIRE(TypeScaleHelper<int16_t, int16_t>::scale(-42) == -42);
    REQUIRE(TypeScaleHelper<int16_t, int16_t>::scale(0) == 0);
    REQUIRE(TypeScaleHelper<int16_t, int16_t>::scale(255) == 255);
    REQUIRE(TypeScaleHelper<int16_t, int16_t>::scale(27) == 27);
    REQUIRE(TypeScaleHelper<int16_t, int16_t>::scale(42) == 42);
    REQUIRE(TypeScaleHelper<int16_t, int32_t>::scale(-10873) == -712551033);
    REQUIRE(TypeScaleHelper<int16_t, int32_t>::scale(-190) == -12419262);
    REQUIRE(TypeScaleHelper<int16_t, int32_t>::scale(-42) == -2719786);
    REQUIRE(TypeScaleHelper<int16_t, int32_t>::scale(0) == 32768);
    REQUIRE(TypeScaleHelper<int16_t, int32_t>::scale(255) == 16744703);
    REQUIRE(TypeScaleHelper<int16_t, int32_t>::scale(27) == 1802267);
    REQUIRE(TypeScaleHelper<int16_t, int32_t>::scale(42) == 2785322);
    REQUIRE(TypeScaleHelper<int16_t, int8_t>::scale(-10873) == -42);
    REQUIRE(TypeScaleHelper<int16_t, int8_t>::scale(-190) == -1);
    REQUIRE(TypeScaleHelper<int16_t, int8_t>::scale(-42) == 0);
    REQUIRE(TypeScaleHelper<int16_t, int8_t>::scale(0) == 0);
    REQUIRE(TypeScaleHelper<int16_t, int8_t>::scale(255) == 0);
    REQUIRE(TypeScaleHelper<int16_t, int8_t>::scale(27) == 0);
    REQUIRE(TypeScaleHelper<int16_t, int8_t>::scale(42) == 0);
    REQUIRE(TypeScaleHelper<int16_t, uint16_t>::scale(-10873) == 21895);
    REQUIRE(TypeScaleHelper<int16_t, uint16_t>::scale(-190) == 32578);
    REQUIRE(TypeScaleHelper<int16_t, uint16_t>::scale(-42) == 32726);
    REQUIRE(TypeScaleHelper<int16_t, uint16_t>::scale(0) == 32768);
    REQUIRE(TypeScaleHelper<int16_t, uint16_t>::scale(255) == 33023);
    REQUIRE(TypeScaleHelper<int16_t, uint16_t>::scale(27) == 32795);
    REQUIRE(TypeScaleHelper<int16_t, uint16_t>::scale(42) == 32810);
    REQUIRE(TypeScaleHelper<int16_t, uint32_t>::scale(-10873) == 1434932615);
    REQUIRE(TypeScaleHelper<int16_t, uint32_t>::scale(-190) == 2135064386);
    REQUIRE(TypeScaleHelper<int16_t, uint32_t>::scale(-42) == 2144763862);
    REQUIRE(TypeScaleHelper<int16_t, uint32_t>::scale(0) == 2147516416);
    REQUIRE(TypeScaleHelper<int16_t, uint32_t>::scale(255) == 2164228351);
    REQUIRE(TypeScaleHelper<int16_t, uint32_t>::scale(27) == 2149285915);
    REQUIRE(TypeScaleHelper<int16_t, uint32_t>::scale(42) == 2150268970);
    REQUIRE(TypeScaleHelper<int16_t, uint8_t>::scale(-10873) == 85);
    REQUIRE(TypeScaleHelper<int16_t, uint8_t>::scale(-190) == 126);
    REQUIRE(TypeScaleHelper<int16_t, uint8_t>::scale(-42) == 127);
    REQUIRE(TypeScaleHelper<int16_t, uint8_t>::scale(0) == 127);
    REQUIRE(TypeScaleHelper<int16_t, uint8_t>::scale(255) == 128);
    REQUIRE(TypeScaleHelper<int16_t, uint8_t>::scale(27) == 127);
    REQUIRE(TypeScaleHelper<int16_t, uint8_t>::scale(42) == 127);
    REQUIRE(TypeScaleHelper<int32_t, double>::scale(-10873) == Approx(0.499997468548826));
    REQUIRE(TypeScaleHelper<int32_t, double>::scale(-190) == Approx(0.499999955878593));
    REQUIRE(TypeScaleHelper<int32_t, double>::scale(-42) == Approx(0.499999990337528));
    REQUIRE(TypeScaleHelper<int32_t, double>::scale(0) == Approx(0.500000000116415));
    REQUIRE(TypeScaleHelper<int32_t, double>::scale(255) == Approx(0.500000059488229));
    REQUIRE(TypeScaleHelper<int32_t, double>::scale(27) == Approx(0.500000006402843));
    REQUIRE(TypeScaleHelper<int32_t, double>::scale(32980) == Approx(0.500007678871045));
    REQUIRE(TypeScaleHelper<int32_t, double>::scale(42) == Approx(0.500000009895302));
    REQUIRE(TypeScaleHelper<int32_t, float>::scale(-10873) == Approx(0.499997468548826));
    REQUIRE(TypeScaleHelper<int32_t, float>::scale(-190) == Approx(0.499999955878593));
    REQUIRE(TypeScaleHelper<int32_t, float>::scale(-42) == Approx(0.499999990337528));
    REQUIRE(TypeScaleHelper<int32_t, float>::scale(0) == Approx(0.500000000116415));
    REQUIRE(TypeScaleHelper<int32_t, float>::scale(255) == Approx(0.500000059488229));
    REQUIRE(TypeScaleHelper<int32_t, float>::scale(27) == Approx(0.500000006402843));
    REQUIRE(TypeScaleHelper<int32_t, float>::scale(32980) == Approx(0.500007678871045));
    REQUIRE(TypeScaleHelper<int32_t, float>::scale(42) == Approx(0.500000009895302));
    REQUIRE(TypeScaleHelper<int32_t, int16_t>::scale(-10873) == 0);
    REQUIRE(TypeScaleHelper<int32_t, int16_t>::scale(-190) == 0);
    REQUIRE(TypeScaleHelper<int32_t, int16_t>::scale(-42) == 0);
    REQUIRE(TypeScaleHelper<int32_t, int16_t>::scale(0) == 0);
    REQUIRE(TypeScaleHelper<int32_t, int16_t>::scale(255) == 0);
    REQUIRE(TypeScaleHelper<int32_t, int16_t>::scale(27) == 0);
    REQUIRE(TypeScaleHelper<int32_t, int16_t>::scale(32980) == 0);
    REQUIRE(TypeScaleHelper<int32_t, int16_t>::scale(42) == 0);
    REQUIRE(TypeScaleHelper<int32_t, int32_t>::scale(-10873) == -10873);
    REQUIRE(TypeScaleHelper<int32_t, int32_t>::scale(-190) == -190);
    REQUIRE(TypeScaleHelper<int32_t, int32_t>::scale(-42) == -42);
    REQUIRE(TypeScaleHelper<int32_t, int32_t>::scale(0) == 0);
    REQUIRE(TypeScaleHelper<int32_t, int32_t>::scale(255) == 255);
    REQUIRE(TypeScaleHelper<int32_t, int32_t>::scale(27) == 27);
    REQUIRE(TypeScaleHelper<int32_t, int32_t>::scale(32980) == 32980);
    REQUIRE(TypeScaleHelper<int32_t, int32_t>::scale(42) == 42);
    REQUIRE(TypeScaleHelper<int32_t, int8_t>::scale(-10873) == 0);
    REQUIRE(TypeScaleHelper<int32_t, int8_t>::scale(-190) == 0);
    REQUIRE(TypeScaleHelper<int32_t, int8_t>::scale(-42) == 0);
    REQUIRE(TypeScaleHelper<int32_t, int8_t>::scale(0) == 0);
    REQUIRE(TypeScaleHelper<int32_t, int8_t>::scale(255) == 0);
    REQUIRE(TypeScaleHelper<int32_t, int8_t>::scale(27) == 0);
    REQUIRE(TypeScaleHelper<int32_t, int8_t>::scale(32980) == 0);
    REQUIRE(TypeScaleHelper<int32_t, int8_t>::scale(42) == 0);
    REQUIRE(TypeScaleHelper<int32_t, uint16_t>::scale(-10873) == 32767);
    REQUIRE(TypeScaleHelper<int32_t, uint16_t>::scale(-190) == 32767);
    REQUIRE(TypeScaleHelper<int32_t, uint16_t>::scale(-42) == 32767);
    REQUIRE(TypeScaleHelper<int32_t, uint16_t>::scale(0) == 32767);
    REQUIRE(TypeScaleHelper<int32_t, uint16_t>::scale(255) == 32767);
    REQUIRE(TypeScaleHelper<int32_t, uint16_t>::scale(27) == 32767);
    REQUIRE(TypeScaleHelper<int32_t, uint16_t>::scale(32980) == 32768);
    REQUIRE(TypeScaleHelper<int32_t, uint16_t>::scale(42) == 32767);
    REQUIRE(TypeScaleHelper<int32_t, uint32_t>::scale(-10873) == 2147472775);
    REQUIRE(TypeScaleHelper<int32_t, uint32_t>::scale(-190) == 2147483458);
    REQUIRE(TypeScaleHelper<int32_t, uint32_t>::scale(-42) == 2147483606);
    REQUIRE(TypeScaleHelper<int32_t, uint32_t>::scale(0) == 2147483648);
    REQUIRE(TypeScaleHelper<int32_t, uint32_t>::scale(255) == 2147483903);
    REQUIRE(TypeScaleHelper<int32_t, uint32_t>::scale(27) == 2147483675);
    REQUIRE(TypeScaleHelper<int32_t, uint32_t>::scale(32980) == 2147516628);
    REQUIRE(TypeScaleHelper<int32_t, uint32_t>::scale(42) == 2147483690);
    REQUIRE(TypeScaleHelper<int32_t, uint8_t>::scale(-10873) == 127);
    REQUIRE(TypeScaleHelper<int32_t, uint8_t>::scale(-190) == 127);
    REQUIRE(TypeScaleHelper<int32_t, uint8_t>::scale(-42) == 127);
    REQUIRE(TypeScaleHelper<int32_t, uint8_t>::scale(0) == 127);
    REQUIRE(TypeScaleHelper<int32_t, uint8_t>::scale(255) == 127);
    REQUIRE(TypeScaleHelper<int32_t, uint8_t>::scale(27) == 127);
    REQUIRE(TypeScaleHelper<int32_t, uint8_t>::scale(32980) == 127);
    REQUIRE(TypeScaleHelper<int32_t, uint8_t>::scale(42) == 127);
    REQUIRE(TypeScaleHelper<int8_t, double>::scale(-42) == Approx(0.337254901960784));
    REQUIRE(TypeScaleHelper<int8_t, double>::scale(0) == Approx(0.501960784313725));
    REQUIRE(TypeScaleHelper<int8_t, double>::scale(27) == Approx(0.607843137254902));
    REQUIRE(TypeScaleHelper<int8_t, double>::scale(42) == Approx(0.666666666666667));
    REQUIRE(TypeScaleHelper<int8_t, float>::scale(-42) == Approx(0.337254901960784));
    REQUIRE(TypeScaleHelper<int8_t, float>::scale(0) == Approx(0.501960784313725));
    REQUIRE(TypeScaleHelper<int8_t, float>::scale(27) == Approx(0.607843137254902));
    REQUIRE(TypeScaleHelper<int8_t, float>::scale(42) == Approx(0.666666666666667));
    REQUIRE(TypeScaleHelper<int8_t, int16_t>::scale(-42) == -10666);
    REQUIRE(TypeScaleHelper<int8_t, int16_t>::scale(0) == 128);
    REQUIRE(TypeScaleHelper<int8_t, int16_t>::scale(27) == 7067);
    REQUIRE(TypeScaleHelper<int8_t, int16_t>::scale(42) == 10922);
    REQUIRE(TypeScaleHelper<int8_t, int32_t>::scale(-42) == -698984874);
    REQUIRE(TypeScaleHelper<int8_t, int32_t>::scale(0) == 8421504);
    REQUIRE(TypeScaleHelper<int8_t, int32_t>::scale(27) == 463182747);
    REQUIRE(TypeScaleHelper<int8_t, int32_t>::scale(42) == 715827882);
    REQUIRE(TypeScaleHelper<int8_t, int8_t>::scale(-42) == -42);
    REQUIRE(TypeScaleHelper<int8_t, int8_t>::scale(0) == 0);
    REQUIRE(TypeScaleHelper<int8_t, int8_t>::scale(27) == 27);
    REQUIRE(TypeScaleHelper<int8_t, int8_t>::scale(42) == 42);
    REQUIRE(TypeScaleHelper<int8_t, uint16_t>::scale(-42) == 22102);
    REQUIRE(TypeScaleHelper<int8_t, uint16_t>::scale(0) == 32896);
    REQUIRE(TypeScaleHelper<int8_t, uint16_t>::scale(27) == 39835);
    REQUIRE(TypeScaleHelper<int8_t, uint16_t>::scale(42) == 43690);
    REQUIRE(TypeScaleHelper<int8_t, uint32_t>::scale(-42) == 1448498774);
    REQUIRE(TypeScaleHelper<int8_t, uint32_t>::scale(0) == 2155905152);
    REQUIRE(TypeScaleHelper<int8_t, uint32_t>::scale(27) == 2610666395);
    REQUIRE(TypeScaleHelper<int8_t, uint32_t>::scale(42) == 2863311530);
    REQUIRE(TypeScaleHelper<int8_t, uint8_t>::scale(-42) == 86);
    REQUIRE(TypeScaleHelper<int8_t, uint8_t>::scale(0) == 128);
    REQUIRE(TypeScaleHelper<int8_t, uint8_t>::scale(27) == 155);
    REQUIRE(TypeScaleHelper<int8_t, uint8_t>::scale(42) == 170);
    REQUIRE(TypeScaleHelper<uint16_t, double>::scale(0) == Approx(0.000000000000000));
    REQUIRE(TypeScaleHelper<uint16_t, double>::scale(255) == Approx(0.003891050583658));
    REQUIRE(TypeScaleHelper<uint16_t, double>::scale(27) == Approx(0.000411993591211));
    REQUIRE(TypeScaleHelper<uint16_t, double>::scale(32980) == Approx(0.503242542153048));
    REQUIRE(TypeScaleHelper<uint16_t, double>::scale(42) == Approx(0.000640878919661));
    REQUIRE(TypeScaleHelper<uint16_t, float>::scale(0) == Approx(0.000000000000000));
    REQUIRE(TypeScaleHelper<uint16_t, float>::scale(255) == Approx(0.003891050583658));
    REQUIRE(TypeScaleHelper<uint16_t, float>::scale(27) == Approx(0.000411993591211));
    REQUIRE(TypeScaleHelper<uint16_t, float>::scale(32980) == Approx(0.503242542153048));
    REQUIRE(TypeScaleHelper<uint16_t, float>::scale(42) == Approx(0.000640878919661));
    REQUIRE(TypeScaleHelper<uint16_t, int16_t>::scale(0) == -32768);
    REQUIRE(TypeScaleHelper<uint16_t, int16_t>::scale(255) == -32513);
    REQUIRE(TypeScaleHelper<uint16_t, int16_t>::scale(27) == -32741);
    REQUIRE(TypeScaleHelper<uint16_t, int16_t>::scale(32980) == 212);
    REQUIRE(TypeScaleHelper<uint16_t, int16_t>::scale(42) == -32726);
    REQUIRE(TypeScaleHelper<uint16_t, int32_t>::scale(0) == -2147483648);
    REQUIRE(TypeScaleHelper<uint16_t, int32_t>::scale(255) == -2130771713);
    REQUIRE(TypeScaleHelper<uint16_t, int32_t>::scale(27) == -2145714149);
    REQUIRE(TypeScaleHelper<uint16_t, int32_t>::scale(32980) == 13926612);
    REQUIRE(TypeScaleHelper<uint16_t, int32_t>::scale(42) == -2144731094);
    REQUIRE(TypeScaleHelper<uint16_t, int8_t>::scale(0) == -128);
    REQUIRE(TypeScaleHelper<uint16_t, int8_t>::scale(255) == -127);
    REQUIRE(TypeScaleHelper<uint16_t, int8_t>::scale(27) == -127);
    REQUIRE(TypeScaleHelper<uint16_t, int8_t>::scale(32980) == 0);
    REQUIRE(TypeScaleHelper<uint16_t, int8_t>::scale(42) == -127);
    REQUIRE(TypeScaleHelper<uint16_t, uint16_t>::scale(0) == 0);
    REQUIRE(TypeScaleHelper<uint16_t, uint16_t>::scale(255) == 255);
    REQUIRE(TypeScaleHelper<uint16_t, uint16_t>::scale(27) == 27);
    REQUIRE(TypeScaleHelper<uint16_t, uint16_t>::scale(32980) == 32980);
    REQUIRE(TypeScaleHelper<uint16_t, uint16_t>::scale(42) == 42);
    REQUIRE(TypeScaleHelper<uint16_t, uint32_t>::scale(0) == 0);
    REQUIRE(TypeScaleHelper<uint16_t, uint32_t>::scale(255) == 16711935);
    REQUIRE(TypeScaleHelper<uint16_t, uint32_t>::scale(27) == 1769499);
    REQUIRE(TypeScaleHelper<uint16_t, uint32_t>::scale(32980) == 2161410260);
    REQUIRE(TypeScaleHelper<uint16_t, uint32_t>::scale(42) == 2752554);
    REQUIRE(TypeScaleHelper<uint16_t, uint8_t>::scale(0) == 0);
    REQUIRE(TypeScaleHelper<uint16_t, uint8_t>::scale(255) == 0);
    REQUIRE(TypeScaleHelper<uint16_t, uint8_t>::scale(27) == 0);
    REQUIRE(TypeScaleHelper<uint16_t, uint8_t>::scale(32980) == 128);
    REQUIRE(TypeScaleHelper<uint16_t, uint8_t>::scale(42) == 0);
    REQUIRE(TypeScaleHelper<uint32_t, double>::scale(0) == Approx(0.000000000000000));
    REQUIRE(TypeScaleHelper<uint32_t, double>::scale(255) == Approx(0.000000059371814));
    REQUIRE(TypeScaleHelper<uint32_t, double>::scale(27) == Approx(0.000000006286427));
    REQUIRE(TypeScaleHelper<uint32_t, double>::scale(32980) == Approx(0.000007678754629));
    REQUIRE(TypeScaleHelper<uint32_t, double>::scale(42) == Approx(0.000000009778887));
    REQUIRE(TypeScaleHelper<uint32_t, float>::scale(0) == Approx(0.000000000000000));
    REQUIRE(TypeScaleHelper<uint32_t, float>::scale(255) == Approx(0.000000059371814));
    REQUIRE(TypeScaleHelper<uint32_t, float>::scale(27) == Approx(0.000000006286427));
    REQUIRE(TypeScaleHelper<uint32_t, float>::scale(32980) == Approx(0.000007678754629));
    REQUIRE(TypeScaleHelper<uint32_t, float>::scale(42) == Approx(0.000000009778887));
    REQUIRE(TypeScaleHelper<uint32_t, int16_t>::scale(0) == -32768);
    REQUIRE(TypeScaleHelper<uint32_t, int16_t>::scale(255) == -32767);
    REQUIRE(TypeScaleHelper<uint32_t, int16_t>::scale(27) == -32767);
    REQUIRE(TypeScaleHelper<uint32_t, int16_t>::scale(32980) == -32767);
    REQUIRE(TypeScaleHelper<uint32_t, int16_t>::scale(42) == -32767);
    REQUIRE(TypeScaleHelper<uint32_t, int32_t>::scale(0) == -2147483648);
    REQUIRE(TypeScaleHelper<uint32_t, int32_t>::scale(255) == -2147483393);
    REQUIRE(TypeScaleHelper<uint32_t, int32_t>::scale(27) == -2147483621);
    REQUIRE(TypeScaleHelper<uint32_t, int32_t>::scale(32980) == -2147450668);
    REQUIRE(TypeScaleHelper<uint32_t, int32_t>::scale(42) == -2147483606);
    REQUIRE(TypeScaleHelper<uint32_t, int8_t>::scale(0) == -128);
    REQUIRE(TypeScaleHelper<uint32_t, int8_t>::scale(255) == -127);
    REQUIRE(TypeScaleHelper<uint32_t, int8_t>::scale(27) == -127);
    REQUIRE(TypeScaleHelper<uint32_t, int8_t>::scale(32980) == -127);
    REQUIRE(TypeScaleHelper<uint32_t, int8_t>::scale(42) == -127);
    REQUIRE(TypeScaleHelper<uint32_t, uint16_t>::scale(0) == 0);
    REQUIRE(TypeScaleHelper<uint32_t, uint16_t>::scale(255) == 0);
    REQUIRE(TypeScaleHelper<uint32_t, uint16_t>::scale(27) == 0);
    REQUIRE(TypeScaleHelper<uint32_t, uint16_t>::scale(32980) == 0);
    REQUIRE(TypeScaleHelper<uint32_t, uint16_t>::scale(42) == 0);
    REQUIRE(TypeScaleHelper<uint32_t, uint32_t>::scale(0) == 0);
    REQUIRE(TypeScaleHelper<uint32_t, uint32_t>::scale(255) == 255);
    REQUIRE(TypeScaleHelper<uint32_t, uint32_t>::scale(27) == 27);
    REQUIRE(TypeScaleHelper<uint32_t, uint32_t>::scale(32980) == 32980);
    REQUIRE(TypeScaleHelper<uint32_t, uint32_t>::scale(42) == 42);
    REQUIRE(TypeScaleHelper<uint32_t, uint8_t>::scale(0) == 0);
    REQUIRE(TypeScaleHelper<uint32_t, uint8_t>::scale(255) == 0);
    REQUIRE(TypeScaleHelper<uint32_t, uint8_t>::scale(27) == 0);
    REQUIRE(TypeScaleHelper<uint32_t, uint8_t>::scale(32980) == 0);
    REQUIRE(TypeScaleHelper<uint32_t, uint8_t>::scale(42) == 0);
    REQUIRE(TypeScaleHelper<uint8_t, double>::scale(0) == Approx(0.000000000000000));
    REQUIRE(TypeScaleHelper<uint8_t, double>::scale(27) == Approx(0.105882352941176));
    REQUIRE(TypeScaleHelper<uint8_t, double>::scale(42) == Approx(0.164705882352941));
    REQUIRE(TypeScaleHelper<uint8_t, float>::scale(0) == Approx(0.000000000000000));
    REQUIRE(TypeScaleHelper<uint8_t, float>::scale(27) == Approx(0.105882352941176));
    REQUIRE(TypeScaleHelper<uint8_t, float>::scale(42) == Approx(0.164705882352941));
    REQUIRE(TypeScaleHelper<uint8_t, int16_t>::scale(0) == -32768);
    REQUIRE(TypeScaleHelper<uint8_t, int16_t>::scale(27) == -25829);
    REQUIRE(TypeScaleHelper<uint8_t, int16_t>::scale(42) == -21974);
    REQUIRE(TypeScaleHelper<uint8_t, int32_t>::scale(0) == -2147483648);
    REQUIRE(TypeScaleHelper<uint8_t, int32_t>::scale(27) == -1692722405);
    REQUIRE(TypeScaleHelper<uint8_t, int32_t>::scale(42) == -1440077270);
    REQUIRE(TypeScaleHelper<uint8_t, int8_t>::scale(0) == -128);
    REQUIRE(TypeScaleHelper<uint8_t, int8_t>::scale(27) == -101);
    REQUIRE(TypeScaleHelper<uint8_t, int8_t>::scale(42) == -86);
    REQUIRE(TypeScaleHelper<uint8_t, uint16_t>::scale(0) == 0);
    REQUIRE(TypeScaleHelper<uint8_t, uint16_t>::scale(27) == 6939);
    REQUIRE(TypeScaleHelper<uint8_t, uint16_t>::scale(42) == 10794);
    REQUIRE(TypeScaleHelper<uint8_t, uint32_t>::scale(0) == 0);
    REQUIRE(TypeScaleHelper<uint8_t, uint32_t>::scale(27) == 454761243);
    REQUIRE(TypeScaleHelper<uint8_t, uint32_t>::scale(42) == 707406378);
    REQUIRE(TypeScaleHelper<uint8_t, uint8_t>::scale(0) == 0);
    REQUIRE(TypeScaleHelper<uint8_t, uint8_t>::scale(27) == 27);
    REQUIRE(TypeScaleHelper<uint8_t, uint8_t>::scale(42) == 42);
}

SCENARIO("tests for scaledCopyFrom", "[core/multidim_image_platform]")
{
    GIVEN("an uint8 image") {
        MultiDimImage<std::uint8_t> im({16, 24});
        im.at({7, 9}) = 42;
        im.at({7, 10}) = 0;
        im.at({7, 11}) = 255;

        WHEN("converted to uint8 image") {
            MultiDimImage<std::uint8_t> dst;
            dst.scaledCopyFrom(im);
            THEN("nothing is changed") {
                REQUIRE(dst.at({7, 9}) == static_cast<uint8_t>(42));
                REQUIRE(dst.at({7, 10}) == static_cast<uint8_t>(0));
                REQUIRE(dst.at({7, 11}) == static_cast<uint8_t>(255));
            }
        }

        WHEN("converted to float image") {
            MultiDimImage<float> dst;
            dst.scaledCopyFrom(im);
            THEN("the values converted correctly") {
                REQUIRE(dst.at({7, 9}) == 42.0f/255.0f);
                REQUIRE(dst.at({7, 10}) == 0.0f);
                REQUIRE(dst.at({7, 11}) == 1.0f);
            }
        }

        WHEN("converted to double image") {
            MultiDimImage<double> dst;
            dst.scaledCopyFrom(im);
            THEN("the values converted correctly") {
                REQUIRE(dst.at({7, 9}) == 42.0/255.0);
                REQUIRE(dst.at({7, 10}) == 0.0);
                REQUIRE(dst.at({7, 11}) == 1.0);
            }
        }

        WHEN("converted to double image") {
            MultiDimImage<std::int8_t> dst;
            dst.scaledCopyFrom(im);
            THEN("the values converted correctly") {
                REQUIRE(dst.at({7, 9}) == -86);
                REQUIRE(dst.at({7, 10}) == -128);
                REQUIRE(dst.at({7, 11}) == 127);
            }
        }

        WHEN("converted to uint32 image") {
            MultiDimImage<std::uint32_t> dst;
            dst.scaledCopyFrom(im);
            THEN("the values converted correctly") {
                REQUIRE(dst.at({7, 9}) == 707406378);
                REQUIRE(dst.at({7, 10}) == 0);
                REQUIRE(dst.at({7, 11}) == 4294967295);
            }
        }

        WHEN("converted to int32 image") {
            MultiDimImage<std::int32_t> dst;
            dst.scaledCopyFrom(im);
            THEN("the values converted correctly") {
                REQUIRE(dst.at({7, 9}) == -1440077270);
                REQUIRE(dst.at({7, 10}) == -2147483648);
                REQUIRE(dst.at({7, 11}) == 2147483647);
            }
        }
    }

    GIVEN("an double image") {
        MultiDimImage<double> im({16, 24});
        im.at({7, 9}) = 0.3141519;
        im.at({7, 10}) = 0.0;
        im.at({7, 11}) = 1.0;

        WHEN("converted to double image") {
            MultiDimImage<double> dst;
            dst.scaledCopyFrom(im);
            THEN("nothing is changed") {
                REQUIRE(dst.at({7, 9}) == 0.3141519);
                REQUIRE(dst.at({7, 10}) == 0.0);
                REQUIRE(dst.at({7, 11}) == 1.0);
            }
        }

        WHEN("converted to float image") {
            MultiDimImage<float> dst;
            dst.scaledCopyFrom(im);
            THEN("the values converted correctly") {
                REQUIRE(dst.at({7, 9}) == 0.3141519f);
                REQUIRE(dst.at({7, 10}) == 0.0f);
                REQUIRE(dst.at({7, 11}) == 1.0f);
            }
        }
    }
}

SCENARIO("removing dimensions", "[core/multidim_image_platform]")
{
    GIVEN("a multidim image") {
        MultiDimImage<double> im({170, 160, 20, 5, 3});
        im.at({0,  0,  0,  0,  0}) = 0.1;
        im.at({1,  0,  0,  0,  0}) = 0.2;
        im.at({0,  1,  0,  0,  0}) = 0.3;
        im.at({0,  0,  1,  0,  0}) = 0.4;
        im.at({0,  0,  0,  1,  0}) = 0.5;
        im.at({0,  0,  0,  0,  1}) = 0.6;
        im.at({17, 0,  0,  0,  0}) = 0.25;
        im.at({0,  17, 0,  0,  0}) = 0.35;
        im.at({0,  0,  17, 0,  0}) = 0.45;
        im.at({0,  0,  0,  4,  0}) = 0.55;
        im.at({0,  0,  0,  0,  2}) = 0.65;

        REQUIRE(im.dims() == 5);
        REQUIRE(im.dim(0) == 170);
        REQUIRE(im.dim(1) == 160);
        REQUIRE(im.dim(2) == 20);
        REQUIRE(im.dim(3) == 5);
        REQUIRE(im.dim(4) == 3);

        WHEN("removing the 1st dimension") {
            im.removeDims({0});
            THEN("it is removed") {
                REQUIRE(im.dims() == 4);
                REQUIRE(im.dim(0) == 160);
                REQUIRE(im.dim(1) == 20);
                REQUIRE(im.dim(2) == 5);
                REQUIRE(im.dim(3) == 3);

                REQUIRE(im.at({0,  0,  0,  0 }) == 0.1 );
                REQUIRE(im.at({1,  0,  0,  0 }) == 0.3 );
                REQUIRE(im.at({0,  1,  0,  0 }) == 0.4 );
                REQUIRE(im.at({0,  0,  1,  0 }) == 0.5 );
                REQUIRE(im.at({0,  0,  0,  1 }) == 0.6 );
                REQUIRE(im.at({17, 0,  0,  0 }) == 0.35);
                REQUIRE(im.at({0,  17, 0,  0 }) == 0.45);
                REQUIRE(im.at({0,  0,  4,  0 }) == 0.55);
                REQUIRE(im.at({0,  0,  0,  2}) == 0.65);
            }
        }

        WHEN("removing the 2nd dimension") {
            im.removeDims({1});
            THEN("it is removed") {
                REQUIRE(im.dims() == 4);
                REQUIRE(im.dim(0) == 170);
                REQUIRE(im.dim(1) == 20);
                REQUIRE(im.dim(2) == 5);
                REQUIRE(im.dim(3) == 3);

                REQUIRE(im.at({0,  0,  0,  0 }) == 0.1 );
                REQUIRE(im.at({1,  0,  0,  0 }) == 0.2 );
                REQUIRE(im.at({0,  1,  0,  0 }) == 0.4 );
                REQUIRE(im.at({0,  0,  1,  0 }) == 0.5 );
                REQUIRE(im.at({0,  0,  0,  1 }) == 0.6 );
                REQUIRE(im.at({17, 0,  0,  0 }) == 0.25);
                REQUIRE(im.at({0,  17, 0,  0 }) == 0.45);
                REQUIRE(im.at({0,  0,  4,  0 }) == 0.55);
                REQUIRE(im.at({0,  0,  0,  2 }) == 0.65);
            }
        }

        WHEN("removing the 3rd dimension") {
            im.removeDims({2});
            THEN("it is removed") {
                REQUIRE(im.dims() == 4);
                REQUIRE(im.dim(0) == 170);
                REQUIRE(im.dim(1) == 160);
                REQUIRE(im.dim(2) == 5);
                REQUIRE(im.dim(3) == 3);

                REQUIRE(im.at({0,  0,  0,  0 }) == 0.1 );
                REQUIRE(im.at({1,  0,  0,  0 }) == 0.2 );
                REQUIRE(im.at({0,  1,  0,  0 }) == 0.3 );
                REQUIRE(im.at({0,  0,  1,  0 }) == 0.5 );
                REQUIRE(im.at({0,  0,  0,  1 }) == 0.6 );
                REQUIRE(im.at({17, 0,  0,  0 }) == 0.25);
                REQUIRE(im.at({0,  17, 0,  0 }) == 0.35);
                REQUIRE(im.at({0,  0,  4, 0 }) == 0.55);
                REQUIRE(im.at({0,  0,  0,  2}) == 0.65);
            }
        }

        WHEN("removing the 4th dimension") {
            im.removeDims({3});
            THEN("it is removed") {
                REQUIRE(im.dims() == 4);
                REQUIRE(im.dim(0) == 170);
                REQUIRE(im.dim(1) == 160);
                REQUIRE(im.dim(2) == 20);
                REQUIRE(im.dim(3) == 3);

                REQUIRE(im.at({0,  0,  0,  0 }) == 0.1 );
                REQUIRE(im.at({1,  0,  0,  0 }) == 0.2 );
                REQUIRE(im.at({0,  1,  0,  0 }) == 0.3 );
                REQUIRE(im.at({0,  0,  1,  0 }) == 0.4 );
                REQUIRE(im.at({0,  0,  0,  1 }) == 0.6 );
                REQUIRE(im.at({17, 0,  0,  0 }) == 0.25);
                REQUIRE(im.at({0,  17, 0,  0 }) == 0.35);
                REQUIRE(im.at({0,  0,  17, 0 }) == 0.45);
                REQUIRE(im.at({0,  0,  0,  2 }) == 0.65);
            }
        }

        WHEN("removing the 5th dimension") {
            im.removeDims({4});
            THEN("it is removed") {
                REQUIRE(im.dims() == 4);
                REQUIRE(im.dim(0) == 170);
                REQUIRE(im.dim(1) == 160);
                REQUIRE(im.dim(2) == 20);
                REQUIRE(im.dim(3) == 5);

                REQUIRE(im.at({0,  0,  0,  0 }) == 0.1 );
                REQUIRE(im.at({1,  0,  0,  0 }) == 0.2 );
                REQUIRE(im.at({0,  1,  0,  0 }) == 0.3 );
                REQUIRE(im.at({0,  0,  1,  0 }) == 0.4 );
                REQUIRE(im.at({0,  0,  0,  1 }) == 0.5 );
                REQUIRE(im.at({17, 0,  0,  0 }) == 0.25);
                REQUIRE(im.at({0,  17, 0,  0 }) == 0.35);
                REQUIRE(im.at({0,  0,  17, 0 }) == 0.45);
                REQUIRE(im.at({0,  0,  0,  4 }) == 0.55);
            }
        }
    }
}

SCENARIO("removing multiple dimensions", "[core/multidim_image_platform]")
{
    GIVEN("a multidim image") {
        MultiDimImage<double> im({17, 16, 3});
        im.at({1, 0, 0}) = 0.01;
        im.at({2, 0, 0}) = 0.02;
        im.at({3, 0, 0}) = 0.03;
        im.at({15, 0, 0}) = 0.015;
        im.at({16, 0, 0}) = 0.016;

        im.at({0, 1, 0}) = 0.11;
        im.at({0, 2, 0}) = 0.12;
        im.at({0, 3, 0}) = 0.13;
        im.at({0, 14, 0}) = 0.115;
        im.at({0, 15, 0}) = 0.116;

        im.at({0, 0, 0}) = 0.21;
        im.at({0, 0, 1}) = 0.22;
        im.at({0, 0, 2}) = 0.23;

        REQUIRE(im.dims() == 3);
        REQUIRE(im.dim(0) == 17);
        REQUIRE(im.dim(1) == 16);
        REQUIRE(im.dim(2) == 3);

        WHEN("removing the 1st and the 2nd dimensions") {
            im.removeDims({0, 1});
            THEN("they are is removed") {
                REQUIRE(im.dims() == 1);
                REQUIRE(im.dim(0) == 3);

                REQUIRE(im.at({0}) == 0.21);
                REQUIRE(im.at({1}) == 0.22);
                REQUIRE(im.at({2}) == 0.23);
            }
        }

        WHEN("removing the 1st and the 3rd dimensions") {
            im.removeDims({0, 2});
            THEN("they are is removed") {
                REQUIRE(im.dims() == 1);
                REQUIRE(im.dim(0) == 16);

                REQUIRE(im.at({1}) == 0.11);
                REQUIRE(im.at({2}) == 0.12);
                REQUIRE(im.at({3}) == 0.13);
                REQUIRE(im.at({14}) == 0.115);
                REQUIRE(im.at({15}) == 0.116);
            }
        }

        WHEN("removing the 2nd and the 3rd dimensions") {
            im.removeDims({2, 1});
            THEN("they are is removed") {
                REQUIRE(im.dims() == 1);
                REQUIRE(im.dim(0) == 17);

                REQUIRE(im.at({1}) == 0.01);
                REQUIRE(im.at({2}) == 0.02);
                REQUIRE(im.at({3}) == 0.03);
                REQUIRE(im.at({15}) == 0.015);
                REQUIRE(im.at({16}) == 0.016);
            }
        }

        WHEN("removing all the dimensions") {
            im.removeDims({2, 0, 1});
            THEN("they are all removed") {
                REQUIRE(im.dims() == 0);
                REQUIRE(im.empty() == true);
            }
        }

        WHEN("removing non-existing dimension") {
            THEN("exception is thrown") {
                REQUIRE_THROWS(im.removeDims({3}));
            }
        }

        WHEN("removing existing and non-existing dimensions") {
            THEN("exception is thrown") {
                REQUIRE_THROWS(im.removeDims({3, 1, 2, 4}));
            }
        }
    }
}

SCENARIO("sliptting dimensions", "[core/multidim_image_platform]")
{
    GIVEN("a multidim image") {
        MultiDimImage<double> im({17, 16, 3});
        im.at({1, 0, 0}) = 0.01;
        im.at({2, 0, 0}) = 0.02;
        im.at({3, 0, 0}) = 0.03;
        im.at({15, 0, 0}) = 0.015;
        im.at({16, 0, 0}) = 0.016;

        im.at({0, 1, 0}) = 0.11;
        im.at({0, 2, 0}) = 0.12;
        im.at({0, 3, 0}) = 0.13;
        im.at({0, 14, 0}) = 0.115;
        im.at({0, 15, 0}) = 0.116;

        im.at({0, 0, 0}) = 0.21;
        im.at({0, 0, 1}) = 0.22;
        im.at({0, 0, 2}) = 0.23;

        REQUIRE(im.dims() == 3);
        REQUIRE(im.dim(0) == 17);
        REQUIRE(im.dim(1) == 16);
        REQUIRE(im.dim(2) == 3);

        WHEN("splitting the 1st dimension") {
            std::vector<MultiDimImage<double>> parts = im.splitDim(0);
            THEN("it is splitted") {
                REQUIRE(parts.size() == 17);
                for (auto& p : parts) {
                    REQUIRE(p.dims() == 2);
                    REQUIRE(p.dim(0) == 16);
                    REQUIRE(p.dim(1) == 3);
                }

                REQUIRE(parts[1].at({0, 0}) == 0.01);
                REQUIRE(parts[2].at({0, 0}) == 0.02);
                REQUIRE(parts[3].at({0, 0}) == 0.03);
                REQUIRE(parts[15].at({0, 0}) == 0.015);
                REQUIRE(parts[16].at({0, 0}) == 0.016);
            }
        }

        WHEN("splitting the 2nd dimension") {
            std::vector<MultiDimImage<double>> parts = im.splitDim(1);
            THEN("it is splitted") {
                REQUIRE(parts.size() == 16);
                for (auto& p : parts) {
                    REQUIRE(p.dims() == 2);
                    REQUIRE(p.dim(0) == 17);
                    REQUIRE(p.dim(1) == 3);
                }

                REQUIRE(parts[1].at({0, 0}) == 0.11);
                REQUIRE(parts[2].at({0, 0}) == 0.12);
                REQUIRE(parts[3].at({0, 0}) == 0.13);
                REQUIRE(parts[14].at({0, 0}) == 0.115);
                REQUIRE(parts[15].at({0, 0}) == 0.116);
            }
        }

        WHEN("splitting the 3rd dimension") {
            std::vector<MultiDimImage<double>> parts = im.splitDim(2);
            THEN("it is splitted") {
                REQUIRE(parts.size() == 3);
                for (auto& p : parts) {
                    REQUIRE(p.dims() == 2);
                    REQUIRE(p.dim(0) == 17);
                    REQUIRE(p.dim(1) == 16);
                }

                REQUIRE(parts[0].at({0, 0}) == 0.21);
                REQUIRE(parts[1].at({0, 0}) == 0.22);
                REQUIRE(parts[2].at({0, 0}) == 0.23);
            }
        }

        WHEN("splitting a non-existing dimension") {
            THEN("exception is raised") {
                REQUIRE_THROWS(im.splitDim(3));
            }
        }
    }
}
