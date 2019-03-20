#!/usr/bin/python


import re


__all__ = ['JniClass', 'JniConstructor', 'JniField', 'JniMethod', 'JniAll']


JniAll = object()


class JniClass(object):

    SKIP_FIELDS = 1
    SKIP_CONSTRUCTORS = 2
    SKIP_METHODS = 4

    def __init__(self, name, altname=None, constructors=None, fields=None, methods=None, force=False):
        self.name = self.normalise(name)
        self.altname = altname
        self.fields = fields or []
        self.constructors = constructors or []
        self.methods = methods or []
        self.force = force

    def __str__(self):
        s = [self.name]
        for f in self.fields:
            s.append('  %s' % f)
        for m in self.methods:
            s.append('  %s' % m)
        return '\n'.join(s)

    @property
    def shortname(self):
        return self.altname or self.name.rsplit('.', 1)[-1].replace('$', '_')

    @property
    def cname(self):
        return self.name.replace('.', '_').replace('$', '_')

    @property
    def jname(self):
        return self.name.replace('.', '/')

    @staticmethod
    def normalise(name):
        if '/' in name:
            return '.'.join(name.split('/'))
        return name

    def add_field(self, field):
        self.fields.append(field)

    def add_constructor(self, constructor):
        self.constructors.append(constructor)

    def add_method(self, method):
        self.methods.append(method)

    def _reduce_members(self, javap, nji):
        if nji is JniAll:
            for j in javap:
                j.add_to(self)
            return
        for n in nji:
            found = 0
            for j in javap:
                if j == n:
                    j.reduce(j, n).add_to(self)
                    found += 1
            if not found:
                if n.force:
                    n.add_to(self)
                else:
                    raise ValueError("Cannot find %s" % n)

    @classmethod
    def reduce(cls, javap, nji):
        if javap.name != nji.name:
            raise ValueError("Classes don't match")

        # new reduced class
        new = cls(javap.name, altname=nji.altname)
        new._reduce_members(javap.fields, nji.fields)
        new._reduce_members(javap.constructors, nji.constructors)
        new._reduce_members(javap.methods, nji.methods)
        return new

    def validate(self):
        if not self.name:
            raise ValueError("Must have name")
        for field in self.fields:
            field.validate()
        for constructor in self.constructors:
            constructor.validate()
        for field in self.fields:
            field.validate()

    def uniqify(self):
        members = self.fields + self.constructors + self.methods
        while members:
            i, m0 = 0, members.pop(0)
            for m1 in members:
                if m0.cname == m1.cname:
                    i += 1
                    m1.altname = m1.cname + 'V%d' % i


JniAll = object()
"""Sentinel for 'all' members"""


class JniMember(object):

    FLAG_STATIC = 1
    FLAG_OPTIONAL = 2

    re_sig = re.compile(r'(?:\((?P<argtypes>[^)]*)\))?(?P<restype>.*)')
    re_type = re.compile(r'\[*(?:[ZBCSIJFDV]|L[^;]+;)')

    types = {
        'Z': 'jboolean',
        'B': 'jbyte',
        'C': 'jchar',
        'S': 'jshort',
        'I': 'jint',
        'J': 'jlong',
        'F': 'jfloat' ,
        'D': 'jdouble',
        'L': 'jobject',
        'V': 'void',
    }

    type_modifiers = {
        'jboolean': 'Boolean',
        'jbyte': 'Byte',
        'jchar': 'Char',
        'jshort': 'Short',
        'jint': 'Int',
        'jlong': 'Long',
        'jfloat' : 'Float',
        'jdouble': 'Double',
        'void': 'Void',
    }

    def __init__(self, name, signature=None, altname=None, flags=0, force=False):
        self.name = name
        self.signature = signature
        self.altname = altname
        self.flags = flags
        self.force = force

    @property
    def static(self):
        return self.flags & self.FLAG_STATIC

    @property
    def optional(self):
        return self.flags & self.FLAG_OPTIONAL

    @property
    def restype(self):
        return self.get_types()[0]

    @property
    def argtypes(self):
        return self.get_types()[1]

    @property
    def modifier(self):
        return self.type_modifiers.get(self.restype, "Object")

    @property
    def cname(self):
        name = self.altname or self.name
        return name.replace('$', '_')

    def validate(self):
        """Must have a non-null name and signature"""
        if not self.name:
            raise ValueError("Must have a name")
        if not self.signature:
            raise ValueError("Must have a signature")

    def __eq__(self, other):
        if other.name != self.name:
            return False
        if other.signature and self.signature:
            if other.signature != self.signature:
                return False
        return True

    def _get_type(self, typesig):
        """Parse single type signature to jni type"""
        dimensions = typesig.count('[')
        if dimensions > 1:
            return 'jarray'
        typesig = typesig.lstrip('[')
        typebyte, classname = typesig[0], typesig[1:]
        jnitype = self.types[typebyte]
        if jnitype == 'jobject' and not dimensions:
            if classname == 'java/lang/Class;':
                jnitype = 'jclass'
            elif classname == 'java/lang/String;':
                jnitype = 'jstring'
            elif classname == 'java/lang/Throwable;':
                jnitype = 'jthrowable'
        if jnitype != 'void' and dimensions:
            jnitype += 'Array'
        return jnitype
    
    def _get_types(self):
        """Parse complete signature to jni type(s)"""
        match = self.re_sig.match(self.signature)
        if not match:
            raise ValueError('Bad signature')
        _argtypes, _restype = match.groups()
        argtypes = []
        while _argtypes:
            match = self.re_type.match(_argtypes)
            if not match:
                raise ValueError('Bad signature')
            argtypes.append(self._get_type(match.group(0)))
            _argtypes = _argtypes[match.end():]
        match = self.re_type.match(_restype)
        if not match:
            raise ValueError('Bad signature')
        restype = self._get_type(match.group(0))
        return restype, argtypes

    def get_types(self):
        if hasattr(self, '_types'):
            return self._types
        if not self.signature:
            return None, None
        self._types = self._get_types()
        return self._types

    @classmethod
    def reduce(cls, javap, nji):
        return cls(
            javap.name,
            javap.signature or nji.signature, 
            altname=nji.altname,
            flags=javap.flags | nji.flags,
            force=nji.force
        )


class JniField(JniMember):

    def __init__(self, *args, **kwargs):
        super(JniField, self).__init__(*args, **kwargs)
        if self.argtypes:
            raise ValueError('Field cannot have arguments')

    def __str__(self):
        return '%s%s %s' % (
            'static ' if self.static else '',
            self.name,
            self.signature
        ) 

    def add_to(self, klass):
        klass.add_field(self)

    @property
    def type(self):
        return self.restype


class JniConstructor(JniMember):

    def __str__(self):
        return '%s<init>() %s' % (
            'static ' if self.static else '',
            self.signature
        ) 

    def add_to(self, klass):
        klass.add_constructor(self)


class JniMethod(JniMember):

    def __str__(self):
        return '%s%s() %s' % (
            'static ' if self.static else '',
            self.name,
            self.signature
        ) 

    def add_to(self, klass):
        klass.add_method(self)
