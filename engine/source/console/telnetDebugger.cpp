//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "console/console.h"
#include "console/telnetDebugger.h"
#include "platform/event.h"
#include "core/stringTable.h"
#include "console/consoleInternal.h"
#include "console/ast.h"
#include "console/compiler.h"
#include "platform/gameInterface.h"


//
// Enhanced TelnetDebugger for Torsion
// http://www.sickheadgames.com/torsion
//
//
// Debugger commands:
//
// CEVAL console line - evaluate the console line
//    output: none
//
// BRKVARSET varName passct expr - NOT IMPLEMENTED!
//    output: none
//
// BRKVARCLR varName - NOT IMPLEMENTED!
//    output: none
//
// BRKSET file line clear passct expr - set a breakpoint on the file,line
//        it must pass passct times for it to break and if clear is true, it
//        clears when hit
//    output: 
//
// BRKNEXT - stop execution at the next breakable line.
//    output: none
//
// BRKCLR file line - clear a breakpoint on the file,line
//    output: none
//
// BRKCLRALL - clear all breakpoints
//    output: none
//
// CONTINUE - continue execution
//    output: RUNNING
//
// STEPIN - run until next statement
//    output: RUNNING
//
// STEPOVER - run until next break <= current frame
//    output: RUNNING
//
// STEPOUT - run until next break <= current frame - 1
//    output: RUNNING
//
// EVAL tag frame expr - evaluate the expr in the console, on the frame'th stack frame
//    output: EVALOUT tag exprResult
//
// FILELIST - list script files loaded
//    output: FILELISTOUT file1 file2 file3 file4 ...
//
// BREAKLIST file - get a list of breakpoint-able lines in the file
//    output: BREAKLISTOUT file skipBreakPairs skiplinecount breaklinecount skiplinecount breaklinecount ...
//
//
// Other output:
//
// BREAK file1 line1 function1 file2 line2 function2 ... - Sent when the debugger hits a 
//          breakpoint.  It lists out one file/line/function triplet for each stack level.
//          The first one is the top of the stack.
//
// COUT console-output - echo of console output from engine
//
// BRKMOV file line newline - sent when a breakpoint is moved to a breakable line.
//
// BRKCLR file line - sent when a breakpoint cannot be moved to a breakable line on the client.
//


ConsoleFunction(dbgSetParameters, void, 3, 4, "(int port, string password, bool waitForClient)"
    "Open a debug server port on the specified port, requiring the specified password, "
    "and optionally waiting for the debug client to connect.")
{
    if (TelDebugger)
        TelDebugger->setDebugParameters(dAtoi(argv[1]), argv[2], argc > 3 ? dAtob(argv[3]) : false);
}

static void debuggerConsumer(ConsoleLogEntry::Level level, const char* line)
{
    level;
    if (TelDebugger)
        TelDebugger->processConsoleLine(line);
}

TelnetDebugger::TelnetDebugger()
{
    Con::addConsumer(debuggerConsumer);

    mAcceptPort = -1;
    mAcceptSocket = InvalidSocket;
    mDebugSocket = InvalidSocket;

    mState = NotConnected;
    mCurPos = 0;

    mBreakpoints = NULL;
    mBreakOnNextStatement = false;
    mStackPopBreakIndex = -1;
    mProgramPaused = false;
    mWaitForClient = false;

    // Add the version number in a global so that
    // scripts can detect the presence of the
    // "enhanced" debugger features.
    Con::evaluatef("$dbgVersion = %d;", Version);
}

TelnetDebugger::Breakpoint** TelnetDebugger::findBreakpoint(StringTableEntry fileName, S32 lineNumber)
{
    Breakpoint** walk = &mBreakpoints;
    Breakpoint* cur;
    while ((cur = *walk) != NULL)
    {
        if (cur->fileName == fileName && cur->lineNumber == U32(lineNumber))
            return walk;
        walk = &cur->next;
    }
    return NULL;
}


TelnetDebugger::~TelnetDebugger()
{
    Con::removeConsumer(debuggerConsumer);

    if (mAcceptSocket != InvalidSocket)
        Net::closeSocket(mAcceptSocket);
    if (mDebugSocket != InvalidSocket)
        Net::closeSocket(mDebugSocket);
}

