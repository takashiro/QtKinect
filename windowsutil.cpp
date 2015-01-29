#include "windowsutil.h"

bool operator==(const QString &str1, const wchar_t *str2)
{
    QString::ConstIterator it1 = str1.begin();
    while (*str2 && it1 != str1.end()) {
        if (*str2 != *it1) {
            return false;
        }

        str2++;
        it1++;
    }

    return *str2 == '\0' && it1 == str1.end();
}
