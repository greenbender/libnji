### Native Java Interface (code generator) ####

njic is a C source code generator that can be used to turn NJI files into C
source and headers files that can be used with libnji.

The idea is that all of the boiler-plate code that is typically required when
using the JNI (Java Native Interface) is taken care of using the code generator
and the libnji library.

This allows the developer to use simple and consistent code to call into the
Java Virtual Machine.


#### njic Command ####

See the usage with `njic --help`

The output of njic depends on the template used. By default a C source and
header file are generated for each NJI file (which defines a single Java
class). The header file gives access to a single extern struct that is the C
version a Java class, and provides a means of accessing all of the
constructors, fields, and methods of the class the were specified in the NJI
file. A means to access the jclass, jfieldID's, and jmethodID's is also
provided.


#### NJI JSON Fields ####

It's probably easiest to supply a documented example. The comments are for
information only. The actual NJI file is JSON and as such does not support
comments:

    {
        # The name of the java class. JNI (slash) and Binary (dot) notation
        # both work but by convention JNI notation is preferred.
        "class": "com/example/Example",

        # An optional alternate name for the class in C. Use this field if you
        # have a class name conflict or simply want to use a different name. By
        # default the name will be the actual class name (for nested classes
        # any '$' in the Java class name are replaced with '_'). In this
        # example it would have been 'Example' if altname wasn't supplied. 
        "altname": "Eg",

        # An optional additional classpath to use to lookup this class for the
        # purposes of parsing out its members with javap.
        "classpath": "/opt/jars/example.jar",

        # If the class is not available for look up via classpath then you can
        # force njic to generate code with using javap. When this is done all
        # of the class members *must* be fully specified, since javap cannot be
        # used to obtain signatures etc. This field defaults to false.
        "force": false,

        # An optional list of all of class constuctors that you want to access
        # from C. If you don't need to access any constructors you can leave
        # this field out. If you want all of the class' constructors you can
        # specify '*' (this obviously won't work if 'force' is set to true).
        "constructors": [
            {
                # JNI signature for the constructor. This field is required for
                # constructors since constructors have no 'name' (they are
                # acutally all called '<init>').
                "signature": "()V",

                # Optional alternate name for the constructor in C. By
                # default the constructor will have the same name as the
                # 'class'. Use this field if you have multiple constructors, or
                # simply want a different name. If there is a name collision
                # between any of the members of a class the members will be
                # have 'V1', 'V2', ... appended to make them unique (it is best
                # to avoid this).
                "altname": "ExampleDefault",

                # Optional flag that specifies that the constructor is
                # optional. If a constructor is deprecated or different across
                # different versions of a library or framework then you can use
                # this to attempt to lookup the constructor and *not* 'fail' to
                # resolve the class if it is not found. This flag defaults to
                # false. NOTE: this flag affects runtime behaviour.
                "optional": false,

                # Optional flag the says that we don't expect javap to find the
                # constructor in the parsed out class. If this flag is set then
                # regardless of whether the constructor is found by javap code
                # will be generated to resolve and access it from C. This flag
                # defaults to false. NOTE: this flag affects code generation.
                "force": false
            },
            ...
        ],

        # An optional list of all of class fields that you want to access from
        # C. If you don't need to access any class fields you can leave this
        # field out. If you want all of the class' fields you can specify '*'
        # (this obviously won't work if 'force' is set to true).
        "fields": [
            {
                # The name of the field.
                "name": "MAX_VALUE",

                # Optional JNI signature for the field. This field is required
                # if the 'force' flag is set (for the field or at the class
                # level), otherwise, we expect that javap will be able to
                # determine the signature from the name.
                "signature": "I",

                # Optional flag for specifying that the field is static. This
                # flag *may* be required if the 'force' flag is set (for the
                # field or at the class level), otherwise, we expect that javap
                # will be able to determine the whether of not the field is
                # static.  This flags default to false.
                "static": true,

                # Optional alternate name for the field in C. Use this field
                # to resolve name collisions or if you simply want a different
                # name. If there is a name collision between any of the members
                # of a class the members will be have 'V1', 'V2', ... appended
                # to make them unique (it is best to avoid this).
                "altname": "MAX",

                # Optional flag that specifies that the field is optional. If a
                # field is deprecated or different across different versions of
                # a library or framework then you can use this to attempt to
                # lookup the field and *not* 'fail' to resolve the class if it
                # is not found. This flag defaults to false. NOTE: this flag
                # affects runtime behaviour.
                "optional": false,

                # Optional flag the says that we don't expect javap to find the
                # field in the parsed out class. If this flag is set then
                # regardless of whether the field is found by javap code will
                # be generated to resolve and access it from C. This flag
                # defaults to false. NOTE: this flag affects code generation.
                "force": false
            },
            ...
        ],

        # An optional list of all of class methods that you want to access from
        # C. If you don't need to access any class methods you can leave this
        # field out. If you want all of the class' methods you can specify '*'
        # (this obviously won't work if 'force' is set to true).
        "methods": [
            {
                # The name of the method.
                "name": "getValue",

                # Optional JNI signature for the method. This field is required
                # if the 'force' flag is set (for the method or at the class
                # level), otherwise, we expect that javap will be able to
                # determine the signature from the name. If the method is
                # overloaded you may want to supply the signature as a means of
                # specifying the which version of the method you want. If a
                # method is overloaded and a signature is not provided all
                # versions of the method are used and the names on the methods
                # will be modified to make them unique by appending 'V1', 'V2',
                # ... etc (it is best to avoid this).
                "signature": "(V)I",

                # Optional flag for specifying that the method is static. This
                # flag *may* be required if the 'force' flag is set (for the
                # field or at the class level), otherwise, we expect that javap
                # will be able to determine the whether of not the method is
                # static.  This flags default to false.
                "static": true,

                # Optional alternate name for the method in C. Use this field
                # to resolve name collisions or if you simply want a different
                # name. If there is a name collision between any of the members
                # of a class the members will be have 'V1', 'V2', ... appended
                # to make them unique (it is best to avoid this).
                "altname": "value",

                # Optional flag that specifies that the method is optional. If a
                # method is deprecated or different across different versions of
                # a library or framework then you can use this to attempt to
                # lookup the field and *not* 'fail' to resolve the class if it is
                # not found. This flag defaults to false. NOTE: this flag
                # affects runtime behaviour.
                "optional": false,

                # Optional flag the says that we don't expect javap to find the
                # method in the parsed out class. If this flag is set then
                # regardless of whether the method is found by javap code will
                # be generated to resolve and access it from C. This flag
                # defaults to false. NOTE: this flag affects code generation.
                "force": false
            },
            ...
        ]
    }
