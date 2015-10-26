#ifndef TMESH_H
#define TMESH_H

#include <QMesh>

class TMesh : public Qt3D::QMesh
{
    Q_OBJECT

    Q_PROPERTY(QVector3D center READ center)
    Q_PROPERTY(float xExtent READ xExtent CONSTANT)
    Q_PROPERTY(float yExtent READ yExtent CONSTANT)
    Q_PROPERTY(float zExtent READ zExtent CONSTANT)

public:
    explicit TMesh(QNode *parent = 0);

    QVector3D center() const;
    Qt3D::QAxisAlignedBoundingBox boundingBox() const;

    float xExtent() const;
    float yExtent() const;
    float zExtent() const;
};

#endif // TMESH_H
