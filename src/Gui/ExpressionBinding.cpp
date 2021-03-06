/***************************************************************************
 *   Copyright (c) 2015 Eivind Kvedalen <eivind@kvedalen.name>             *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/

# include <QPixmapCache>
#include "ExpressionBinding.h"
#include "BitmapFactory.h"
#include "Command.h"
#include "App/Expression.h"
#include "App/DocumentObject.h"
#include "Base/Tools.h"
#include "App/ObjectIdentifier.h"
#include "App/Document.h"
#include <boost/bind/bind.hpp>

using namespace Gui;
using namespace App;
namespace bp = boost::placeholders;

ExpressionBinding::ExpressionBinding()
    : iconLabel(0)
    , iconHeight(-1)
    , m_autoApply(false)
{
}


ExpressionBinding::~ExpressionBinding()
{
}

bool ExpressionBinding::isBound() const
{
    return path.getDocumentObject() != 0;
}

void Gui::ExpressionBinding::setExpression(boost::shared_ptr<Expression> expr)
{
    DocumentObject * docObj = path.getDocumentObject();

    if (expr) {
        const std::string error = docObj->ExpressionEngine.validateExpression(path, expr);

        if (error.size())
            throw Base::RuntimeError(error.c_str());

    }

    lastExpression = getExpression();
    docObj->ExpressionEngine.setValue(path, expr);
    
    if(m_autoApply)
        apply();
}

void ExpressionBinding::bind(const App::ObjectIdentifier &_path)
{
    const Property * prop = _path.getProperty();

    Q_ASSERT(prop != 0);

    path = prop->canonicalPath(_path);
    
    //connect to be informed about changes
    DocumentObject * docObj = path.getDocumentObject();
    connection = docObj->ExpressionEngine.expressionChanged.connect(boost::bind(&ExpressionBinding::expressionChange, this, bp::_1));
}

void ExpressionBinding::bind(const Property &prop)
{
    bind(App::ObjectIdentifier(prop));
}

bool ExpressionBinding::hasExpression() const
{
    return isBound() && getExpression() != 0;
}

boost::shared_ptr<App::Expression> ExpressionBinding::getExpression() const
{
    DocumentObject * docObj = path.getDocumentObject();

    Q_ASSERT(isBound() && docObj != 0);

    return docObj->getExpression(path).expression;
}

std::string ExpressionBinding::getExpressionString() const
{
    if (!getExpression())
        throw Base::RuntimeError("No expression found.");

    return getExpression()->toString();
}

std::string ExpressionBinding::getEscapedExpressionString() const
{
    return Base::Tools::escapedUnicodeFromUtf8(getExpressionString().c_str());
}

QPixmap ExpressionBinding::getIcon(const char* name, const QSize& size) const
{
    QString key = QString("%1_%2x%3")
        .arg(QString(name))
        .arg(size.width())
        .arg(size.height());
    QPixmap icon;
    if (QPixmapCache::find((QString &)key, (QPixmap*)&icon))
        return icon;

    icon = BitmapFactory().pixmapFromSvg(name, size);
    if (!icon.isNull())
        QPixmapCache::insert(key, icon);
    return icon;
}

bool ExpressionBinding::apply(const std::string & propName)
{
    Q_UNUSED(propName); 
    if (hasExpression()) {
        DocumentObject * docObj = path.getDocumentObject();

        if (!docObj)
            throw Base::RuntimeError("Document object not found.");
        Gui::Command::doCommand(Gui::Command::Doc,"App.getDocument('%s').%s.setExpression('%s', u'%s')",
                                docObj->getDocument()->getName(),
                                docObj->getNameInDocument(),
                                path.toEscapedString().c_str(),
                                getEscapedExpressionString().c_str());
        return true;
    }
    else {
        if (isBound()) {
            DocumentObject * docObj = path.getDocumentObject();

            if (!docObj)
                throw Base::RuntimeError("Document object not found.");

            if (lastExpression)
                Gui::Command::doCommand(Gui::Command::Doc,"App.getDocument('%s').%s.setExpression('%s', None)",
                                        docObj->getDocument()->getName(),
                                        docObj->getNameInDocument(),
                                        path.toEscapedString().c_str());
        }

        return false;
    }
}

bool ExpressionBinding::apply()
{
    Property * prop(path.getProperty());

    assert(prop != 0);
    Q_UNUSED(prop);

    DocumentObject * docObj(path.getDocumentObject());

    if (!docObj)
        throw Base::RuntimeError("Document object not found.");

    /* Skip updating read-only properties */
    if (prop->isReadOnly())
        return true;
    
    std::string name = docObj->getNameInDocument();

    return apply("App.ActiveDocument." + name + "." + getPath().toEscapedString());
}

void ExpressionBinding::expressionChange(const ObjectIdentifier& id) {

    if(id==path)
        onChange();
}