TelnetDebugger* TelDebugger = NULL;

void TelnetDebugger::create()
{
    TelDebugger = new TelnetDebugger;
}

void TelnetDebugger::destroy()
{
    delete TelDebugger;
    TelDebugger = NULL;
}

void TelnetDebugger::send(const char* str)
{
    Net::send(mDebugSocket, (const unsigned char*)str, dStrlen(str));
}

void TelnetDebugger::setDebugParameters(S32 port, const char* password, bool waitForClient)
{
    // Don't bail if same port... we might just be wanting to change
    // the password.
 //   if(port == mAcceptPort)
 //      return;

    if (mAcceptSocket != InvalidSocket)
    {
        Net::closeSocket(mAcceptSocket);
        mAcceptSocket = InvalidSocket;
    }
    mAcceptPort = port;
    if (mAcceptPort != -1 && mAcceptPort != 0)
    {
        mAcceptSocket = Net::openSocket();
        Net::bind(mAcceptSocket, mAcceptPort);
        Net::listen(mAcceptSocket, 4);

        Net::setBlocking(mAcceptSocket, false);
    }
    dStrncpy(mDebuggerPassword, password, PasswordMaxLength);

    mWaitForClient = waitForClient;
    if (!mWaitForClient)
        return;

    // Wait for the client to fully connect.
    while (mState != Connected)
    {
        Platform::sleep(10);
        process();
    }

}

void TelnetDebugger::processConsoleLine(const char* consoleLine)
{
    if (mState != NotConnected)
    {
        send("COUT ");
        send(consoleLine);
        send("\r\n");
    }
}

void TelnetDebugger::process()
{
    NetAddress address;

    if (mAcceptSocket != InvalidSocket)
    {
        // ok, see if we have any new connections:
        NetSocket newConnection;
        newConnection = Net::accept(mAcceptSocket, &address);

        if (newConnection != InvalidSocket && mDebugSocket == InvalidSocket)
        {
            Con::printf("Debugger connection from %i.%i.%i.%i",
                address.netNum[0], address.netNum[1], address.netNum[2], address.netNum[3]);

            mState = PasswordTry;
            mDebugSocket = newConnection;

            Net::setBlocking(newConnection, false);
        }
        else if (newConnection != InvalidSocket)
            Net::closeSocket(newConnection);
    }
    // see if we have any input to process...

    if (mDebugSocket == InvalidSocket)
        return;

    checkDebugRecv();
    if (mDebugSocket == InvalidSocket)
        removeAllBreakpoints();
}

void TelnetDebugger::checkDebugRecv()
{
    for (;;)
    {
        // Process all the complete commands in the buffer.
        while (mCurPos > 0)
        {
            // Remove leading whitespace.
            while (mCurPos > 0 && (mLineBuffer[0] == 0 || mLineBuffer[0] == '\r' || mLineBuffer[0] == '\n'))
            {
                mCurPos--;
                dMemmove(mLineBuffer, mLineBuffer + 1, mCurPos);
            }

            // Look for a complete command.
            bool gotCmd = false;
            for (S32 i = 0; i < mCurPos; i++)
            {
                if (mLineBuffer[i] == 0)
                    mLineBuffer[i] = '_';

                else if (mLineBuffer[i] == '\r' || mLineBuffer[i] == '\n')
                {
                    // Send this command to be processed.
                    mLineBuffer[i] = '\n';
                    processLineBuffer(i + 1);

                    // Remove the command from the buffer.
                    mCurPos -= i + 1;
                    dMemmove(mLineBuffer, mLineBuffer + i + 1, mCurPos);

                    gotCmd = true;
                    break;
                }
            }

            // If we didn't find a command in this pass
            // then we have an incomplete buffer.
            if (!gotCmd)
                break;
        }

        // found no <CR> or <LF>
        if (mCurPos == MaxCommandSize) // this shouldn't happen
        {
            Net::closeSocket(mDebugSocket);
            mDebugSocket = InvalidSocket;
            mState = NotConnected;
            return;
        }

        S32 numBytes;
        Net::Error err = Net::recv(mDebugSocket, (unsigned char*)(mLineBuffer + mCurPos), MaxCommandSize - mCurPos, &numBytes);

        if ((err != Net::NoError && err != Net::WouldBlock) || numBytes == 0)
        {
            Net::closeSocket(mDebugSocket);
            mDebugSocket = InvalidSocket;
            mState = NotConnected;
            return;
        }
        if (err == Net::WouldBlock)
            return;

        mCurPos += numBytes;
    }
}

