#ifndef TNUIIMAGE_H
#define TNUIIMAGE_H

#include "tnuiimagebuffer.h"
#include "timage.h"

class TNuiImage : public TImage
{
    Q_OBJECT
public:
    explicit TNuiImage(QQuickItem *parent = 0);
    void setImage(const TNuiImageBuffer *frame);
};

#endif // TNUIIMAGE_H
