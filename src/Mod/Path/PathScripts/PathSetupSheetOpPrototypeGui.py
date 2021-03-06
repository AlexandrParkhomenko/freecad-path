################################################################################
#  Copyright (c) 2018 sliptonic <shopinthewoods@gmail.com>
#  FreeCAD LICENSE IS LGPL3 WITHOUT ANY WARRANTY
################################################################################
import FreeCAD
import PathScripts.PathLog as PathLog
import PathScripts.PathSetupSheetOpPrototype as PathSetupSheetOpPrototype

from PySide2 import QtCore, QtGui, QtWidgets

__title__ = "Setup Sheet Editor"
__author__ = "sliptonic (Brad Collette)"
__doc__ = "Task panel editor for a SetupSheet"

# Qt translation handling
def translate(context, text, disambig=None):
    return QtCore.QCoreApplication.translate(context, text, disambig)

LOGLEVEL = False

if LOGLEVEL:
    PathLog.setLevel(PathLog.Level.DEBUG, PathLog.thisModule())
    PathLog.trackModule(PathLog.thisModule())
else:
    PathLog.setLevel(PathLog.Level.INFO, PathLog.thisModule())

class _PropertyEditor(object):
    '''Base class of all property editors - just outlines the TableView delegate interface.'''
    def __init__(self, prop):
        self.prop = prop

    def widget(self, parent):
        '''widget(parent) ... called by the delegate to get a new editor widget.
        Must be implemented by subclasses and return the widget.'''
        pass # pylint: disable=unnecessary-pass
    def setEditorData(self, widget):
        '''setEditorData(widget) ... called by the delegate to initialize the editor.
        The widget is the object returned by widget().
        Must be implemented by subclasses.'''
        pass # pylint: disable=unnecessary-pass
    def setModelData(self, widget):
        '''setModelData(widget) ... called by the delegate to store new values.
        Must be implemented by subclasses.'''
        pass # pylint: disable=unnecessary-pass

class _PropertyEnumEditor(_PropertyEditor):
    '''Editor for enumeration values - uses a combo box.'''

    def widget(self, parent):
        PathLog.track(self.prop.name, self.prop.getEnumValues())
        return QtWidgets.QComboBox(parent)

    def setEditorData(self, widget):
        widget.clear()
        widget.addItems(self.prop.getEnumValues())
        if self.prop.getValue():
            index = widget.findText(self.prop.getValue(), QtCore.Qt.MatchFixedString)
            if index >= 0:
                widget.setCurrentIndex(index)

    def setModelData(self, widget):
        self.prop.setValue(widget.currentText())


class _PropertyBoolEditor(_PropertyEditor):
    '''Editor for boolean values - uses a combo box.'''

    def widget(self, parent):
        return QtWidgets.QComboBox(parent)

    def setEditorData(self, widget):
        widget.clear()
        widget.addItems(['false', 'true'])
        if not self.prop.getValue() is None:
            index = 1 if self.prop.getValue() else 0
            widget.setCurrentIndex(index)

    def setModelData(self, widget):
        self.prop.setValue(widget.currentText() == 'true')

class _PropertyStringEditor(_PropertyEditor):
    '''Editor for string values - uses a line edit.'''

    def widget(self, parent):
        return QtWidgets.QLineEdit(parent)

    def setEditorData(self, widget):
        text = '' if self.prop.getValue() is None else self.prop.getValue()
        widget.setText(text)

    def setModelData(self, widget):
        self.prop.setValue(widget.text())

class _PropertyLengthEditor(_PropertyEditor):
    '''Editor for length values - uses a line edit.'''

    def widget(self, parent):
        return QtWidgets.QLineEdit(parent)

    def setEditorData(self, widget):
        quantity = self.prop.getValue()
        if quantity is None:
            quantity = FreeCAD.Units.Quantity(0, FreeCAD.Units.Length)
        widget.setText(quantity.getUserPreferred()[0])

    def setModelData(self, widget):
        self.prop.setValue(FreeCAD.Units.Quantity(widget.text()))

class _PropertyPercentEditor(_PropertyEditor):
    '''Editor for percent values - uses a spin box.'''

    def widget(self, parent):
        return QtWidgets.QSpinBox(parent)

    def setEditorData(self, widget):
        widget.setRange(0, 100)
        value = self.prop.getValue()
        if value is None:
            value = 0
        widget.setValue(value)

    def setModelData(self, widget):
        self.prop.setValue(widget.value())

class _PropertyIntegerEditor(_PropertyEditor):
    '''Editor for integer values - uses a spin box.'''

    def widget(self, parent):
        return QtWidgets.QSpinBox(parent)

    def setEditorData(self, widget):
        value = self.prop.getValue()
        if value is None:
            value = 0
        widget.setValue(value)

    def setModelData(self, widget):
        self.prop.setValue(widget.value())

class _PropertyFloatEditor(_PropertyEditor):
    '''Editor for float values - uses a double spin box.'''

    def widget(self, parent):
        return QtWidgets.QDoubleSpinBox(parent)

    def setEditorData(self, widget):
        value = self.prop.getValue()
        if value is None:
            value = 0.0
        widget.setValue(value)

    def setModelData(self, widget):
        self.prop.setValue(widget.value())

_EditorFactory = {
        PathSetupSheetOpPrototype.Property: None,
        PathSetupSheetOpPrototype.PropertyBool: _PropertyBoolEditor,
        PathSetupSheetOpPrototype.PropertyDistance: _PropertyLengthEditor,
        PathSetupSheetOpPrototype.PropertyEnumeration: _PropertyEnumEditor,
        PathSetupSheetOpPrototype.PropertyFloat: _PropertyFloatEditor,
        PathSetupSheetOpPrototype.PropertyInteger: _PropertyIntegerEditor,
        PathSetupSheetOpPrototype.PropertyLength: _PropertyLengthEditor,
        PathSetupSheetOpPrototype.PropertyPercent: _PropertyPercentEditor,
        PathSetupSheetOpPrototype.PropertyString: _PropertyStringEditor,
        }

def Editor(prop):
    '''Returns an editor class to be used for the given property.'''
    factory = _EditorFactory[prop.__class__]
    if factory:
        return factory(prop)
    return None
