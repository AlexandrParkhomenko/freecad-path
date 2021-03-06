/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/


#ifndef GUI_DOCUMENTOBSERVER_H
#define GUI_DOCUMENTOBSERVER_H

#include "stdexport.h"
#include "Base/BaseClass.h"
#include <boost/signals2.hpp>

namespace App { class Property; }
namespace Gui
{
class Document;
class ViewProviderDocumentObject;

/**
 * The DocumentT class is a helper class to store the name of a document.
 * This can be useful when you cannot rely on that the document still exists when you have to
 * access it.
 *
 * @author Werner Mayer
 */
class GuiExport DocumentT
{
public:
    /*! Constructor */
    DocumentT();
    /*! Constructor */
    DocumentT(Document*);
    /*! Constructor */
    DocumentT(const std::string&);
    /*! Destructor */
    ~DocumentT();
    /*! Assignment operator */
    void operator=(const DocumentT&);
    /*! Assignment operator */
    void operator=(const Document*);
    /*! Assignment operator */
    void operator=(const std::string&);

    /*! Get a pointer to the document or 0 if it doesn't exist any more. */
    Document* getDocument() const;
    /*! Get the name of the document. */
    std::string getDocumentName() const;
    /*! Get the Gui::Document as Python command. */
    std::string getGuiDocumentPython() const;
    /*! Get the App::Document as Python command. */
    std::string getAppDocumentPython() const;

private:
    std::string document;
};

/**
 * The ViewProviderT class is a helper class to store the names of a view provider and its document.
 * This can be useful when you cannot rely on that the document or the object still exists when you have to
 * access it.
 *
 * @author Werner Mayer
 */
class GuiExport ViewProviderT
{
public:
    /*! Constructor */
    ViewProviderT();
    /*! Constructor */
    ViewProviderT(ViewProviderDocumentObject*);
    /*! Destructor */
    ~ViewProviderT();
    /*! Assignment operator */
    void operator=(const ViewProviderT&);
    /*! Assignment operator */
    void operator=(const ViewProviderDocumentObject*);

    /*! Get a pointer to the document or 0 if it doesn't exist any more. */
    Document* getDocument() const;
    /*! Get the name of the document. */
    std::string getDocumentName() const;
    /*! Get the Gui::Document as Python command. */
    std::string getGuiDocumentPython() const;
    /*! Get the App::Document as Python command. */
    std::string getAppDocumentPython() const;
    /*! Get a pointer to the document object or 0 if it doesn't exist any more. */
    ViewProviderDocumentObject* getViewProvider() const;
    /*! Get the name of the document object. */
    std::string getObjectName() const;
    /*! Get the document object as Python command. */
    std::string getObjectPython() const;
    /*! Get a pointer to the document or 0 if it doesn't exist any more or the type doesn't match. */
    template<typename T>
    inline T* getObjectAs() const
    {
        return Base::freecad_dynamic_cast<T>(getViewProvider());
    }

private:
    std::string document;
    std::string object;
};

/**
 * The DocumentObserver class simplifies the step to write classes that listen
 * to what happens inside a document.
 * This is very useful for classes that needs to be notified when an observed
 * object has changed.
 *
 * @author Werner Mayer
 */
class GuiExport DocumentObserver
{
public:
    /// Constructor
    DocumentObserver();
    virtual ~DocumentObserver();

    /** Attaches to another document, the old document
     * is not longer observed then.
     */
    void attachDocument(Document*);
    /** Detaches from the current document, the document
     * is not longer observed then.
     */
    void detachDocument();

private:
    /** Notifies when an object has been created. */
    virtual void slotCreatedObject(const ViewProviderDocumentObject& Obj);
    /** Notifies when the object is about to be removed. */
    virtual void slotDeletedObject(const ViewProviderDocumentObject& Obj);
    /** The property of an observed object has changed */
    virtual void slotChangedObject(const ViewProviderDocumentObject& Obj,
                                   const App::Property& Prop);
    /** Notifies when the object has been relabeled. */
    virtual void slotRelabelObject(const ViewProviderDocumentObject& Obj);
    /** Notifies when the object has been activated. */
    virtual void slotActivatedObject(const ViewProviderDocumentObject& Obj);
    /** Notifies when the object entered edit mode. */
    virtual void slotEnterEditObject(const ViewProviderDocumentObject& Obj);
    /** Notifies when the object resets edit mode. */
    virtual void slotResetEditObject(const ViewProviderDocumentObject& Obj);
    /** Notifies on undo */
    virtual void slotUndoDocument(const Document& Doc);
    /** Notifies on redo */
    virtual void slotRedoDocument(const Document& Doc);
    /** Notifies on deletion */
    virtual void slotDeleteDocument(const Document& Doc);

private:
    typedef boost::signals2::scoped_connection Connection;
    Connection connectDocumentCreatedObject;
    Connection connectDocumentDeletedObject;
    Connection connectDocumentChangedObject;
    Connection connectDocumentRelabelObject;
    Connection connectDocumentActivateObject;
    Connection connectDocumentEditObject;
    Connection connectDocumentResetObject;
    Connection connectDocumentUndo;
    Connection connectDocumentRedo;
    Connection connectDocumentDelete;
};

} //namespace Gui

#endif // GUI_DOCUMENTOBSERVER_H
