#!/usr/bin/python


from .jni import *
import subprocess
import re


__all__ = ['JavaP']


class JavaP(object):
    
    javap = '/usr/bin/javap'

    # more specific regex
    re_method = re.compile(r'''(?:\ \ )
        (?:(?P<access>public|private|protected)\ )?
        (?P<modifiers>(?:(?:static|final|abstract|synchronized|native|transient)\ )*)
        (?P<restype>[\w., \$\?<>\[\]]+)(?:\ )
        (?P<name>[\w\$]+)
        \(
        (?P<argtypes>[^)]*)
        \).*;''', re.X)
    re_field = re.compile(r'''(?:\ \ )
        (?:(?P<access>public|private|protected)\ )?
        (?P<modifiers>(?:(?:static|final|abstract|synchronized|native|transient|volatile)\ )*)
        (?P<type>[\w., \$\?<>\[\]]+)(?:\ )
        (?P<name>[\w\$]+);''', re.X)
    re_descriptor = re.compile(r'''(?:\ \ \ \ descriptor:\ )
        (?P<signature>.*)''', re.X)

    @classmethod
    def parse_class(cls, classname, classpath=None):
        cmd = [cls.javap, '-p', '-s']
        classpath = classpath or []
        for p in classpath:
            cmd.extend(['-cp', p])
        cmd.append(classname)
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        clazz = JniClass(classname)

        while True:
            line = p.stdout.readline().decode('utf-8')
            if not line:
                break

            # end of parsing (static initializer block or end of class)
            if line.startswith('  static {') or line.startswith('}'):
                break

            # skip blank lines and first line
            line = line.rstrip()
            if not line or line.startswith('Compiled'):
                continue

            # class
            if line.startswith('public class'):
                name = line.split()[2]
                angle = name.find('<')
                if angle != -1:
                    name = name[:angle]
                if name != clazz.name:
                    raise IOError('Bad Class name')

            # this shouldn't happen
            elif line.startswith('    '):
                raise IOError('Unexpected line')

            # constructor, method or field
            elif line.startswith('  '):

                member_cls = None
                name = signature = None
                kwargs = {}

                # constructor or method
                if '(' in line and ')' in line:

                    # constructor
                    if clazz.name + '(' in line:
                        member_cls = JniConstructor
                        name = clazz.shortname

                    # method
                    else:
                        match = cls.re_method.match(line)
                        if not match:
                            raise IOError('Failed to match method: %s' % line)
                        member_cls = JniMethod
                        name = match.group('name')
                        if 'static' in match.group('modifiers'):
                            kwargs['flags'] = JniMethod.FLAG_STATIC

                # field
                else:
                    match = cls.re_field.match(line)
                    if not match:
                        raise IOError('Failed to match field: %s' % line)  
                    member_cls = JniField
                    name = match.group('name')
                    if 'static' in match.group('modifiers'):
                        kwargs['flags'] = JniField.FLAG_STATIC

                # descriptor
                line = p.stdout.readline().decode('utf-8')
                if not line:
                    raise IOError('Missing descriptor')
                line = line.rstrip()
                match = cls.re_descriptor.match(line)
                if not match:
                    raise IOError('Failed to match descriptor')
                signature = match.group('signature')

                # add member to class
                if member_cls is None:
                    raise IOError('Failed to match class member')
                member_cls(name, signature, **kwargs).add_to(clazz)
    
        # wait for process to end
        out, err = p.communicate()
        if p.returncode != 0:
            raise IOError(str(err).rstrip())
        return clazz


if __name__ == '__main__':
    import sys
    classpath = sys.argv[1]
    classname = sys.argv[2] if len(sys.argv) > 2 else None
    print(JavaP.parse_class(classname, classpath))
