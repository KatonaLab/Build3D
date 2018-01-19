#include "catch.hpp"

using namespace core::multidim_image_platform;
using namespace std;

SCENARIO("multidim image basic usage", "[core/multidim_image_platform]")
{
    GIVEN("two images, an empty and a 2d image with some content") {
        MultiDimImage emptyImage;
        REQUIRE(emptyImage.size() == 0);
        REQUIRE(emptyImage.byteSize() == 0);
        REQUIRE(emptyImage.dims() == 0);
        REQUIRE_THROWS(emptyImage.dim(0));
        REQUIRE_THROWS(emptyImage.dim(23));
        REQUIRE(emptyImage.empty() == true);

        MultiDimImage image2d({16, 32});
        
        REQUIRE(image2d.size() == 16 * 32);
        REQUIRE(image2d.byteSize() == 16 * 32 * 4);
        REQUIRE(image2d.dims() == 2);
        REQUIRE(image2d.dim(0) == 16);
        REQUIRE(image2d.dim(1) == 32);
        REQUIRE_THROWS(image2d.dim(2));
        REQUIRE(image2d.empty() == false);
        REQUIRE(image2d.at(0, 0) == 0);
        image2d.at(0, 0) = 42;
        REQUIRE(image2d.at(0, 0) == 42);

        WHEN("the image2d is assigned to the empty one") {
            emptyImage = image2d;
            image2d.at(0, 0) = 0;
            THEN("it is truly copied") {
                REQUIRE(emptyImage.size() == 16 * 32);
                REQUIRE(emptyImage.byteSize() == 16 * 32 * 4);
                REQUIRE(emptyImage.dims() == 2);
                REQUIRE(emptyImage.dim(0) == 16);
                REQUIRE(emptyImage.dim(1) == 32);
                REQUIRE_THROWS(emptyImage.dim(2));
                REQUIRE(emptyImage.empty() == false);
                REQUIRE(emptyImage.at(0, 0) == 42);
            }
        }

        WHEN("a new image is copy constructed from image2d") {
            image2d.at(0, 0) = 17;
            MultiDimImage newImage(image2d);
            image2d.at(0, 0) = 0;
            THEN("it is truly copied") {
                REQUIRE(newImage.size() == 16 * 32);
                REQUIRE(newImage.byteSize() == 16 * 32 * 4);
                REQUIRE(newImage.dims() == 2);
                REQUIRE(newImage.dim(0) == 16);
                REQUIRE(newImage.dim(1) == 32);
                REQUIRE_THROWS(newImage.dim(2));
                REQUIRE(newImage.empty() == false);
                REQUIRE(newImage.at(0, 0) == 17);
            }
        }
    }

    // TODO: type check
    // TODO: huge allocation test, copy, assign, dctr
    // TODO: clear
    // TODO: custom(?) meta data, dim name, (channel name)
    // TODO: create multidim data by copying some dimensions
    // TODO: view(?)
}
