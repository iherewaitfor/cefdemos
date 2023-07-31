#ifndef REMOTEOBJECTSTRUCTS_H
#define REMOTEOBJECTSTRUCTS_H

#include <QtCore>

typedef struct RemoteObjectStruct
{
    QString objName;
    QString parentName;
    friend QDataStream& operator << (QDataStream& dataStream, const RemoteObjectStruct& object)
    {
        dataStream << object.objName;
        dataStream << object.parentName;
        return dataStream;
    }

    friend QDataStream& operator >> (QDataStream& dataStream, RemoteObjectStruct& object)
    {
        dataStream >> object.objName;
        dataStream >> object.parentName;
        return dataStream;
    }
}RemoteObjectStruct;
Q_DECLARE_METATYPE(RemoteObjectStruct);
#endif
