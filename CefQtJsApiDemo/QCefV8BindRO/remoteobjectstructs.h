#ifndef REMOTEOBJECTSTRUCTS_H
#define REMOTEOBJECTSTRUCTS_H

#include <QtCore>

typedef struct RemoteObjectStruct
{
    QString objName;
    QString parentName;
    QString v8Name;
    friend QDataStream& operator << (QDataStream& dataStream, const RemoteObjectStruct& object)
    {
        dataStream << object.objName;
        dataStream << object.parentName;
        dataStream << object.v8Name;
        return dataStream;
    }

    friend QDataStream& operator >> (QDataStream& dataStream, RemoteObjectStruct& object)
    {
        dataStream >> object.objName;
        dataStream >> object.parentName;
        dataStream >> object.v8Name;
        return dataStream;
    }
}RemoteObjectStruct;
Q_DECLARE_METATYPE(RemoteObjectStruct);
#endif
