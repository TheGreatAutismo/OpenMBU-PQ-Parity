//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------
#include "game/aiClient.h"
#include "core/realComp.h"
#include "math/mMatrix.h"
#include "game/player.h"
#include "game/moveManager.h"

#include "console/consoleInternal.h"

IMPLEMENT_CONOBJECT(AIClient);

/**
 * Constructor
 */
AIClient::AIClient() {
    mMoveMode = ModeStop;
    mMoveDestination.set(0.0f, 0.0f, 0.0f);
    mAimLocation.set(0.0f, 0.0f, 0.0f);
    mMoveSpeed = 1.0f;
    mMoveTolerance = 0.25f;

    // Clear the triggers
    for (int i = 0; i < MaxTriggerKeys; i++)
        mTriggers[i] = false;

    mAimToDestination = true;
    mTargetInLOS = false;

    mLocation.set(0.0f, 0.0f, 0.0f);
    mPlayer = NULL;
}

/**
 * Destructor
 */
AIClient::~AIClient() {
    // Blah
}

/**
 * Sets the object the bot is targeting
 *
 * @param targetObject The object to target
 */
void AIClient::setTargetObject(ShapeBase* targetObject) {
    if (!targetObject || !bool(mTargetObject) || targetObject->getId() != mTargetObject->getId())
        mTargetInLOS = false;

    mTargetObject = targetObject;
}

/**
 * Returns the target object
 *
 * @return Object bot is targeting
 */
S32 AIClient::getTargetObject() const {
    if (bool(mTargetObject))
        return mTargetObject->getId();
    else
        return -1;
}

/**
 * Sets the speed at which this AI moves
 *
 * @param speed Speed to move, default player was 10
 */
void AIClient::setMoveSpeed(F32 speed) {
    if (speed <= 0.0f)
        mMoveSpeed = 0.0f;
    else
        mMoveSpeed = getMin(1.0f, speed);
}

/**
 * Sets the movement mode for this AI
 *
 * @param mode Movement mode, see enum
 */
void AIClient::setMoveMode(S32 mode) {
    if (mode < 0 || mode >= ModeCount)
        mode = 0;

    if (mode != mMoveMode) {
        switch (mode) {
        case ModeStuck:
            throwCallback("onStuck");
            break;
        case ModeMove:
            if (mMoveMode == ModeStuck)
                throwCallback("onUnStuck");
            else
                throwCallback("onMove");
            break;
        case ModeStop:
            throwCallback("onStop");
            break;
        }
    }

    mMoveMode = mode;
}

/**
 * Sets how far away from the move location is considered
 * "on target"
 *
 * @param tolerance Movement tolerance for error
 */
void AIClient::setMoveTolerance(const F32 tolerance) {
    mMoveTolerance = getMax(0.1f, tolerance);
}

/**
 * Sets the location for the bot to run to
 *
 * @param location Point to run to
 */
void AIClient::setMoveDestination(const Point3F& location) {
    // Ok, here's the story...we're going to aim where we are going UNLESS told otherwise
    if (mAimToDestination) {
        mAimLocation = location;
        mAimLocation.z = 0.0f;
    }

    mMoveDestination = location;
}

/**
 * Sets the location for the bot to aim at
 *
 * @param location Point to aim at
 */
void AIClient::setAimLocation(const Point3F& location) {
    mAimLocation = location;
    mAimToDestination = false;
}

/**
 * Clears the aim location and sets it to the bot's
 * current destination so he looks where he's going
 */
void AIClient::clearAim() {
    mAimLocation = Point3F(0.0f, 0.0f, 0.0f);
    mAimToDestination = true;
}

/**
 * This method gets the move list for an object, in the case
 * of the AI, it actually calculates the moves, and then
 * sends them down the pipe.
 *
 * @param movePtr Pointer to move the move list into
 * @param numMoves Number of moves in the move list
 */
