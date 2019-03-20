/* auto-generated using nji */
#ifndef __NJI_{{ cls.cname }}_H__
#define __NJI_{{ cls.cname }}_H__


#include "nji.h"


#ifdef __cplusplus
extern "C" {
#endif


/* indexes for looking up field IDs */{% if cls.fields %}
enum { {% for f in cls.fields %}
    JID({{ cls.shortname }}, {{ f.cname }}),{% endfor %}
};{% endif %}


/* indexes for looking up constructor IDs */{% if cls.constructors %}
enum { {% for k in cls.constructors %}
    JID({{ cls.shortname }}, {{ k.cname }}),{% endfor %}
};{% endif %}


/* indexes for looking up method IDs */{% if cls.methods %}
enum { {% for m in cls.methods %}
    JID({{ cls.shortname }}, {{ m.cname }}),{% endfor %}
};{% endif %}


/* {{ cls.shortname }} */
typedef struct {
    NJI_HEAD;{% for f in cls.fields %}
    nji_error (*{{ f.cname }})({{ f.type }} *{% if not f.static %}, jobject{% endif %}, field_mode);{% endfor %}{% for k in cls.constructors %}
    nji_error (*{{ k.cname }})(jobject *{% for a in k.argtypes %}, {{ a }}{% endfor %});{% endfor %}{% for m in cls.methods %}
    nji_error (*{{ m.cname }})({% if m.restype != 'void' %}{{ m.restype }} *{% if not m.static %}, jobject{% endif %}{% for a in m.argtypes %}, {{ a }}{% endfor %}{% elif not m.static %}jobject self{% for a in m.argtypes %}, {{ a }}{% endfor %}{% else %}{% for a in m.argtypes %}{{ a }}{% if not loop.last %},{% endif %}{% else %}void{% endfor %}{% endif %});{% endfor %}
} _{{ cls.shortname }};


extern _{{ cls.shortname }} {{ cls.shortname }};


#ifdef __cplusplus
}
#endif


#endif

