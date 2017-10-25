import Qt3D.Core 2.0
import Qt3D.Render 2.0
import koki.katonalab.a3dc 1.0

// TODO: maybe restructuring need, this class is not needed
Material {
    id: root

    property alias backfaceMap: effect.backfaceMap
    property alias volumeParameter0: effect.volumeParameter0
    property alias volumeParameter1: effect.volumeParameter1
    property alias volumeParameter2: effect.volumeParameter2
    property alias volumeParameter3: effect.volumeParameter3

    effect: VolumeEffect {
        id: effect
    }
}