/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel (juergen.riegel@web.de)              *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/



#include "Base/Exception.h"
#include "App/Document.h"
#include "Gui/Application.h"
#include "Gui/MainWindow.h"
#include "Gui/Command.h"
#include "Gui/FileDialog.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
using Gui::FileDialog;

//===========================================================================
// Import_Box
//===========================================================================
DEF_STD_CMD_A(FCCmdImportReadBREP);

FCCmdImportReadBREP::FCCmdImportReadBREP()
   : Command("Import_ReadBREP")
{
    sAppModule      = "Import";
    sGroup          = "Import";
    sMenuText       = "Read BREP";
    sToolTipText    = "Read a BREP file";
    sStatusTip      = sToolTipText;
    sPixmap         = "Std_Tool1";
}

void FCCmdImportReadBREP::activated(int iMsg)
{
    Q_UNUSED(iMsg);
    openCommand("Read BREP");
    QString fn = Gui::FileDialog::getOpenFileName(Gui::getMainWindow(), QString(), QString(), QString("BREP (*.brep *.rle)"));
    if (fn.isEmpty()) {
        abortCommand();
        return;
    }

    doCommand(Doc,"TopoShape = Import.ReadBREP(\"%s\")",(const char*)fn.toUtf8());
    commitCommand();
}

bool FCCmdImportReadBREP::isActive(void)
{
    return getGuiApplication()->activeDocument() != 0;
}

//===========================================================================
// PartImportStep
//===========================================================================
DEF_STD_CMD_A(ImportStep);

ImportStep::ImportStep()
  : Command("Part_ImportStep")
{
    sAppModule      = "Part";
    sGroup          = "Part";
    sMenuText       = "Import STEP";
    sToolTipText    = "Create or change a Import STEP feature";
    sStatusTip      = sToolTipText;
    sPixmap         = "Save";
}


void ImportStep::activated(int iMsg)
{
    Q_UNUSED(iMsg);
    QString fn = Gui::FileDialog::getOpenFileName(Gui::getMainWindow(), QString(), QString(), QString("STEP (*.stp *.step)"));
    if (!fn.isEmpty()) {
        openCommand("Part ImportSTEP Create");
        doCommand(Doc,"f = App.document().addObject(\"ImportStep\",\"ImportStep\")");
        doCommand(Doc,"f.FileName = \"%s\"",(const char*)fn.toUtf8());
        commitCommand();
        updateActive();
    }
}

bool ImportStep::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}


//===========================================================================
// ImportIges
//===========================================================================
DEF_STD_CMD_A(ImportIges);

ImportIges::ImportIges()
  : Command("Import_Iges")
{
    sAppModule      = "Import";
    sGroup          = "Part";
    sMenuText       = "Import IGES";
    sToolTipText    = "Create or change a Import IGES feature";
    sStatusTip      = sToolTipText;
    sPixmap         = "Save";
}

void ImportIges::activated(int iMsg)
{
    Q_UNUSED(iMsg);
    QString fn = Gui::FileDialog::getOpenFileName(Gui::getMainWindow(), QString(), QString(), QString("IGES (*.igs *.iges)"));
    if (!fn.isEmpty()) {
        openCommand("ImportIGES Create");
        doCommand(Doc,"f = App.document().addObject(\"ImportIges\",\"ImportIges\")");
        doCommand(Doc,"f.FileName = \"%s\"",(const char*)fn.toUtf8());
        commitCommand();
        updateActive();
    }
}

bool ImportIges::isActive(void)
{
    if (getActiveGuiDocument())
        return true;
    else
        return false;
}


void CreateImportCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new FCCmdImportReadBREP());
}