U32 AIClient::getMoveList(Move** movePtr, U32* numMoves) {
    //initialize the move structure and return pointers
    mMove = NullMove;
    *movePtr = &mMove;
    *numMoves = 1;

    // Check if we got a player
    mPlayer = NULL;
    mPlayer = static_cast<Player*>(getControlObject());

    // We got a something controling us?
    if (!mPlayer)
        return *numMoves;


    // What is The Matrix?
    MatrixF moveMatrix;
    moveMatrix.set(EulerF(0, 0, 0));
    moveMatrix.setColumn(3, Point3F(0, 0, 0));
    moveMatrix.transpose();

    // Position / rotation variables
    F32 curYaw, curPitch;
    F32 newYaw, newPitch;
    F32 xDiff, yDiff, zDiff;


    F32 moveSpeed = mMoveSpeed;

    switch (mMoveMode) {

    case ModeStop:
        return *numMoves;     // Stop means no action
        break;

    case ModeStuck:
        // Fall through, so we still try to move
    case ModeMove:

        // Get my location
        MatrixF const& myTransform = mPlayer->getTransform();
        myTransform.getColumn(3, &mLocation);

        // Set rotation variables
        Point3F rotation = mPlayer->getRotation();
        Point3F headRotation = mPlayer->getHeadRotation();
        curYaw = rotation.z;
        curPitch = headRotation.x;
        xDiff = mAimLocation.x - mLocation.x;
        yDiff = mAimLocation.y - mLocation.y;

        // first do Yaw
        if (!isZero(xDiff) || !isZero(yDiff)) {
            // use the cur yaw between -Pi and Pi
            while (curYaw > M_2PI)
                curYaw -= M_2PI;
            while (curYaw < -M_2PI)
                curYaw += M_2PI;

            // find the new yaw
            newYaw = mAtan(xDiff, yDiff);

            // find the yaw diff 
            F32 yawDiff = newYaw - curYaw;

            // make it between 0 and 2PI
            if (yawDiff < 0.0f)
                yawDiff += M_2PI;
            else if (yawDiff >= M_2PI)
                yawDiff -= M_2PI;

            // now make sure we take the short way around the circle
            if (yawDiff > M_PI)
                yawDiff -= M_2PI;
            else if (yawDiff < -M_PI)
                yawDiff += M_2PI;

            mMove.yaw = yawDiff;

            // set up the movement matrix
            moveMatrix.set(EulerF(0, 0, newYaw));
        }
        else
            moveMatrix.set(EulerF(0, 0, curYaw));

        // next do pitch
        F32 horzDist = Point2F(mAimLocation.x, mAimLocation.y).len();

        if (!isZero(horzDist)) {
            //we shoot from the gun, not the eye...
            F32 vertDist = mAimLocation.z;

            newPitch = mAtan(horzDist, vertDist) - (M_PI / 2.0f);

            F32 pitchDiff = newPitch - curPitch;
            mMove.pitch = pitchDiff;
        }

        // finally, mMove towards mMoveDestination
        xDiff = mMoveDestination.x - mLocation.x;
        yDiff = mMoveDestination.y - mLocation.y;


        // Check if we should mMove, or if we are 'close enough'
        if (((mFabs(xDiff) > mMoveTolerance) ||
            (mFabs(yDiff) > mMoveTolerance)) && (!isZero(mMoveSpeed)))
        {
            if (isZero(xDiff))
                mMove.y = (mLocation.y > mMoveDestination.y ? -moveSpeed : moveSpeed);
            else if (isZero(yDiff))
                mMove.x = (mLocation.x > mMoveDestination.x ? -moveSpeed : moveSpeed);
            else if (mFabs(xDiff) > mFabs(yDiff)) {
                F32 value = mFabs(yDiff / xDiff) * mMoveSpeed;
                mMove.y = (mLocation.y > mMoveDestination.y ? -value : value);
                mMove.x = (mLocation.x > mMoveDestination.x ? -moveSpeed : moveSpeed);
            }
            else {
                F32 value = mFabs(xDiff / yDiff) * mMoveSpeed;
                mMove.x = (mLocation.x > mMoveDestination.x ? -value : value);
                mMove.y = (mLocation.y > mMoveDestination.y ? -moveSpeed : moveSpeed);
            }

            //now multiply the mMove vector by the transpose of the object rotation matrix
            moveMatrix.transpose();
            Point3F newMove;
            moveMatrix.mulP(Point3F(mMove.x, mMove.y, 0), &newMove);

            //and sub the result back in the mMove structure
            mMove.x = newMove.x;
            mMove.y = newMove.y;

            // We should check to see if we are stuck...
            if (mLocation.x == mLastLocation.x &&
                mLocation.y == mLastLocation.y &&
                mLocation.z == mLastLocation.z) {

                // We're stuck...probably
                setMoveMode(ModeStuck);
            }
            else
                setMoveMode(ModeMove);
        }
        else {
            // Ok, we are close enough, lets stop

            // setMoveMode( ModeStop ); // DON'T use this, it'll throw the wrong callback
            mMoveMode = ModeStop;
            throwCallback("onReachDestination"); // Callback

        }
        break;
    }

    // Test for target location in sight
    RayInfo dummy;
    Point3F targetLoc = mMoveDestination; // Change this

    if (mPlayer) {
        if (!mPlayer->getContainer()->castRay(mLocation, targetLoc, InteriorObjectType |
            StaticShapeObjectType | StaticObjectType |
            AtlasObjectType | TerrainObjectType, &dummy)) {
            if (!mTargetInLOS)
                throwCallback("onTargetEnterLOS");
        }
        else {
            if (mTargetInLOS)
                throwCallback("onTargetExitLOS");

        }
    }

    // Copy over the trigger status
    for (int i = 0; i < MaxTriggerKeys; i++) {
        mMove.trigger[i] = mTriggers[i];
        mTriggers[i] = false;
    }

    return *numMoves;
}

