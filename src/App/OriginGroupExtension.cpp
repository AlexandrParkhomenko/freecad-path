/***************************************************************************
 *   Copyright (c) Alexander Golubev (Fat-Zer) <fatzer2@gmail.com> 2015    *
 *   FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY                         *
 ***************************************************************************/

#include "FCConfig.h"
#include "stdexport.h"
#include "OriginGroupExtension.h"


#include "Base/Exception.h"

#include "App/Document.h"
#include "Origin.h"

#include "GeoFeature.h"

using namespace App;

EXTENSION_PROPERTY_SOURCE(App::OriginGroupExtension, App::GeoFeatureGroupExtension)

OriginGroupExtension::OriginGroupExtension () {

    initExtensionType(OriginGroupExtension::getExtensionClassTypeId());

    EXTENSION_ADD_PROPERTY_TYPE ( Origin, (0), 0, App::Prop_Hidden, "Origin linked to the group" );
    Origin.setScope(LinkScope::Child);
}

OriginGroupExtension::~OriginGroupExtension ()
{ }

App::Origin *OriginGroupExtension::getOrigin () const {
    App::DocumentObject *originObj = Origin.getValue ();

    if ( !originObj ) {
        std::stringstream err;
        err << "Can't find Origin for \"" << getExtendedObject()->getNameInDocument () << "\"";
        throw Base::RuntimeError ( err.str().c_str () );

    } else if (! originObj->isDerivedFrom ( App::Origin::getClassTypeId() ) ) {
        std::stringstream err;
        err << "Bad object \"" << originObj->getNameInDocument () << "\"(" << originObj->getTypeId().getName()
            << ") linked to the Origin of \"" << getExtendedObject()->getNameInDocument () << "\"";
        throw Base::RuntimeError ( err.str().c_str () );
    } else {
            return static_cast<App::Origin *> ( originObj );
    }
}

App::DocumentObject *OriginGroupExtension::getGroupOfObject (const DocumentObject* obj) {

    if(!obj)
        return nullptr;
    
    bool isOriginFeature = obj->isDerivedFrom(App::OriginFeature::getClassTypeId());
    
    auto list = obj->getInList();
    for (auto o : list) {
        if(o->hasExtension(App::OriginGroupExtension::getExtensionClassTypeId()))
            return o;
        else if (isOriginFeature && o->isDerivedFrom(App::Origin::getClassTypeId())) {
            auto result = getGroupOfObject(o);
            if(result)
                return result;
        }
    }

    return nullptr;
}

short OriginGroupExtension::extensionMustExecute() {
    if (Origin.isTouched ()) {
        return 1;
    } else {
        return GeoFeatureGroupExtension::extensionMustExecute();
    }
}

App::DocumentObjectExecReturn *OriginGroupExtension::extensionExecute() {
    try { // try to find all base axis and planes in the origin
        getOrigin ();
    } catch (const Base::Exception &ex) {
        //getExtendedObject()->setError ();
        return new App::DocumentObjectExecReturn ( ex.what () );
    }

    return GeoFeatureGroupExtension::extensionExecute ();
}

void OriginGroupExtension::onExtendedSetupObject () {
    App::Document *doc = getExtendedObject()->getDocument ();

    App::DocumentObject *originObj = doc->addObject ( "App::Origin", "Origin" );

    assert ( originObj && originObj->isDerivedFrom ( App::Origin::getClassTypeId () ) );
    Origin.setValue (originObj);

    GeoFeatureGroupExtension::onExtendedSetupObject ();
}

void OriginGroupExtension::onExtendedUnsetupObject () {
    App::DocumentObject *origin = Origin.getValue ();
    if (origin && !origin->isRemoving ()) {
        origin->getDocument ()->removeObject (origin->getNameInDocument());
    }

    GeoFeatureGroupExtension::onExtendedUnsetupObject ();
}

void OriginGroupExtension::relinkToOrigin(App::DocumentObject* obj)
{
    //we get all links and replace the origin objects if needed (subnames need not to change, they
    //would stay the same)
    std::vector< App::DocumentObject* > result;
    std::vector<App::Property*> list;
    obj->getPropertyList(list);
    for(App::Property* prop : list) {
        if(prop->getTypeId().isDerivedFrom(App::PropertyLink::getClassTypeId())) {
            
            auto p = static_cast<App::PropertyLink*>(prop);
            if(!p->getValue() || !p->getValue()->isDerivedFrom(App::OriginFeature::getClassTypeId()))
                continue;
        
            p->setValue(getOrigin()->getOriginFeature(static_cast<OriginFeature*>(p->getValue())->Role.getValue()));
        }            
        else if(prop->getTypeId().isDerivedFrom(App::PropertyLinkList::getClassTypeId())) {
            auto p = static_cast<App::PropertyLinkList*>(prop);
            auto vec = p->getValues();
            std::vector<App::DocumentObject*> result;
            bool changed = false;
            for(App::DocumentObject* o : vec) {
                if(!o || !o->isDerivedFrom(App::OriginFeature::getClassTypeId()))
                    result.push_back(o);
                else {
                    result.push_back(getOrigin()->getOriginFeature(static_cast<OriginFeature*>(o)->Role.getValue()));
                    changed = true;
                }
            }
            if(changed)
                static_cast<App::PropertyLinkList*>(prop)->setValues(result);
        }
        else if(prop->getTypeId().isDerivedFrom(App::PropertyLinkSub::getClassTypeId())) {
            auto p = static_cast<App::PropertyLinkSub*>(prop);
            if(!p->getValue() || !p->getValue()->isDerivedFrom(App::OriginFeature::getClassTypeId()))
                continue;
        
            std::vector<std::string> subValues = p->getSubValues();
            p->setValue(getOrigin()->getOriginFeature(static_cast<OriginFeature*>(p->getValue())->Role.getValue()), subValues);
        }
        else if(prop->getTypeId().isDerivedFrom(App::PropertyLinkSubList::getClassTypeId())) {
            auto p = static_cast<App::PropertyLinkSubList*>(prop);
            auto vec = p->getSubListValues();
            bool changed = false;
            for(auto &v : vec) {
                if(v.first && v.first->isDerivedFrom(App::OriginFeature::getClassTypeId())) {
                    v.first = getOrigin()->getOriginFeature(static_cast<OriginFeature*>(v.first)->Role.getValue());
                    changed = true;
                }
            }
            if(changed)
                p->setSubListValues(vec);
        }
    }
}

std::vector< DocumentObject* > OriginGroupExtension::addObjects(std::vector<DocumentObject*> objs) {

    for(auto obj : objs)
        relinkToOrigin(obj);

    return App::GeoFeatureGroupExtension::addObjects(objs);
}

bool OriginGroupExtension::hasObject(const DocumentObject* obj, bool recursive) const {

    if(Origin.getValue() && (obj == getOrigin() || getOrigin()->hasObject(obj)))
        return true;

    return App::GroupExtension::hasObject(obj, recursive);
}



// Python feature ---------------------------------------------------------

namespace App {
EXTENSION_PROPERTY_SOURCE_TEMPLATE(App::OriginGroupExtensionPython, App::OriginGroupExtension)

// explicit template instantiation
template class AppExport ExtensionPythonT<GroupExtensionPythonT<OriginGroupExtension>>;
}
