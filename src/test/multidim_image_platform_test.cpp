#include "catch.hpp"

#include <core/multidim_image_platform/MultiDimImage.hpp>

using namespace core::multidim_image_platform;
using namespace std;

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
    std::cout << "." << std::flush;
    std::size_t n = 2048;
    GIVEN("the demand for a huge volume with 4 channels, 2048x2048x64x4 float") {
        WHEN("created") {
            std::cout << "." << std::flush;
            MultiDimImage<float> huge({n, n, 64, 4});
            ImageViewPair<float> wpp = weakPointerToImageData(huge);
            THEN("it is allocated without problems") {
                std::cout << "." << std::flush;
                REQUIRE(huge.size() == n * n * 64 * 4);
                REQUIRE(huge.byteSize() == n * n * 64 * 4 * sizeof(float));
                REQUIRE(huge.dims() == 4);
                REQUIRE(huge.dim(0) == n);
                REQUIRE(huge.dim(1) == n);
                REQUIRE(huge.dim(2) == 64);
                REQUIRE(huge.dim(3) == 4);
                REQUIRE(huge.type() == GetType<float>());
                REQUIRE(huge.empty() == false);
            }

            std::shared_ptr<MultiDimImage<float>> hugeCopy = std::make_shared<MultiDimImage<float>>();
            ImageViewPair<float> wppCpy;
            AND_WHEN("copied") {
                std::cout << "." << std::flush;
                *hugeCopy = huge;
                wppCpy = weakPointerToImageData(*hugeCopy);
                THEN("the copy is allocated without problems") {
                    std::cout << "." << std::flush;
                    REQUIRE(hugeCopy->size() == n * n * 64 * 4);
                    REQUIRE(hugeCopy->byteSize() == n * n * 64 * 4 * sizeof(float));
                    REQUIRE(hugeCopy->dims() == 4);
                    REQUIRE(hugeCopy->dim(0) == n);
                    REQUIRE(hugeCopy->dim(1) == n);
                    REQUIRE(hugeCopy->dim(2) == 64);
                    REQUIRE(hugeCopy->dim(3) == 4);
                    REQUIRE(hugeCopy->type() == GetType<float>());
                    REQUIRE(hugeCopy->empty() == false);
                }
            }

            AND_WHEN("the copy is destroyed") {
                std::cout << "." << std::flush;
                hugeCopy.reset();
                THEN("the memory is freed") {
                    std::cout << "." << std::flush;
                    checkWeakPtrNull<float>(wppCpy);
                }
            }

            AND_WHEN("the original is cleared") {
                std::cout << "." << std::flush;
                huge.clear();
                THEN("the memory is freed") {
                    std::cout << "." << std::flush;
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
    GIVEN("an image") {
        MultiDimImage<uint8_t> image({512, 512, 16, 3});
        image.at({511, 0, 0, 1}) = 42;
        image.at({78, 96, 7, 1}) = 47;
        WHEN("first two dimensions reordered") {
            image.reorderDims({1, 0, 2, 3});
            THEN("the data is reordered") {
                REQUIRE(image.size() == 512 * 512 * 16 * 3);
                REQUIRE(image.dims() == 4);
                REQUIRE(image.dim(0) == 512);
                REQUIRE(image.dim(1) == 512);
                REQUIRE(image.dim(2) == 16);
                REQUIRE(image.dim(3) == 3);
                REQUIRE(image.at({511, 0, 0, 1}) == 0);
                REQUIRE(image.at({78, 96, 7, 1}) == 0);
                REQUIRE(image.at({0, 511, 0, 1}) == 42);
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
                REQUIRE(image.size() == 512 * 512 * 16 * 3);
                REQUIRE(image.dims() == 4);
                REQUIRE(image.dim(0) == 512);
                REQUIRE(image.dim(1) == 512);
                REQUIRE(image.dim(2) == 3);
                REQUIRE(image.dim(3) == 16);
                REQUIRE(image.at({511, 0, 0, 1}) == 0);
                REQUIRE_THROWS(image.at({78, 96, 7, 1}));
                REQUIRE(image.at({511, 0, 1, 0}) == 42);
                REQUIRE(image.at({78, 96, 1, 7}) == 47);
            }
        }

        WHEN("complex reordeing happens #2") {
            image.reorderDims({3, 0, 1, 2});
            THEN("the data is reordered") {
                REQUIRE(image.size() == 512 * 512 * 16 * 3);
                REQUIRE(image.dims() == 4);
                REQUIRE(image.dim(0) == 3);
                REQUIRE(image.dim(1) == 512);
                REQUIRE(image.dim(2) == 512);
                REQUIRE(image.dim(3) == 16);
                REQUIRE_THROWS(image.at({511, 0, 0, 1}));
                REQUIRE_THROWS(image.at({78, 96, 7, 1}));
                REQUIRE(image.at({1, 511, 0, 0}) == 42);
                REQUIRE(image.at({1, 78, 96, 7}) == 47);
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
                REQUIRE(image.at({511, 0, 0, 1}) == 42);
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

        imd.at({11, 47}) = 2024.0;
        imd.at({11, 48}) = 0.0;
        imd.at({11, 49}) = 2024.202420242024;

        WHEN("copy converting from im8s to im64u") {
            im64u.convertCopyFrom(im8s);
            THEN("it is correct") {
                REQUIRE(im64u.size() == 16 * 24);
                REQUIRE(im64u.byteSize() == 16 * 24 * sizeof(std::uint64_t));
                REQUIRE(im64u.dims() == 2);
                REQUIRE(im64u.dim(0) == 16);
                REQUIRE(im64u.dim(1) == 24);
                REQUIRE(im64u.at({7, 9}) == static_cast<std::uint64_t>(-2));
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
                REQUIRE(im8s.at({17, 13}) == static_cast<std::int8_t>(42000));
                REQUIRE(im8s.at({17, 14}) == 0);
                REQUIRE(im8s.at({17, 15}) == static_cast<std::int8_t>(18446744073709551615ULL));
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
                cout << (int)imd.at({11, 47}) << endl;
                cout << (int)im8s.at({11, 47}) << endl;
                cout << (int)static_cast<std::int8_t>(2024.0) << endl;
                // REQUIRE(im8s.at({11, 47}) == static_cast<std::int8_t>((double)2024.0));
                REQUIRE(im8s.at({11, 48}) == 0);
                REQUIRE(im8s.at({11, 49}) == static_cast<std::int8_t>(2024.202420242024));
            }
        }

        // TODO: test saturate

        // WHEN("saturate converting from im8s to im64u") {
        //     im64u.saturateCopyFrom(im8s, 0, 100);
        //     THEN("it is correct") {
        //         REQUIRE(im64u.size() == 16 * 24);
        //         REQUIRE(im64u.byteSize() == 16 * 24 * 8);
        //         REQUIRE(im64u.dims() == 2);
        //         REQUIRE(im64u.dim(0) == 16);
        //         REQUIRE(im64u.dim(1) == 24);
        //         REQUIRE(im64u.at({7, 9}) == 0);
        //         REQUIRE(im64u.at({7, 10}) == 127);
        //         REQUIRE(im64u.at({7, 11}) == 92);
        //         REQUIRE(im64u.at({7, 12}) == 0);
        //     }
        // }

// #define TEST_BLOCK \
// REQUIRE(A.size() == B_SIZE_0 * B_SIZE_1); \
// REQUIRE(A.byteSize() == B_SIZE_0 * B_SIZE_1 * sizeof(A_TYPE)); \
// REQUIRE(A.dims() == 2); \
// REQUIRE(A.dim(0) == B_SIZE_0); \
// REQUIRE(A.dim(1) == B_SIZE_1); \
//   REQUIRE(A.at({7, 9}) == static_cast<A_TYPE>(B.at({7, 9})  ) ); \
//  REQUIRE(A.at({7, 10}) == static_cast<A_TYPE>(B.at({7, 10}) ) ); \
//  REQUIRE(A.at({7, 11}) == static_cast<A_TYPE>(B.at({7, 11}) ) ); \
//  REQUIRE(A.at({7, 12}) == static_cast<A_TYPE>(B.at({7, 12}) ) ); \
// REQUIRE(A.at({17, 13}) == static_cast<A_TYPE>(B.at({17, 13})) ); \
// REQUIRE(A.at({17, 14}) == static_cast<A_TYPE>(B.at({17, 14})) ); \
// REQUIRE(A.at({17, 15}) == static_cast<A_TYPE>(B.at({17, 15})) ); \
// REQUIRE(A.at({17, 16}) == static_cast<A_TYPE>(B.at({17, 16})) ); \
// REQUIRE(A.at({17, 23}) == static_cast<A_TYPE>(B.at({17, 23})) ); \
// REQUIRE(A.at({17, 24}) == static_cast<A_TYPE>(B.at({17, 24})) ); \
// REQUIRE(A.at({17, 25}) == static_cast<A_TYPE>(B.at({17, 25})) ); \
// REQUIRE(A.at({11, 47}) == static_cast<A_TYPE>(B.at({11, 47})) ); \
// REQUIRE(A.at({11, 48}) == static_cast<A_TYPE>(B.at({11, 48})) ); \
// REQUIRE(A.at({11, 49}) == static_cast<A_TYPE>(B.at({11, 49})) );

//         #define A im8s
//         #define A_TYPE std::int8_t
//         #define B im8s
//         #define B_SIZE_0 160
//         #define B_SIZE_1 240
//         WHEN("A <- B convert copied") {
//             A.convertCopy(B);
//             THEN("converted correctly") {
//                 TEST_BLOCK
//             }
//         }
//         #undef A
//         #undef A_TYPE
//         #undef B
//         #undef B_SIZE_0
//         #undef B_SIZE_1

//         #define A im64u
//         #define A_TYPE std::uint64_t
//         #define B im8s
//         #define B_SIZE_0 160
//         #define B_SIZE_1 240
//         WHEN("A <- B convert copied") {
//             A.convertCopy(B);
//             THEN("converted correctly") {
//                 TEST_BLOCK
//             }
//         }
//         #undef A
//         #undef A_TYPE
//         #undef B
//         #undef B_SIZE_0
//         #undef B_SIZE_1

//         #define A imf
//         #define A_TYPE float
//         #define B im8s
//         #define B_SIZE_0 160
//         #define B_SIZE_1 240
//         WHEN("A <- B convert copied") {
//             A.convertCopy(B);
//             THEN("converted correctly") {
//                 TEST_BLOCK
//             }
//         }
//         #undef A
//         #undef A_TYPE
//         #undef B
//         #undef B_SIZE_0
//         #undef B_SIZE_1
        
//         #define A imd
//         #define A_TYPE double
//         #define B im8s
//         #define B_SIZE_0 160
//         #define B_SIZE_1 240
//         WHEN("A <- B convert copied") {
//             A.convertCopy(B);
//             THEN("converted correctly") {
//                 TEST_BLOCK
//             }
//         }
//         #undef A
//         #undef A_TYPE
//         #undef B
//         #undef B_SIZE_0
//         #undef B_SIZE_1
    }
}

// TODO: test convert copy
// TODO: test 1d multidimimage