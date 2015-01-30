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

    inline QString toString(const OLECHAR *str)
    {
#if defined(_WIN32) && !defined(OLE2ANSI)
    return QString::fromWCharArray(str);
#else
    return QString::fromLocal8Bit(str);
#endif
    }
}

#endif // WINDOWSUTIL_H
