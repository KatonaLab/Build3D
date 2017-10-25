import QtQuick 2.8
import Qt3D.Core 2.0
import koki.katonalab.a3dc 1.0

// TODO: this class should be refactored, the whole 
// render-every-volume-in-one-material/effect idea should be rethinked
Entity {
    property VolumetricTexture texture: VolumetricTexture {}
    property color color
    property vector4d cutParams
}