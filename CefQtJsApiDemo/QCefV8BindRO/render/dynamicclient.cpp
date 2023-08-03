#include "dynamicclient.h"

// constructor
DynamicClient::DynamicClient(QSharedPointer<QRemoteObjectDynamicReplica> ptr) :
    QObject(nullptr), clientSwitchState(false), reptr(ptr), m_binitConnection(false)
{
    //connect signal for replica valid changed with signal slot initialization
    QObject::connect(reptr.data(), SIGNAL(initialized()), this, SLOT(initConnection_slot()), Qt::UniqueConnection);
    QObject::connect(reptr.data(), SIGNAL(stateChanged(QRemoteObjectReplica::State, QRemoteObjectReplica::State)), 
        this, SLOT(stateChanged_slot(QRemoteObjectReplica::State, QRemoteObjectReplica::State)));
}

//destructor
DynamicClient::~DynamicClient()
{

}

// Function to initialize connections between slots and signals
void DynamicClient::initConnection_slot()
{
    if (m_binitConnection) {
        return;
    }
    m_binitConnection = true;
}

//monitor the replica disconnect with the source
void DynamicClient::stateChanged_slot(QRemoteObjectReplica::State state, QRemoteObjectReplica::State oldState){
    
    if (QRemoteObjectReplica::State::Suspect == state && QRemoteObjectReplica::State::Valid == oldState) {
        // source断开了
        // to do 
    }
    else if (QRemoteObjectReplica::State::Valid == state && QRemoteObjectReplica::State::Suspect == oldState) {
        //重新连接上了
        // to do 
    }
}

QSharedPointer<QRemoteObjectDynamicReplica> DynamicClient::getReplica() {
    return reptr;
}