void TelnetDebugger::executionStopped(CodeBlock* code, U32 lineNumber)
{
    if (mProgramPaused)
        return;

    if (mBreakOnNextStatement)
    {
        setBreakOnNextStatement(false);
        breakProcess();
        return;
    }

    Breakpoint** bp = findBreakpoint(code->name, lineNumber);
    if (!bp)
        return;

    Breakpoint* brk = *bp;
    mProgramPaused = true;
    Con::evaluatef("$Debug::result = %s;", brk->testExpression);
    if (Con::getBoolVariable("$Debug::result"))
    {
        brk->curCount++;
        if (brk->curCount >= brk->passCount)
        {
            brk->curCount = 0;
            if (brk->clearOnHit)
                removeBreakpoint(code->name, lineNumber);
            breakProcess();
        }
    }
    mProgramPaused = false;
}

void TelnetDebugger::pushStackFrame()
{
    if (mState == NotConnected)
        return;

    if (mBreakOnNextStatement && mStackPopBreakIndex > -1 &&
        gEvalState.stack.size() > mStackPopBreakIndex)
        setBreakOnNextStatement(false);
}

void TelnetDebugger::popStackFrame()
{
    if (mState == NotConnected)
        return;

    if (mStackPopBreakIndex > -1 && gEvalState.stack.size() - 1 <= mStackPopBreakIndex)
        setBreakOnNextStatement(true);
}

void TelnetDebugger::breakProcess()
{
    mProgramPaused = true;

    // echo out the break
    send("BREAK");
    char buffer[MaxCommandSize];
    char scope[MaxCommandSize];

    for (S32 i = (S32)gEvalState.stack.size() - 1; i >= 0; i--)
    {
        CodeBlock* code = gEvalState.stack[i]->code;
        const char* file = code->name;
        if ((!file) || (!file[0]))
            file = "<none>";

        Namespace* ns = gEvalState.stack[i]->scopeNamespace;
        scope[0] = 0;
        if (ns) {

            if (ns->mParent && ns->mParent->mPackage && ns->mParent->mPackage[0]) {
                dStrcat(scope, ns->mParent->mPackage);
                dStrcat(scope, "::");
            }
            if (ns->mName && ns->mName[0]) {
                dStrcat(scope, ns->mName);
                dStrcat(scope, "::");
            }
        }

        const char* function = gEvalState.stack[i]->scopeName;
        if ((!function) || (!function[0]))
            function = "<none>";
        dStrcat(scope, function);

        U32 line, inst;
        U32 ip = gEvalState.stack[i]->ip;
        code->findBreakLine(ip, line, inst);
        dSprintf(buffer, MaxCommandSize, " %s %d %s", file, line, scope);
        send(buffer);
    }

    send("\r\n");

    while (mProgramPaused)
    {
        Platform::sleep(10);
        checkDebugRecv();
        if (mDebugSocket == InvalidSocket)
        {
            mProgramPaused = false;
            removeAllBreakpoints();
            debugContinue();
            return;
        }
    }
}

