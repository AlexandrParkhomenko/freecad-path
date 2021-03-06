/***************************************************************************
 *   Copyright (c) 2011 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/


#include <sstream>
#include <QDir>
#include <QMessageBox>

#include "App/Document.h"
#include "App/PropertyStandard.h"

#include "DlgProjectUtility.h"
#include "Application.h"
#include "Command.h"
#include "ui_DlgProjectUtility.h"


using namespace Gui::Dialog;

// taken from the script doctools.py
std::string DlgProjectUtility::doctools =
"import os,sys,string\n"
"import xml.sax\n"
"import xml.sax.handler\n"
"import xml.sax.xmlreader\n"
"import zipfile_in_DlgProjectUtility\n"
"\n"
"# SAX handler to parse the Document.xml\n"
"class DocumentHandler(xml.sax.handler.ContentHandler):\n"
"	def __init__(self, dirname):\n"
"		self.files = []\n"
"		self.dirname = dirname\n"
"\n"
"	def startElement(self, name, attributes):\n"
"		item=attributes.get(\"file\")\n"
"		if item != None:\n"
"			self.files.append(os.path.join(self.dirname,str(item)))\n"
"\n"
"	def characters(self, data):\n"
"		return\n"
"\n"
"	def endElement(self, name):\n"
"		return\n"
"\n"
"def extractDocument(filename, outpath):\n"
"	zfile=zipfile.ZipFile(filename)\n"
"	files=zfile.namelist()\n"
"\n"
"	for i in files:\n"
"		data=zfile.read(i)\n"
"		dirs=i.split(\"/\")\n"
"		if len(dirs) > 1:\n"
"			dirs.pop()\n"
"			curpath=outpath\n"
"			for j in dirs:\n"
"				curpath=curpath+\"/\"+j\n"
"				os.mkdir(curpath)\n"
"		output=open(outpath+\"/\"+i,\'wb\')\n"
"		output.write(data)\n"
"		output.close()\n"
"\n"
"def createDocument(filename, outpath):\n"
"	files=getFilesList(filename)\n"
"	dirname=os.path.dirname(filename)\n"
"	guixml=os.path.join(dirname,\"GuiDocument.xml\")\n"
"	if os.path.exists(guixml):\n"
"		files.extend(getFilesList(guixml))\n"
"	compress=zipfile.ZipFile(outpath,\'w\',zipfile.ZIP_DEFLATED)\n"
"	for i in files:\n"
"		dirs=os.path.split(i)\n"
"		#print i, dirs[-1]\n"
"		compress.write(i,dirs[-1],zipfile.ZIP_DEFLATED)\n"
"	compress.close()\n"
"\n"
"def getFilesList(filename):\n"
"	dirname=os.path.dirname(filename)\n"
"	handler=DocumentHandler(dirname)\n"
"	parser=xml.sax.make_parser()\n"
"	parser.setContentHandler(handler)\n"
"	parser.parse(filename)\n"
"\n"
"	files=[]\n"
"	files.append(filename)\n"
"	files.extend(iter(handler.files))\n"
"	return files\n"
;



/* TRANSLATOR Gui::Dialog::DlgProjectUtility */

DlgProjectUtility::DlgProjectUtility(QWidget* parent, Qt::WindowFlags fl)
  : QDialog(parent, fl), ui(new Ui_DlgProjectUtility)
{
    ui->setupUi(this);
    ui->extractSource->setFilter(QString("%1 (*.freecad)").arg(tr("Project file")));
}

/**
 *  Destroys the object and frees any allocated resources
 */
DlgProjectUtility::~DlgProjectUtility()
{
  // no need to delete child widgets, Qt does it all for us
    delete ui;
}

void DlgProjectUtility::on_extractButton_clicked()
{
    QString source = ui->extractSource->fileName();
    QString dest = ui->extractDest->fileName();
    if (source.isEmpty()) {
        QMessageBox::critical(this, tr("Empty source"), tr("No source is defined."));
        return;
    }
    if (dest.isEmpty()) {
        QMessageBox::critical(this, tr("Empty destination"), tr("No destination is defined."));
        return;
    }

    std::stringstream str;
    str << doctools << "\n";
    str << "extractDocument(\"" << (const char*)source.toUtf8()
        << "\", \"" << (const char*)dest.toUtf8() << "\")";
    Gui::Command::runCommand(Gui::Command::App, str.str().c_str());
}

void DlgProjectUtility::on_createButton_clicked()
{
    QString source = ui->createSource->fileName();
    QString dest = ui->createDest->fileName();
    if (source.isEmpty()) {
        QMessageBox::critical(this, tr("Empty source"), tr("No source is defined."));
        return;
    }
    if (dest.isEmpty()) {
        QMessageBox::critical(this, tr("Empty destination"), tr("No destination is defined."));
        return;
    }

    dest = QDir(dest).absoluteFilePath(QString("project.freecad"));

    std::stringstream str;
    str << doctools << "\n";
    str << "createDocument(\"" << (const char*)source.toUtf8()
        << "\", \"" << (const char*)dest.toUtf8() << "\")";
    Gui::Command::runCommand(Gui::Command::App, str.str().c_str());

    if (ui->checkLoadProject->isChecked())
        Application::Instance->open((const char*)dest.toUtf8(),"FreeCAD");
}

#include "moc_DlgProjectUtility.cpp"
