#include "catch.hpp"

#include <core/multidim_image_platform/MultiDimImage.hpp>

using namespace core::multidim_image_platform;
using namespace std;

SCENARIO("multidim image basic usage", "[core/multidim_image_platform]")
{
    GIVEN("two images, an empty and a 2d image with some content") {
        MultiDimImage<uint8_t> emptyImage;
        REQUIRE(emptyImage.size() == 0);
        REQUIRE(emptyImage.byteSize() == 0);
        REQUIRE(emptyImage.dims() == 0);
        REQUIRE_THROWS(emptyImage.dim(0));
        REQUIRE_THROWS(emptyImage.dim(23));
        REQUIRE(emptyImage.empty() == true);

        MultiDimImage<uint8_t> image2d({16, 32});
        
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
            MultiDimImage<uint8_t> newImage(image2d);
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
            im3.convertCopy(im2);
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
    GIVEN("the demand for a huge volume with 4 channels, 2048x2048x64x4 float") {
        WHEN("created") {
            MultiDimImage<float> huge({2048, 2048, 64, 4});
            ImageViewPair<float> wpp = weakPointerToImageData(huge);
            THEN("it is allocated without problems") {
                REQUIRE(huge.size() == 2048 * 2048 * 64 * 4);
                REQUIRE(huge.byteSize() == 2048 * 2048 * 64 * 4 * sizeof(float));
                REQUIRE(huge.dims() == 4);
                REQUIRE(huge.dim(0) == 2048);
                REQUIRE(huge.dim(1) == 2048);
                REQUIRE(huge.dim(2) == 64);
                REQUIRE(huge.dim(3) == 4);
                REQUIRE(huge.type() == GetType<float>());
                REQUIRE(huge.empty() == false);
            }

            std::shared_ptr<MultiDimImage<float>> hugeCopy = std::make_shared<MultiDimImage<float>>();
            ImageViewPair<float> wppCpy;
            AND_WHEN("copied") {
                *hugeCopy = huge;
                wppCpy = weakPointerToImageData(*hugeCopy);
                THEN("the copy is allocated without problems") {
                    REQUIRE(hugeCopy->size() == 2048 * 2048 * 64 * 4);
                    REQUIRE(hugeCopy->byteSize() == 2048 * 2048 * 64 * 4 * sizeof(float));
                    REQUIRE(hugeCopy->dims() == 4);
                    REQUIRE(hugeCopy->dim(0) == 2048);
                    REQUIRE(hugeCopy->dim(1) == 2048);
                    REQUIRE(hugeCopy->dim(2) == 64);
                    REQUIRE(hugeCopy->dim(3) == 4);
                    REQUIRE(hugeCopy->type() == GetType<float>());
                    REQUIRE(hugeCopy->empty() == false);
                }
            }

            AND_WHEN("the copy is destroyed") {
                hugeCopy.reset();
                THEN("the memory is freed") {
                    checkWeakPtrNull<float>(wppCpy);
                }
            }

            AND_WHEN("the original is cleared") {
                huge.clear();
                THEN("the memory is freed") {
                    checkWeakPtrNull<float>(wpp);
                }
            }
        }
    }
}

SCENARIO("multidim metadata usage", "[core/multidim_image_platform]")
{
    GIVEN("an image") {
        MultiDimImage<uint8_t> im;
        WHEN("meta is added") {
            im.meta.add<int>("int_meta_test", 42);
            im.meta.add<float>("float_meta_test", 2024.2024);
            im.meta.add<std::string>("string_meta_test", "testing 1, 2, 3...");
            im.meta.add<char>("to be removed", 0);
            THEN("it is stored correctly") {
                REQUIRE(im.meta.get<int>("int_meta_test") == 42);
                REQUIRE(im.meta.get<float>("float_meta_test") == 2024.2024);
                REQUIRE(im.meta.get<std::string>("string_meta_test") == "testing 1, 2, 3...");
                REQUIRE(im.meta.get<char>("to be removed") == 0);
            }

            AND_THEN("the data can only be reached type correctly") {
                REQUIRE(im.meta.has<int>("int_meta_test") == true);
                REQUIRE(im.meta.has<float>("int_meta_test") == false);
                REQUIRE_THROWS(im.meta.get<float>("int_meta_test"));
                
                REQUIRE(im.meta.has<std::string>("string_meta_test") == true);
                REQUIRE(im.meta.has<int>("string_meta_test") == false);
                REQUIRE_THROWS(im.meta.get<int>("string_meta_test"));
            }
            
            AND_WHEN("one is removed") {
                im.meta.remove<char>("to be removed");
                THEN("it is no longer reachable") {
                    REQUIRE(im.meta.has<char>("to be removed") == false);
                    REQUIRE_THROWS(im.meta.get<char>("to be removed"));
                }
            }
        }
    }
}

SCENARIO("multidim subdata from multidim", "[core/multidim_image_platform]")
{
    GIVEN("an image") {
        MultiDimImage<uint8_t> image({16, 16, 4, 2});
        image.at({0, 0, 0, 0}) = 42;
        image.at({0, 0, 1, 0}) = 43;
        image.at({0, 0, 2, 0}) = 44;
        image.at({0, 0, 3, 0}) = 45;
        image.at({0, 0, 0, 1}) = 82;
        image.at({0, 0, 1, 1}) = 83;
        image.at({0, 0, 2, 1}) = 84;
        image.at({0, 0, 3, 1}) = 85;
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
                REQUIRE(view1.at({0, 0}) == 42);
                REQUIRE(view2.at({0, 0}) == 43);
                REQUIRE(view3.at({0, 0}) == 44);
                REQUIRE(view4.at({0, 0}) == 45);
                REQUIRE(view5.at({0, 0}) == 82);
                REQUIRE(view6.at({0, 0}) == 83);
                REQUIRE(view7.at({0, 0}) == 84);
                REQUIRE(view8.at({0, 0}) == 85);
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
                REQUIRE(vol1.empty() == false);
                REQUIRE(vol1.size() == 16 * 16);
                REQUIRE(vol1.valid() == true);
                REQUIRE(vol1.parent() == &image);

                REQUIRE(vol2.dims() == 3);
                REQUIRE(vol2.dim(0) == 16);
                REQUIRE(vol2.dim(1) == 16);
                REQUIRE(vol2.empty() == false);
                REQUIRE(vol2.size() == 16 * 16);
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
    GIVEN("an image") {
        MultiDimImage<uint8_t> image({512, 512, 16, 3});
        image.at({511, 0, 0, 1}) = 42;
        WHEN("first two dimensions reordered") {
            image.reorderDims({1, 0, 2, 3});
            THEN("the data is reordered") {
                REQUIRE(image.size() == 512 * 512 * 16 * 3);
                REQUIRE(image.dims() == 4);
                REQUIRE(image.dim(0) == 512);
                REQUIRE(image.dim(1) == 512);
                REQUIRE(image.dim(2) == 16);
                REQUIRE(image.dim(3) == 3);
                REQUIRE(image.at({0, 511, 0, 1}) == 42);
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

        WHEN("complex reordeing happens") {
            image.reorderDims({3, 0, 1, 2});
            THEN("the data is reordered") {
                REQUIRE(image.size() == 512 * 512 * 16 * 3);
                REQUIRE(image.dims() == 4);
                REQUIRE(image.dim(0) == 3);
                REQUIRE(image.dim(1) == 512);
                REQUIRE(image.dim(2) == 512);
                REQUIRE(image.dim(3) == 16);
                REQUIRE(image.at({1, 511, 0, 0}) == 42);
            }
        }

        WHEN("the reordering is the same") {
            image.reorderDims({0, 1, 2, 3});
            THEN("the data is the same") {
                REQUIRE(image.size() == 512 * 512 * 16 * 3);
                REQUIRE(image.dims() == 4);
                REQUIRE(image.dim(0) == 512);
                REQUIRE(image.dim(1) == 512);
                REQUIRE(image.dim(2) == 16);
                REQUIRE(image.dim(3) == 3);
                REQUIRE(image.at({511, 0, 0, 0}) == 42);
            }
        }
    }
}