void TelnetDebugger::processLineBuffer(S32 cmdLen)
{
    if (mState == PasswordTry)
    {
        if (dStrncmp(mLineBuffer, mDebuggerPassword, cmdLen - 1))
        {
            // failed password:
            send("PASS WrongPassword.\r\n");
            Net::closeSocket(mDebugSocket);
            mDebugSocket = InvalidSocket;
            mState = NotConnected;
        }
        else
        {
            send("PASS Connected.\r\n");
            mState = mWaitForClient ? Initialize : Connected;
        }

        return;
    }
    else
    {
        char evalBuffer[MaxCommandSize];
        char varBuffer[MaxCommandSize];
        char fileBuffer[MaxCommandSize];
        char clear[MaxCommandSize];
        S32 passCount, line, frame;

        if (dSscanf(mLineBuffer, "CEVAL %[^\n]", evalBuffer) == 1)
        {
            ConsoleEvent postEvent;
            dStrcpy(postEvent.data, evalBuffer);
            postEvent.size = ConsoleEventHeaderSize + dStrlen(evalBuffer) + 1;
            Game->postEvent(postEvent);
        }
        else if (dSscanf(mLineBuffer, "BRKVARSET %s %d %[^\n]", varBuffer, &passCount, evalBuffer) == 3)
            addVariableBreakpoint(varBuffer, passCount, evalBuffer);
        else if (dSscanf(mLineBuffer, "BRKVARCLR %s", varBuffer) == 1)
            removeVariableBreakpoint(varBuffer);
        else if (dSscanf(mLineBuffer, "BRKSET %s %d %s %d %[^\n]", fileBuffer, &line, &clear, &passCount, evalBuffer) == 5)
            addBreakpoint(fileBuffer, line, dAtob(clear), passCount, evalBuffer);
        else if (dSscanf(mLineBuffer, "BRKCLR %s %d", fileBuffer, &line) == 2)
            removeBreakpoint(fileBuffer, line);
        else if (!dStrncmp(mLineBuffer, "BRKCLRALL\n", cmdLen))
            removeAllBreakpoints();
        else if (!dStrncmp(mLineBuffer, "BRKNEXT\n", cmdLen))
            debugBreakNext();
        else if (!dStrncmp(mLineBuffer, "CONTINUE\n", cmdLen))
            debugContinue();
        else if (!dStrncmp(mLineBuffer, "STEPIN\n", cmdLen))
            debugStepIn();
        else if (!dStrncmp(mLineBuffer, "STEPOVER\n", cmdLen))
            debugStepOver();
        else if (!dStrncmp(mLineBuffer, "STEPOUT\n", cmdLen))
            debugStepOut();
        else if (dSscanf(mLineBuffer, "EVAL %s %d %[^\n]", varBuffer, &frame, evalBuffer) == 3)
            evaluateExpression(varBuffer, frame, evalBuffer);
        else if (!dStrncmp(mLineBuffer, "FILELIST\n", cmdLen))
            dumpFileList();
        else if (dSscanf(mLineBuffer, "BREAKLIST %s", fileBuffer) == 1)
            dumpBreakableList(fileBuffer);
        else
        {
            // invalid stuff.
            send("DBGERR Invalid command!\r\n");
        }
    }
}

void TelnetDebugger::addVariableBreakpoint(const char*, S32, const char*)
{
    send("addVariableBreakpoint\r\n");
}

void TelnetDebugger::removeVariableBreakpoint(const char*)
{
    send("removeVariableBreakpoint\r\n");
}

void TelnetDebugger::addAllBreakpoints(CodeBlock* code)
{
    if (mState == NotConnected)
        return;

    // Find the breakpoints for this code block and attach them.
    Breakpoint** walk = &mBreakpoints;
    Breakpoint* cur;
    while ((cur = *walk) != NULL)
    {
        if (cur->fileName == code->name)
        {
            cur->code = code;

            // Find the fist breakline starting from and
            // including the requested breakline.
            S32 newLine = code->findFirstBreakLine(cur->lineNumber);
            if (newLine <= 0)
            {
                walk = &cur->next;

                char buffer[MaxCommandSize];
                dSprintf(buffer, MaxCommandSize, "BRKCLR %s %d\r\n", cur->fileName, cur->lineNumber);
                send(buffer);

                removeBreakpoint(cur->fileName, cur->lineNumber);
                continue;
            }

            // If the requested breakline does not match
            // the actual break line we need to inform
            // the client.
            if (newLine != cur->lineNumber)
            {
                char buffer[MaxCommandSize];

                // If we already have a line at this breapoint then
                // tell the client to clear the breakpoint.
                if (findBreakpoint(cur->fileName, newLine)) {

                    walk = &cur->next;

                    dSprintf(buffer, MaxCommandSize, "BRKCLR %s %d\r\n", cur->fileName, cur->lineNumber);
                    send(buffer);

                    removeBreakpoint(cur->fileName, cur->lineNumber);
                    continue;
                }

                // We're moving the breakpoint to new line... inform the 
                // client so it can update it's view.
                dSprintf(buffer, MaxCommandSize, "BRKMOV %s %d %d\r\n", cur->fileName, cur->lineNumber, newLine);
                send(buffer);
                cur->lineNumber = newLine;
            }

            code->setBreakpoint(cur->lineNumber);
        }

        walk = &cur->next;
    }

    // Enable all breaks if a break next was set.
    if (mBreakOnNextStatement)
        code->setAllBreaks();
}

