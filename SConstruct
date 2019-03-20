# vi:syntax=python

import os


env = Environment(tools=['default', 'njic'])


# default flags
env.Append(CCFLAGS=['-pedantic', '-Wall'])
env.Append(CPPDEFINES=['__DEBUG__'])
env.Append(CPPPATH=[env.Dir('include')])


libnji, test = SConscript('SConscript', exports='env')


# platform specific (change this if required)
JDK_HOME = os.getenv('JDK_HOME', '/usr/lib/jvm/java-9-openjdk-amd64')
env.Append(CPPPATH=[os.path.join(JDK_HOME, 'include'), os.path.join(JDK_HOME, 'include', 'linux')])
env.Append(LIBPATH=[os.path.join(JDK_HOME, 'lib', 'server')])
env.Append(RPATH=[os.path.join(JDK_HOME, 'lib', 'server')])
env.Append(LIBS=['jvm', 'pthread'])
