//-----------------------------------------------------------------------------
// Torque Shader Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "console/simBase.h"
#include "platform/event.h"
#include "sim/netConnection.h"
#include "core/bitStream.h"
#include "sim/netObject.h"

class SimpleMessageEvent : public NetEvent
{
    typedef NetEvent Parent;
    char* msg;
public:
    SimpleMessageEvent(const char* message = NULL)
    {
        if (message)
            msg = dStrdup(message);
        else
            msg = NULL;
    }
    ~SimpleMessageEvent()
    {
        dFree(msg);
    }

    virtual void pack(NetConnection* /*ps*/, BitStream* bstream)
    {
        bstream->writeString(msg);
    }
    virtual void write(NetConnection*, BitStream* bstream)
    {
        bstream->writeString(msg);
    }
    virtual void unpack(NetConnection* /*ps*/, BitStream* bstream)
    {
        char buf[256]; bstream->readString(buf); msg = dStrdup(buf);
    }
    virtual void process(NetConnection*)
    {
        Con::printf("RMSG %d  %s", mSourceId, msg);
    }

    DECLARE_CONOBJECT(SimpleMessageEvent);
};

IMPLEMENT_CO_NETEVENT_V1(SimpleMessageEvent);

class SimpleNetObject : public NetObject
{
    typedef NetObject Parent;
public:
    char message[256];
    SimpleNetObject()
    {
        mNetFlags.set(ScopeAlways | Ghostable);
        dStrcpy(message, "Hello World!");
    }
    U32 packUpdate(NetConnection* conn, U32 mask, BitStream* stream)
    {
        stream->writeString(message);
        return 0;
    }
    void unpackUpdate(NetConnection* conn, BitStream* stream)
    {
        stream->readString(message);
        Con::printf("Got message: %s", message);
    }
    void setMessage(const char* msg)
    {
        setMaskBits(1);
        dStrcpy(message, msg);
    }

    DECLARE_CONOBJECT(SimpleNetObject);
};

IMPLEMENT_CO_NETOBJECT_V1(SimpleNetObject);

ConsoleMethod(SimpleNetObject, setMessage, void, 3, 3, "(string msg)")
{
    object->setMessage(argv[2]);
}

ConsoleFunction(msg, void, 3, 3, "(NetConnection id, string message)"
    "Send a SimpleNetObject message to the specified connection.")
{
    NetConnection* con = (NetConnection*)Sim::findObject(argv[1]);
    if (con)
        con->postNetEvent(new SimpleMessageEvent(argv[2]));
}