void TelnetDebugger::addBreakpoint(const char* fileName, S32 line, bool clear, S32 passCount, const char* evalString)
{
    fileName = StringTable->insert(fileName);
    Breakpoint** bp = findBreakpoint(fileName, line);

    if (bp)
    {
        // trying to add the same breakpoint...
        Breakpoint* brk = *bp;
        dFree(brk->testExpression);
        brk->testExpression = dStrdup(evalString);
        brk->passCount = passCount;
        brk->clearOnHit = clear;
        brk->curCount = 0;
    }
    else
    {
        // Note that if the code block is not already 
        // loaded it is handled by addAllBreakpoints.
        CodeBlock* code = CodeBlock::find(fileName);
        if (code)
        {
            // Find the fist breakline starting from and
            // including the requested breakline.
            S32 newLine = code->findFirstBreakLine(line);
            if (newLine <= 0)
            {
                char buffer[MaxCommandSize];
                dSprintf(buffer, MaxCommandSize, "BRKCLR %s %d\r\n", fileName, line);
                send(buffer);
                return;
            }

            // If the requested breakline does not match
            // the actual break line we need to inform
            // the client.
            if (newLine != line)
            {
                char buffer[MaxCommandSize];

                // If we already have a line at this breapoint then
                // tell the client to clear the breakpoint.
                if (findBreakpoint(fileName, newLine)) {
                    dSprintf(buffer, MaxCommandSize, "BRKCLR %s %d\r\n", fileName, line);
                    send(buffer);
                    return;
                }

                // We're moving the breakpoint to new line... inform the client.
                dSprintf(buffer, MaxCommandSize, "BRKMOV %s %d %d\r\n", fileName, line, newLine);
                send(buffer);
                line = newLine;
            }

            code->setBreakpoint(line);
        }

        Breakpoint* brk = new Breakpoint;
        brk->code = code;
        brk->fileName = fileName;
        brk->lineNumber = line;
        brk->passCount = passCount;
        brk->clearOnHit = clear;
        brk->curCount = 0;
        brk->testExpression = dStrdup(evalString);
        brk->next = mBreakpoints;
        mBreakpoints = brk;
    }
}

void TelnetDebugger::removeBreakpointsFromCode(CodeBlock* code)
{
    Breakpoint** walk = &mBreakpoints;
    Breakpoint* cur;
    while ((cur = *walk) != NULL)
    {
        if (cur->code == code)
        {
            dFree(cur->testExpression);
            *walk = cur->next;
            delete walk;
        }
        else
            walk = &cur->next;
    }
}

void TelnetDebugger::removeBreakpoint(const char* fileName, S32 line)
{
    fileName = StringTable->insert(fileName);
    Breakpoint** bp = findBreakpoint(fileName, line);
    if (bp)
    {
        Breakpoint* brk = *bp;
        *bp = brk->next;
        if (brk->code)
            brk->code->clearBreakpoint(brk->lineNumber);
        dFree(brk->testExpression);
        delete brk;
    }
}

void TelnetDebugger::removeAllBreakpoints()
{
    Breakpoint* walk = mBreakpoints;
    while (walk)
    {
        Breakpoint* temp = walk->next;
        if (walk->code)
            walk->code->clearBreakpoint(walk->lineNumber);
        dFree(walk->testExpression);
        delete walk;
        walk = temp;
    }
    mBreakpoints = NULL;
}

