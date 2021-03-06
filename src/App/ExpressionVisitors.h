/***************************************************************************
 *   Copyright (c) Eivind Kvedalen (eivind@kvedalen.name) 2016             *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/

#ifndef RENAMEOBJECTIDENTIFIEREXPRESSIONVISITOR_H
#define RENAMEOBJECTIDENTIFIEREXPRESSIONVISITOR_H

#include "Base/BaseClass.h"
#include "Expression.h"

namespace App {

/**
 * @brief The RenameObjectIdentifierExpressionVisitor class is a functor used to visit each node of an expression, and
 * possibly rename VariableExpression nodes.
 */

template<class P> class RenameObjectIdentifierExpressionVisitor : public ExpressionModifier<P> {
public:
    RenameObjectIdentifierExpressionVisitor(P & _prop,
                                            const std::map<ObjectIdentifier, ObjectIdentifier> &_paths, const ObjectIdentifier & _owner)
        : ExpressionModifier<P>(_prop)
        , paths(_paths)
        , owner(_owner)
    {
    }

    void visit(Expression *node) {
        VariableExpression *expr = Base::freecad_dynamic_cast<VariableExpression>(node);

        if (expr) {
            const App::ObjectIdentifier & oldPath = expr->getPath().canonicalPath();
            const std::map<ObjectIdentifier, ObjectIdentifier>::const_iterator it = paths.find(oldPath);

            if (it != paths.end()) {
                ExpressionModifier<P>::setExpressionChanged();
                expr->setPath(it->second.relativeTo(owner));
            }
        }
    }


private:
   const std::map<ObjectIdentifier, ObjectIdentifier> &paths; /**< Map with current and new object identifiers */
   const ObjectIdentifier owner;                              /**< Owner of expression */
};

/**
 * @brief The RelabelDocumentObjectExpressionVisitor class is a functor class used to rename variables in an expression.
 */

template<class P> class RelabelDocumentObjectExpressionVisitor : public ExpressionModifier<P> {
public:

    RelabelDocumentObjectExpressionVisitor(P & _prop, const std::string & _oldName, const std::string & _newName)
        : ExpressionModifier<P>(_prop)
        , oldName(_oldName)
        , newName(_newName)
    {
    }

    ~RelabelDocumentObjectExpressionVisitor() {
    }

    /**
     * @brief Visit each node in the expression, and if it is a VariableExpression object, incoke renameDocumentObject in it.
     * @param node Node to visit
     */

    void visit(Expression * node) {
        VariableExpression *expr = Base::freecad_dynamic_cast<VariableExpression>(node);

        if (expr && expr->validDocumentObjectRename(oldName, newName)) {
            ExpressionModifier<P>::setExpressionChanged();
            expr->renameDocumentObject(oldName, newName);
        }
    }

private:
    std::string oldName; /**< Document object name to replace  */
    std::string newName; /**< New document object name */
};

template<class P> class RelabelDocumentExpressionVisitor : public ExpressionModifier<P> {
public:

    RelabelDocumentExpressionVisitor(P & prop, const std::string & _oldName, const std::string & _newName)
         : ExpressionModifier<P>(prop)
         , oldName(_oldName)
         , newName(_newName)
    {
    }

    void visit(Expression * node) {
        VariableExpression *expr = Base::freecad_dynamic_cast<VariableExpression>(node);

        if (expr && expr->validDocumentRename(oldName, newName)) {
            ExpressionModifier<P>::setExpressionChanged();
            expr->renameDocument(oldName, newName);
        }
    }

private:
    std::string oldName;
    std::string newName;
};

}

#endif // RENAMEOBJECTIDENTIFIEREXPRESSIONVISITOR_H
