/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/



#include "SyntaxHighlighter.h"
#include "TextEdit.h"

using namespace Gui;

namespace Gui {
class SyntaxHighlighterP
{
public:
    SyntaxHighlighterP()
    {
        cNormalText.setRgb(0, 0, 0); cComment.setRgb(0, 170, 0);
        cBlockcomment.setRgb(160, 160, 164); cLiteral.setRgb(255, 0, 0);
        cNumber.setRgb(0, 0, 255); cOperator.setRgb(160, 160, 164);
        cKeyword.setRgb(0, 0, 255); cClassName.setRgb(255, 170, 0);
        cDefineName.setRgb(255, 170, 0); cOutput.setRgb(170, 170, 127); 
        cError.setRgb(255, 0, 0);
    }

    QColor cNormalText, cComment, cBlockcomment, cLiteral, cNumber,
    cOperator, cKeyword, cClassName, cDefineName, cOutput, cError;
};
} // namespace Gui

/**
 * Constructs a syntax highlighter.
 */
SyntaxHighlighter::SyntaxHighlighter(QObject* parent)
    : QSyntaxHighlighter(parent)
{
    d = new SyntaxHighlighterP;
}

/** Destroys this object. */
SyntaxHighlighter::~SyntaxHighlighter()
{
    delete d;
}

/** Sets the color \a col to the paragraph type \a type. 
 * This method is provided for convenience to specify the paragraph type
 * by its name.
 */
void SyntaxHighlighter::setColor(const QString& type, const QColor& col)
{
    // Rehighlighting is very expensive, thus avoid it if this color is already set
    QColor old = color(type);
    if (!old.isValid())
        return; // no such type
    if (old == col)
        return; 
    if (type == QString("Text"))
        d->cNormalText = col;
    else if (type == QString("Comment"))
        d->cComment = col;
    else if (type == QString("Block comment"))
        d->cBlockcomment = col;
    else if (type == QString("Number"))
        d->cNumber = col;
    else if (type == QString("String"))
        d->cLiteral = col;
    else if (type == QString("Keyword"))
        d->cKeyword = col;
    else if (type == QString("Class name"))
        d->cClassName = col;
    else if (type == QString("Define name"))
        d->cDefineName = col;
    else if (type == QString("Operator"))
        d->cOperator = col;
    else if (type == QString("Python output"))
        d->cOutput = col;
    else if (type == QString("Python error"))
        d->cError = col;
    colorChanged(type, col);
}

QColor SyntaxHighlighter::color(const QString& type)
{
    if (type == QString("Text"))
        return d->cNormalText;
    else if (type == QString("Comment"))
        return d->cComment;
    else if (type == QString("Block comment"))
        return d->cBlockcomment;
    else if (type == QString("Number"))
        return d->cNumber;
    else if (type == QString("String"))
        return d->cLiteral;
    else if (type == QString("Keyword"))
        return d->cKeyword;
    else if (type == QString("Class name"))
        return d->cClassName;
    else if (type == QString("Define name"))
        return d->cDefineName;
    else if (type == QString("Operator"))
        return d->cOperator;
    else if (type == QString("Python output"))
        return d->cOutput;
    else if (type == QString("Python error"))
        return d->cError;
    else
        return QColor(); // not found
}

QColor SyntaxHighlighter::colorByType(SyntaxHighlighter::TColor type)
{
    if (type == SyntaxHighlighter::Text)
        return d->cNormalText;
    else if (type == SyntaxHighlighter::Comment)
        return d->cComment;
    else if (type == SyntaxHighlighter::BlockComment)
        return d->cBlockcomment;
    else if (type == SyntaxHighlighter::Number)
        return d->cNumber;
    else if (type == SyntaxHighlighter::String)
        return d->cLiteral;
    else if (type == SyntaxHighlighter::Keyword)
        return d->cKeyword;
    else if (type == SyntaxHighlighter::Classname)
        return d->cClassName;
    else if (type == SyntaxHighlighter::Defname)
        return d->cDefineName;
    else if (type == SyntaxHighlighter::Operator)
        return d->cOperator;
    else if (type == SyntaxHighlighter::Output)
        return d->cOutput;
    else if (type == SyntaxHighlighter::Error)
        return d->cError;
    else
        return QColor(); // not found
}

void SyntaxHighlighter::colorChanged(const QString& type, const QColor& col)
{
    Q_UNUSED(type); 
    Q_UNUSED(col); 
    rehighlight();
}

int SyntaxHighlighter::maximumUserState() const
{
    return 8;
}