void TelnetDebugger::debugContinue()
{
    if (mState == Initialize) {
        mState = Connected;
        return;
    }

    setBreakOnNextStatement(false);
    mStackPopBreakIndex = -1;
    mProgramPaused = false;
    send("RUNNING\r\n");
}

void TelnetDebugger::setBreakOnNextStatement(bool enabled)
{
    if (enabled)
    {
        // Apply breaks on all the code blocks.
        for (CodeBlock* walk = CodeBlock::getCodeBlockList(); walk; walk = walk->nextFile)
            walk->setAllBreaks();
        mBreakOnNextStatement = true;
    }
    else if (!enabled)
    {
        // Clear all the breaks on the codeblocks 
        // then go reapply the breakpoints.
        for (CodeBlock* walk = CodeBlock::getCodeBlockList(); walk; walk = walk->nextFile)
            walk->clearAllBreaks();
        for (Breakpoint* w = mBreakpoints; w; w = w->next)
        {
            if (w->code)
                w->code->setBreakpoint(w->lineNumber);
        }
        mBreakOnNextStatement = false;
    }
}

void TelnetDebugger::debugBreakNext()
{
    if (mState != Connected)
        return;

    if (!mProgramPaused)
        setBreakOnNextStatement(true);
}

void TelnetDebugger::debugStepIn()
{
    // Note that step in is allowed during
    // the initialize state, so that we can
    // break on the first script line executed.

    setBreakOnNextStatement(true);
    mStackPopBreakIndex = -1;
    mProgramPaused = false;

    // Don't bother sending this to the client
    // if it's in the initialize state.  It will
    // just be ignored as the client knows it
    // is in a running state when it connects.
    if (mState != Initialize)
        send("RUNNING\r\n");
    else
        mState = Connected;
}

void TelnetDebugger::debugStepOver()
{
    if (mState != Connected)
        return;

    setBreakOnNextStatement(true);
    mStackPopBreakIndex = gEvalState.stack.size();
    mProgramPaused = false;
    send("RUNNING\r\n");
}

void TelnetDebugger::debugStepOut()
{
    if (mState != Connected)
        return;

    setBreakOnNextStatement(false);
    mStackPopBreakIndex = gEvalState.stack.size() - 1;
    if (mStackPopBreakIndex == 0)
        mStackPopBreakIndex = -1;
    mProgramPaused = false;
    send("RUNNING\r\n");
}

void TelnetDebugger::evaluateExpression(const char* tag, S32 frame, const char* evalBuffer)
{
    // Make sure we're passing a valid frame to the eval.
    if (frame > gEvalState.stack.size())
        frame = gEvalState.stack.size() - 1;
    if (frame < 0)
        frame = 0;

    // Gotta have a statement to execute.
    char buffer[MaxCommandSize];
    dSprintf(buffer, MaxCommandSize, "$Debug::result = %s;", evalBuffer);

    CodeBlock* newCodeBlock = new CodeBlock();
    const char* result = newCodeBlock->compileExec(NULL, buffer, false, frame);

    dSprintf(buffer, MaxCommandSize, "EVALOUT %s %s\r\n", tag, result[0] ? result : "\"\"");
    send(buffer);
}

void TelnetDebugger::dumpFileList()
{
    send("FILELISTOUT ");
    for (CodeBlock* walk = CodeBlock::getCodeBlockList(); walk; walk = walk->nextFile)
    {
        send(walk->name);
        if (walk->nextFile)
            send(" ");
    }
    send("\r\n");
}

void TelnetDebugger::dumpBreakableList(const char* fileName)
{
    fileName = StringTable->insert(fileName);
    CodeBlock* file = CodeBlock::find(fileName);
    char buffer[MaxCommandSize];
    if (file)
    {
        dSprintf(buffer, MaxCommandSize, "BREAKLISTOUT %s %d", fileName, file->breakListSize >> 1);
        send(buffer);
        for (U32 i = 0; i < file->breakListSize; i += 2)
        {
            dSprintf(buffer, MaxCommandSize, " %d %d", file->breakList[i], file->breakList[i + 1]);
            send(buffer);
        }
        send("\r\n");
    }
    else
        send("DBGERR No Such file!");
}
