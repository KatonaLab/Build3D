import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.0
import koki.katonalab.a3dc 1.0


// TODO: maybe restructuring need, this class is not needed
Material {
    id: root

    property Texture2D backfaceMap

    effect: VolumeEffect {
        backfaceMap: root.backfaceMap
    }
}