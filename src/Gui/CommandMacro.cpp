/***************************************************************************
 *   Copyright (c) 2010 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/



#include "Command.h"
#include "Application.h"
#include "MainWindow.h"
#include "DlgMacroExecuteImp.h"
#include "DlgMacroRecordImp.h"
#include "Macro.h"
#include "PythonDebugger.h"

using namespace Gui;


//===========================================================================
// Std_DlgMacroRecord
//===========================================================================
DEF_STD_CMD_A(StdCmdDlgMacroRecord);

StdCmdDlgMacroRecord::StdCmdDlgMacroRecord()
  : Command("Std_DlgMacroRecord")
{
    sGroup        = QT_TR_NOOP("Macro");
    sMenuText     = QT_TR_NOOP("&Macro recording ...");
    sToolTipText  = QT_TR_NOOP("Opens a dialog to record a macro");
    sStatusTip    = QT_TR_NOOP("Opens a dialog to record a macro");
    sPixmap       = "media-record";
    eType         = 0;
}

void StdCmdDlgMacroRecord::activated(int iMsg)
{
    Q_UNUSED(iMsg); 
    Gui::Dialog::DlgMacroRecordImp cDlg(getMainWindow());
    cDlg.exec();
}

bool StdCmdDlgMacroRecord::isActive(void)
{
    return ! (getGuiApplication()->macroManager()->isOpen());
}

//===========================================================================
// Std_MacroStopRecord
//===========================================================================
DEF_STD_CMD_A(StdCmdMacroStopRecord);

StdCmdMacroStopRecord::StdCmdMacroStopRecord()
  : Command("Std_MacroStopRecord")
{
    sGroup        = QT_TR_NOOP("Macro");
    sMenuText     = QT_TR_NOOP("S&top macro recording");
    sToolTipText  = QT_TR_NOOP("Stop the macro recording session");
    sStatusTip    = QT_TR_NOOP("Stop the macro recording session");
    sPixmap       = "media-playback-stop";
    eType         = 0;
}

void StdCmdMacroStopRecord::activated(int iMsg)
{
    Q_UNUSED(iMsg); 
    getGuiApplication()->macroManager()->commit();
}

bool StdCmdMacroStopRecord::isActive(void)
{
    return getGuiApplication()->macroManager()->isOpen();
}

//===========================================================================
// Std_DlgMacroExecute
//===========================================================================
DEF_STD_CMD_A(StdCmdDlgMacroExecute);

StdCmdDlgMacroExecute::StdCmdDlgMacroExecute()
  : Command("Std_DlgMacroExecute")
{
    sGroup        = QT_TR_NOOP("Macro");
    sMenuText     = QT_TR_NOOP("Macros ...");
    sToolTipText  = QT_TR_NOOP("Opens a dialog to let you execute a recorded macro");
    sStatusTip    = QT_TR_NOOP("Opens a dialog to let you execute a recorded macro");
    sPixmap       = "accessories-text-editor";
    eType         = 0;
}

void StdCmdDlgMacroExecute::activated(int iMsg)
{
    Q_UNUSED(iMsg); 
    Gui::Dialog::DlgMacroExecuteImp cDlg(getMainWindow());
    cDlg.exec();
}

bool StdCmdDlgMacroExecute::isActive(void)
{
    return ! (getGuiApplication()->macroManager()->isOpen());
}

//===========================================================================
// Std_DlgMacroExecuteDirect
//===========================================================================
DEF_STD_CMD_A(StdCmdDlgMacroExecuteDirect);

StdCmdDlgMacroExecuteDirect::StdCmdDlgMacroExecuteDirect()
  : Command("Std_DlgMacroExecuteDirect")
{
    sGroup        = QT_TR_NOOP("Macro");
    sMenuText     = QT_TR_NOOP("Execute macro");
    sToolTipText  = QT_TR_NOOP("Execute the macro in the editor");
    sStatusTip    = QT_TR_NOOP("Execute the macro in the editor");
    sPixmap       = "media-playback-start";
    sAccel        = "Ctrl+F6";
    eType         = 0;
}

void StdCmdDlgMacroExecuteDirect::activated(int iMsg)
{
    Q_UNUSED(iMsg); 
    doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"Run\")");
}

bool StdCmdDlgMacroExecuteDirect::isActive(void)
{
    return getGuiApplication()->sendHasMsgToActiveView("Run");
}

DEF_STD_CMD_A(StdCmdMacroStartDebug);

StdCmdMacroStartDebug::StdCmdMacroStartDebug()
  : Command("Std_MacroStartDebug")
{
    sGroup        = QT_TR_NOOP("Macro");
    sMenuText     = QT_TR_NOOP("Debug macro");
    sToolTipText  = QT_TR_NOOP("Start debugging of macro");
    sStatusTip    = QT_TR_NOOP("Start debugging of macro");
    sPixmap       = "debug-start";
    sAccel        = "F6";
    eType         = 0;
}

void StdCmdMacroStartDebug::activated(int iMsg)
{
    Q_UNUSED(iMsg); 
    PythonDebugger* dbg = Application::Instance->macroManager()->debugger();
    if (!dbg->isRunning())
        doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"StartDebug\")");
    else
        dbg->stepRun();
}

bool StdCmdMacroStartDebug::isActive(void)
{
    return getGuiApplication()->sendHasMsgToActiveView("StartDebug");
}

DEF_STD_CMD_A(StdCmdMacroStopDebug);

StdCmdMacroStopDebug::StdCmdMacroStopDebug()
  : Command("Std_MacroStopDebug")
{
    sGroup        = QT_TR_NOOP("Macro");
    sMenuText     = QT_TR_NOOP("Stop debugging");
    sToolTipText  = QT_TR_NOOP("Stop debugging of macro");
    sStatusTip    = QT_TR_NOOP("Stop debugging of macro");
    sPixmap       = "debug-stop";
    sAccel        = "Shift+F6";
    eType         = 0;
}

void StdCmdMacroStopDebug::activated(int iMsg)
{
    Q_UNUSED(iMsg); 
    Application::Instance->macroManager()->debugger()->tryStop();
}

bool StdCmdMacroStopDebug::isActive(void)
{
    static PythonDebugger* dbg = Application::Instance->macroManager()->debugger();
    return dbg->isRunning();
}

DEF_STD_CMD_A(StdCmdMacroStepOver);

StdCmdMacroStepOver::StdCmdMacroStepOver()
  : Command("Std_MacroStepOver")
{
    sGroup        = QT_TR_NOOP("Macro");
    sMenuText     = QT_TR_NOOP("Step over");
    sToolTipText  = QT_TR_NOOP("Step over");
    sStatusTip    = QT_TR_NOOP("Step over");
    sPixmap       = 0;
    sAccel        = "F10";
    eType         = 0;
}

void StdCmdMacroStepOver::activated(int iMsg)
{
    Q_UNUSED(iMsg); 
    Application::Instance->macroManager()->debugger()->stepOver();
}

bool StdCmdMacroStepOver::isActive(void)
{
    static PythonDebugger* dbg = Application::Instance->macroManager()->debugger();
    return dbg->isRunning();
}

DEF_STD_CMD_A(StdCmdMacroStepInto);

StdCmdMacroStepInto::StdCmdMacroStepInto()
  : Command("Std_MacroStepInto")
{
    sGroup        = QT_TR_NOOP("Macro");
    sMenuText     = QT_TR_NOOP("Step into");
    sToolTipText  = QT_TR_NOOP("Step into");
    sStatusTip    = QT_TR_NOOP("Step into");
    sPixmap       = 0;
    sAccel        = "F11";
    eType         = 0;
}

void StdCmdMacroStepInto::activated(int iMsg)
{
    Q_UNUSED(iMsg); 
    Application::Instance->macroManager()->debugger()->stepInto();
}

bool StdCmdMacroStepInto::isActive(void)
{
    static PythonDebugger* dbg = Application::Instance->macroManager()->debugger();
    return dbg->isRunning();
}

DEF_STD_CMD_A(StdCmdToggleBreakpoint);

StdCmdToggleBreakpoint::StdCmdToggleBreakpoint()
  : Command("Std_ToggleBreakpoint")
{
    sGroup        = QT_TR_NOOP("Macro");
    sMenuText     = QT_TR_NOOP("Toggle breakpoint");
    sToolTipText  = QT_TR_NOOP("Toggle breakpoint");
    sStatusTip    = QT_TR_NOOP("Toggle breakpoint");
    sPixmap       = 0;
    sAccel        = "F9";
    eType         = 0;
}

void StdCmdToggleBreakpoint::activated(int iMsg)
{
    Q_UNUSED(iMsg); 
    doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"ToggleBreakpoint\")");
}

bool StdCmdToggleBreakpoint::isActive(void)
{
    return getGuiApplication()->sendHasMsgToActiveView("ToggleBreakpoint");
}

namespace Gui {

void CreateMacroCommands(void)
{
    CommandManager &rcCmdMgr = Application::Instance->commandManager();
    rcCmdMgr.addCommand(new StdCmdDlgMacroRecord());
    rcCmdMgr.addCommand(new StdCmdMacroStopRecord());
    rcCmdMgr.addCommand(new StdCmdDlgMacroExecute());
    rcCmdMgr.addCommand(new StdCmdDlgMacroExecuteDirect());
    rcCmdMgr.addCommand(new StdCmdMacroStartDebug());
    rcCmdMgr.addCommand(new StdCmdMacroStopDebug());
    rcCmdMgr.addCommand(new StdCmdMacroStepOver());
    rcCmdMgr.addCommand(new StdCmdMacroStepInto());
    rcCmdMgr.addCommand(new StdCmdToggleBreakpoint());
}

} // namespace Gui
