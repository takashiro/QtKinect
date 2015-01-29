#ifndef WINDOWSUTIL_H
#define WINDOWSUTIL_H

#include <wtypes.h>

#include <QString>
#include <QVector>

namespace WindowsUtil {

    inline const OLECHAR *ToOLECharArray(const QString &str)
    {
    #if defined(_WIN32) && !defined(OLE2ANSI)
        return str.toStdWString().data();
    #else
        return str.toStdString().data();
    #endif
    }

}

bool operator==(const QString &str1, const wchar_t *str2);

#endif // WINDOWSUTIL_H
