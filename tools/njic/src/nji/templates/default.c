/* auto-generated using nji */
#include <stdlib.h>
#include "{{ include }}"
#include "nji/resolve.h"


/* forward declarations */
static jclass __class__(void);
static jfieldID __field__(int);
static jmethodID __constructor__(int);
static jmethodID __method__(int);{% for f in cls.fields %}
static nji_error {{ cls.shortname }}_{{ f.cname }}({{ f.type }} *{% if not f.static %}, jobject{% endif %}, field_mode);{% endfor %}{% for k in cls.constructors %}
static nji_error {{ cls.shortname }}_{{ k.cname }}(jobject *{% for a in k.argtypes %}, {{ a }}{% endfor %});{% endfor %}{% for m in cls.methods %}
static nji_error {{ cls.shortname }}_{{ m.cname }}({% if m.restype != 'void' %} {{ m.restype }} *{% if not m.static %}, jobject{% endif %}{% for a in m.argtypes %}, {{ a }}{% endfor %}{% elif not m.static %}jobject self{% for a in m.argtypes %}, {{ a }}{% endfor %}{% else %}{% for a in m.argtypes %}{{ a }}{% if not loop.last %},{% endif %}{% else %}void{% endfor %}{% endif %});{% endfor %}


/* class implementation */
_{{ cls.shortname }} {{ cls.shortname }} = {
    .__class__ = __class__,
    .__field__ = __field__,
    .__constructor__ = __constructor__,
    .__method__ = __method__,{% for f in cls.fields %}
    .{{ f.cname }} = {{ cls.shortname }}_{{ f.cname }},{% endfor %}{% for k in cls.constructors %}
    .{{ k.cname }} = {{ cls.shortname }}_{{ k.cname }},{% endfor %}{% for m in cls.methods %}
    .{{ m.cname }} = {{ cls.shortname }}_{{ m.cname }},{% endfor %}
};


/* symbols */{% if cls.fields %}
static FieldSym FieldSymbols[] = { {% for f in cls.fields %}
    { "{{ f.name }}", "{{ f.signature }}", {{ f.flags }} },{% endfor %}
    { NULL, NULL, 0 },
};{% endif %}{% if cls.constructors %}
static ConstructorSym ConstructorSymbols[] = { {% for k in cls.constructors %}
    { "{{ k.signature }}", {{ k.flags }} },{% endfor %}
    { NULL },
};{% endif %}{% if cls.methods %}
static MethodSym MethodSymbols[] = { {% for m in cls.methods %}
    { "{{ m.name }}", "{{ m.signature }}", {{ m.flags }} },{% endfor %}
    { NULL, NULL, 0 },
};{% endif %}
static ClassSym ClassSymbol = {
    "{{ cls.jname }}",
    {% if cls.fields %}FieldSymbols{% else %}NULL{% endif %},
    {% if cls.constructors %}ConstructorSymbols{% else %}NULL{% endif %},
    {% if cls.methods %}MethodSymbols{% else %}NULL{% endif %}
};


/* definitions */{% if cls.fields %}
static jfieldID FieldDefinitions[ENTRIES(FieldSymbols)-1];{% endif %}{% if cls.constructors %}
static jmethodID ConstructorDefinitions[ENTRIES(ConstructorSymbols)-1];{% endif %}{% if cls.methods %}
static jmethodID MethodDefinitions[ENTRIES(MethodSymbols)-1];{% endif %}
static ClassDef ClassDefinition = {
    NULL,
    {% if cls.fields %}FieldDefinitions{% else %}NULL{% endif %},
    {% if cls.constructors %}ConstructorDefinitions{% else %}NULL{% endif %},
    {% if cls.methods %}MethodDefinitions{% else %}NULL{% endif %}
};


/* initializer */
static nji_error init(void) {
    static int done = 0;
    if (!done) {
        nji_error error = resolveClassDef(&ClassDefinition, &ClassSymbol);
        if (error == NJI_OK)
            done = 1;
        return error;
    }
    return NJI_OK;
}


/* class */    
static jclass __class__(void) {
    init();
    return ClassDefinition.Class;
}


/* field ID */
static jfieldID __field__(int index) {
    init();
    return ClassDefinition.Fields[index];
}


/* constructor ID */
static jmethodID __constructor__(int index) {
    init();
    return ClassDefinition.Constructors[index];
}


/* method ID */
static jmethodID __method__(int index) {
    init();
    return ClassDefinition.Methods[index];
}


/* fields */{% for f in cls.fields %}
static nji_error {{ cls.shortname }}_{{ f.cname }}({{ f.type }} *value{% if not f.static %}, jobject self{% endif %}, field_mode mode) {
    nji_error error = init();
    return error ? error : {% if f.optional %}ClassDefinition.Fields[{{ loop.index0 }}] ? {% endif %}GetOrSet{% if f.static %}Static{% endif %}{{ f.modifier }}Field(value, {% if f.static %}ClassDefinition.Class{% else %}self{% endif %}, ClassDefinition.Fields[{{ loop.index0 }}], mode){% if f.optional %} : NJI_UNRESOLVED{% endif %};
}{% endfor %}


/* constructors */{% for k in cls.constructors %}
static nji_error {{ cls.shortname }}_{{ k.cname }}(jobject *result{% for a in k.argtypes %}, {{ a }} arg{{ loop.index }}{% endfor %}) {
    nji_error error = init();
    return error ? error : {% if k.optional %}ClassDefinition.Constructors[{{ loop.index0 }}] ?{% endif %}NewObject(result, ClassDefinition.Class, ClassDefinition.Constructors[{{ loop.index0 }}]{% for a in k.argtypes %}, arg{{ loop.index }}{% endfor %}){% if k.optional %} : NJI_UNRESOLVED{% endif %};
}{% endfor %}


/* methods */{% for m in cls.methods %}
static nji_error {{ cls.shortname }}_{{ m.cname }}({% if m.restype != 'void' %}{{ m.restype }} *result{% if not m.static %}, jobject self{% endif %}{% for a in m.argtypes %}, {{ a }} arg{{ loop.index }}{% endfor %}{% elif not m.static %}jobject self{% for a in m.argtypes %}, {{ a }} arg{{ loop.index }}{% endfor %}{% else %}{% for a in m.argtypes %}{{ a }} arg{{ loop.index }}{% if not loop.last %},{% endif %}{% else %}void{% endfor %}{% endif %}) {
    nji_error error = init();
    return error ? error : {% if m.optional %}ClassDefinition.Methods[{{ loop.index0 }}] ?{% endif %}Call{% if m.static %}Static{% endif %}{{ m.modifier }}Method({% if m.restype != 'void' %}result, {% endif %}{% if m.static %}ClassDefinition.Class{% else %}self{% endif %}, ClassDefinition.Methods[{{ loop.index0 }}]{% for a in m.argtypes %}, arg{{ loop.index }}{% endfor %}){% if m.optional %} : NJI_UNRESOLVED{% endif %};
}{% endfor %}