/**
 * This method is just called to stop the bots from running amuck
 * while the mission cycles
 */
void AIClient::missionCycleCleanup() {
    setMoveMode(ModeStop);
}


/**
 * Utility function to throw callbacks
 */
void AIClient::throwCallback(const char* name) {
    Con::executef(this, 2, name);
}

/**
 * What gets called when this gets created, different from constructor
 */
void AIClient::onAdd(const char* nameSpace) {

    // This doesn't work...
    //
    if (dStrcmp(nameSpace, mNameSpace->mName)) {
        Con::linkNamespaces(mNameSpace->mName, nameSpace);
        mNameSpace = Con::lookupNamespace(nameSpace);
    }

    throwCallback("onAdd");
}

// --------------------------------------------------------------------------------------------
// Console Functions
// --------------------------------------------------------------------------------------------

/**
 * Sets the move speed for an AI object
 */
ConsoleMethod(AIConnection, setMoveSpeed, void, 3, 3, "ai.setMoveSpeed( float );") {
    AIClient* ai = static_cast<AIClient*>(object);
    ai->setMoveSpeed(dAtof(argv[2]));
}

/**
 * Stops all AI movement, halt!
 */
ConsoleMethod(AIConnection, stop, void, 2, 2, "ai.stop();") {
    AIClient* ai = static_cast<AIClient*>(object);
    ai->setMoveMode(AIClient::ModeStop);
}

/**
 * Tells the AI to aim at the location provided
 */
ConsoleMethod(AIConnection, setAimLocation, void, 3, 3, "ai.setAimLocation( x y z );") {
    AIClient* ai = static_cast<AIClient*>(object);
    Point3F v(0.0f, 0.0f, 0.0f);
    dSscanf(argv[2], "%f %f %f", &v.x, &v.y, &v.z);

    ai->setAimLocation(v);
}

/**
 * Tells the AI to move to the location provided
 */
ConsoleMethod(AIConnection, setMoveDestination, void, 3, 3, "ai.setMoveDestination( x y z );") {
    AIClient* ai = static_cast<AIClient*>(object);
    Point3F v(0.0f, 0.0f, 0.0f);
    dSscanf(argv[2], "%f %f", &v.x, &v.y);

    ai->setMoveDestination(v);
}

/**
 * Returns the point the AI is aiming at
 */
ConsoleMethod(AIConnection, getAimLocation, const char*, 2, 2, "ai.getAimLocation();") {
    AIClient* ai = static_cast<AIClient*>(object);
    Point3F aimPoint = ai->getAimLocation();

    char* returnBuffer = Con::getReturnBuffer(256);
    dSprintf(returnBuffer, 256, "%f %f %f", aimPoint.x, aimPoint.y, aimPoint.z);

    return returnBuffer;
}

