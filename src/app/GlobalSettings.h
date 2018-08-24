#ifndef _app_GlobalSettings_h_
#define _app_GlobalSettings_h_

#include <QDir>

struct GlobalSettings {
    static QDir modulePath;
    static bool editorMode;
};

#endif
