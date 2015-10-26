#include "tmesh.h"

TMesh::TMesh(Qt3D::QNode *parent)
    : QMesh(parent)
{
}

QVector3D TMesh::center() const
{
    Qt3D::QAbstractMeshFunctorPtr functor = meshFunctor();
    Qt3D::QMeshDataPtr data = (*functor)();
    return data->boundingBox().center();
}

Qt3D::QAxisAlignedBoundingBox TMesh::boundingBox() const
{
    Qt3D::QAbstractMeshFunctorPtr functor = meshFunctor();
    Qt3D::QMeshDataPtr data = (*functor)();
    return data->boundingBox();
}

float TMesh::xExtent() const
{
    Qt3D::QAbstractMeshFunctorPtr functor = meshFunctor();
    Qt3D::QMeshDataPtr data = (*functor)();
    return data->boundingBox().xExtent();
}

float TMesh::yExtent() const
{
    Qt3D::QAbstractMeshFunctorPtr functor = meshFunctor();
    Qt3D::QMeshDataPtr data = (*functor)();
    return data->boundingBox().yExtent();
}

float TMesh::zExtent() const
{
    Qt3D::QAbstractMeshFunctorPtr functor = meshFunctor();
    Qt3D::QMeshDataPtr data = (*functor)();
    return data->boundingBox().zExtent();
}
