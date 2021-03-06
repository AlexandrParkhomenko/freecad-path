################################################################################
#  Copyright (c) 2017 sliptonic <shopinthewoods@gmail.com>
#  FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY
################################################################################
import FreeCAD
import PathScripts.PathGeom as PathGeom
import PathScripts.PathLog as PathLog
import PySide2


__title__ = "Path UI helper and utility functions"
__author__ = "sliptonic (Brad Collette)"
__doc__ = "A collection of helper and utility functions for the Path GUI."

def translate(context, text, disambig=None):
    return PySide2.QtCore.QCoreApplication.translate(context, text, disambig)

LOGLEVEL = False

if LOGLEVEL:
    PathLog.setLevel(PathLog.Level.DEBUG, PathLog.thisModule())
    PathLog.trackModule(PathLog.thisModule())
else:
    PathLog.setLevel(PathLog.Level.INFO, PathLog.thisModule())

def _getProperty(obj, prop):
    o = obj
    attr = obj
    name = None
    for name in prop.split('.'):
        o = attr
        if not hasattr(o, name):
            break
        attr = getattr(o, name)

    if o == attr:
        PathLog.warning(translate('PathGui', "%s has no property %s (%s))") % (obj.Label, prop, name))
        return (None, None, None)

    #PathLog.debug("found property %s of %s (%s: %s)" % (prop, obj.Label, name, attr))
    return(o, attr, name)

def getProperty(obj, prop):
    '''getProperty(obj, prop) ... answer obj's property defined by its canonical name.'''
    o, attr, name = _getProperty(obj, prop) # pylint: disable=unused-variable
    return attr

def setProperty(obj, prop, value):
    '''setProperty(obj, prop, value) ... set the property value of obj's property defined by its canonical name.'''
    o, attr, name = _getProperty(obj, prop) # pylint: disable=unused-variable
    if o and name:
        setattr(o, name, value)

def updateInputField(obj, prop, widget, onBeforeChange=None):
    '''updateInputField(obj, prop, widget) ... update obj's property prop with the value of widget.
The property's value is only assigned if the new value differs from the current value.
This prevents onChanged notifications where the value didn't actually change.
Gui::InputField and Gui::QuantitySpinBox widgets are supported - and the property can
be of type Quantity or Float.
If onBeforeChange is specified it is called before a new value is assigned to the property.
Returns True if a new value was assigned, False otherwise (new value is the same as the current).
'''
    value = FreeCAD.Units.Quantity(widget.text()).Value
    attr = getProperty(obj, prop)
    attrValue = attr.Value if hasattr(attr, 'Value') else attr
    if not PathGeom.isRoughly(attrValue, value):
        PathLog.debug("updateInputField(%s, %s): %.2f -> %.2f" % (obj.Label, prop, attr, value))
        if onBeforeChange:
            onBeforeChange(obj)
        setProperty(obj, prop, value)
        return True
    return False

class QuantitySpinBox:
    '''Controller class to interface a Gui::QuantitySpinBox.
The spin box gets bound to a given property and supports update in both directions.
   QuatitySpinBox(widget, obj, prop, onBeforeChange=None)
        widget ... expected to be reference to a Gui::QuantitySpinBox
        obj    ... document object
        prop   ... canonical name of the (sub-) property
        onBeforeChange ... an optional callback being executed before the value of the property is changed
'''

    def __init__(self, widget, obj, prop, onBeforeChange=None):
        self.obj = obj
        self.widget = widget
        self.prop = prop
        self.onBeforeChange = onBeforeChange
        attr = getProperty(self.obj, self.prop)
        if attr is not None:
            if hasattr(attr, 'Value'):
                widget.setProperty('unit', attr.getUserPreferred()[2])
            widget.setProperty('binding', "%s.%s" % (obj.Name, prop))
            self.valid = True
        else:
            PathLog.warning(translate('PathGui', "Cannot find property %s of %s") % (prop, obj.Label))
            self.valid = False

    def expression(self):
        '''expression() ... returns the expression if one is bound to the property'''
        if self.valid:
            return self.widget.property('expression')
        return ''

    def updateSpinBox(self, quantity=None):
        '''updateSpinBox(quantity=None) ... update the display value of the spin box.
If no value is provided the value of the bound property is used.
quantity can be of type Quantity or Float.'''
        if self.valid:
            if quantity is None:
                quantity = getProperty(self.obj, self.prop)
            value = quantity.Value if hasattr(quantity, 'Value') else quantity
            self.widget.setProperty('rawValue', value)

    def updateProperty(self):
        '''updateProperty() ... update the bound property with the value from the spin box'''
        if self.valid:
            return updateInputField(self.obj, self.prop, self.widget, self.onBeforeChange)
        return None

