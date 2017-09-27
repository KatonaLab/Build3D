#include "VolumeMaterial.h"

using namespace Qt3DRender;

QTextureImageDataGeneratorPtr CustomDataTextureImage::dataGenerator() const
{
//    QTextureImageDataGeneratorPtr ptr = QTextureImageDataGeneratorPtr::create();
//    return ptr;

//    QTextureGenerator
//    QTextureImageDataGenerator gen(QTextureImageDataGenerator
    return nullptr;
}

void CustomDataTextureImage::setData(const uchar *data, int width, int height)
{

}

QTextureImageDataPtr CustomDataTextureImageGenerator::operator()()
{
    return nullptr;
}

bool CustomDataTextureImageGenerator::operator ==(const QTextureImageDataGenerator &other) const
{
    return false;
}
