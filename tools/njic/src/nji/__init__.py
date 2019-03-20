#!/usr/bin/python

from jni import *
from javap import *
import json
import os


__all__ = [
    'template_h',
    'template_c',
    'parse',
]


# default locations for templates
_root = os.path.dirname(__file__)
_templates = os.path.join(_root, 'templates')
template_h = os.path.join(_templates, 'default.h')
template_c = os.path.join(_templates, 'default.c')


def _nji_to_class(data):
    """Parse an NJI file and return a JniClass"""

    name = data.pop('class')
    fields = data.pop('fields', [])
    constructors = data.pop('constructors', [])
    methods = data.pop('methods', [])

    clazz = JniClass(name, **data)

    # fields
    if isinstance(fields, list):
        for field in fields:
            name = field.pop('name')
            field.setdefault('flags', 0)
            if field.pop('static', False):
                field['flags'] |= JniField.FLAG_STATIC
            if field.pop('optional', False):
                field['flags'] |= JniField.FLAG_OPTIONAL
            JniField(name, **field).add_to(clazz)
    elif fields == '*':
        clazz.fields = JniAll

    # constructors
    if isinstance(constructors, list):
        for constructor in constructors:
            constructor.setdefault('flags', 0)
            if constructor.pop('optional', False):
                constructor['flags'] |= JniConstructor.FLAG_OPTIONAL
            JniConstructor(clazz.shortname, **constructor).add_to(clazz)
    elif constructors == '*':
        clazz.constructors = JniAll

    # methods
    if isinstance(methods, list):
        for method in methods:
            name = method.pop('name')
            method.setdefault('flags', 0)
            if method.pop('static', False):
                method['flags'] |= JniMethod.FLAG_STATIC
            if method.pop('optional', False):
                method['flags'] |= JniMethod.FLAG_OPTIONAL
            JniMethod(name, **method).add_to(clazz)
    elif methods == '*':
        clazz.methods = JniAll

    return clazz


def _javap_to_class(data, classpath=None):
    """Parse the output of javap for a java class and return a JniClass"""

    classname = data['class']

    # build classpath
    classpath = classpath or []
    extra = data.get('classpath')
    if extra:
        if isinstance(extra, list):
            classpath.extend(extra)
        else:
            classpath.append(extra)

    # parse the complete class
    clazz_full = JavaP.parse_class(classname, classpath)

    # parse the partial class
    clazz_partial = _nji_to_class(data)

    return JniClass.reduce(clazz_full, clazz_partial)


def parse(fd, classpath=None):
    """Parse an NJI file and generate a JniClass based on the NJI file itself
    and any additional information that can be retrieved from javap"""
    
    data = json.load(fd)
    force = data.get('force', False)

    # create class directly
    if force:
        clazz = _nji_to_class(data)

    # create class using javap
    else:
        clazz = _javap_to_class(data, classpath=classpath)
        
    if clazz:
        clazz.validate()
        clazz.uniqify()

    return clazz
