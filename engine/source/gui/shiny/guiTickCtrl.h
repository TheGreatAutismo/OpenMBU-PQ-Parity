//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUITICKCTRL_H_
#define _GUITICKCTRL_H_

#include "gui/core/guiControl.h"
#include "core/iTickable.h"

/// This Gui Control is designed to be subclassed and let people create controls
/// which want to recieve update ticks at a constant interval. This class was
/// created to be the Parent class of a control which used a DynamicTexture
/// along with a VectorField to create warping effects much like the ones found
/// in visualization displays for iTunes or Winamp. Those displays are updated
/// at the framerate frequency. This works fine for those effects, however for
/// an application of the same type of effects for things like Gui transitions
/// the framerate-driven update frequency is not desireable because it does not
/// allow the developer to be able to have any idea of a consistant user-experience.
///
/// Enter the ITickable interface. This lets the Gui control, in this case, update
/// the dynamic texture at a constant rate of once per tick, even though it gets
/// rendered every frame, thus creating a framerate-independant update frequency
/// so that the effects are at a consistant speed regardless of the specifics
/// of the system the user is on. This means that the screen-transitions will
/// occur in the same time on a machine getting 300fps in the Gui shell as a
/// machine which gets 150fps in the Gui shell.
/// @see ITickable
class GuiTickCtrl : public GuiControl, public virtual ITickable
{
    typedef GuiControl Parent;

private:

protected:

    // So this can be instantiated and not be a pure virtual class
    virtual void interpolateTick(F32 delta) {};
    virtual void processTick() {};
    virtual void advanceTime(F32 timeDelta) {};

public:
    DECLARE_CONOBJECT(GuiTickCtrl);
};


#endif