/**
 * Returns the point the AI is set to move to
 */
ConsoleMethod(AIConnection, getMoveDestination, const char*, 2, 2, "ai.getMoveDestination();") {
    AIClient* ai = static_cast<AIClient*>(object);
    Point3F movePoint = ai->getMoveDestination();

    char* returnBuffer = Con::getReturnBuffer(256);
    dSprintf(returnBuffer, 256, "%f %f %f", movePoint.x, movePoint.y, movePoint.z);

    return returnBuffer;
}

/**
 * Sets the bots target object
 */
ConsoleMethod(AIConnection, setTargetObject, void, 3, 3, "ai.setTargetObject( obj );") {
    AIClient* ai = static_cast<AIClient*>(object);

    // Find the target
    ShapeBase* targetObject;
    if (Sim::findObject(argv[2], targetObject))
        ai->setTargetObject(targetObject);
    else
        ai->setTargetObject(NULL);
}

/**
 * Gets the object the AI is targeting
 */
ConsoleMethod(AIConnection, getTargetObject, S32, 2, 2, "ai.getTargetObject();") {
    AIClient* ai = static_cast<AIClient*>(object);

    return ai->getTargetObject();
}

/**
 * Tells the bot the mission is cycling
 */
ConsoleMethod(AIConnection, missionCycleCleanup, void, 2, 2, "ai.missionCycleCleanup();") {
    AIClient* ai = static_cast<AIClient*>(object);
    ai->missionCycleCleanup();
}

/**
 * Sets the AI to run mode
 */
ConsoleMethod(AIConnection, move, void, 2, 2, "ai.move();") {
    AIClient* ai = static_cast<AIClient*>(object);
    ai->setMoveMode(AIClient::ModeMove);
}

/**
 * Gets the AI's location in the world
 */
ConsoleMethod(AIConnection, getLocation, const char*, 2, 2, "ai.getLocation();") {
    AIClient* ai = static_cast<AIClient*>(object);
    Point3F locPoint = ai->getLocation();

    char* returnBuffer = Con::getReturnBuffer(256);
    dSprintf(returnBuffer, 256, "%f %f %f", locPoint.x, locPoint.y, locPoint.z);

    return returnBuffer;
}

/**
 * Adds an AI Player to the game
 */
ConsoleFunction(aiAddPlayer, S32, 2, 3, "aiAddPlayer( 'playerName'[, 'AIClassType'] );") {
    // Create the player
    AIClient* aiPlayer = new AIClient();
    aiPlayer->registerObject();
    aiPlayer->setGhostFrom(false);
    aiPlayer->setGhostTo(false);
    aiPlayer->setSendingEvents(false);
    aiPlayer->setTranslatesStrings(true);
    aiPlayer->setEstablished();

    // Add the connection to the client group
    SimGroup* g = Sim::getClientGroup();
    g->addObject(aiPlayer);

    char* name = new char[dStrlen(argv[1]) + 1];
    char* ns = new char[dStrlen(argv[2]) + 1];

    dStrcpy(name, argv[1]);
    dStrcpy(ns, argv[2]);

    // Execute the connect console function, this is the same 
    // onConnect function invoked for normal client connections
    Con::executef(aiPlayer, 2, "onConnect", name);

    // Now execute the onAdd command and feed it the namespace
    if (argc > 2)
        aiPlayer->onAdd(ns);
    else
        aiPlayer->onAdd("AIClient");

    return aiPlayer->getId();
}


/**
 * Tells the AI to move forward 100 units...TEST FXN
 */
ConsoleMethod(AIConnection, moveForward, void, 2, 2, "ai.moveForward();") {

    AIClient* ai = static_cast<AIClient*>(object);
    ShapeBase* player = ai->getControlObject();
    Point3F location;
    MatrixF const& myTransform = player->getTransform();
    myTransform.getColumn(3, &location);

    location.y += 100.0f;

    ai->setMoveDestination(location);
} // *** /TEST FXN
