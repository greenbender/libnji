# vi:syntax=python

Import('env')


libnji = SConscript(dirs=['lib'], exports='env')
test = SConscript(dirs=['test'], exports='env')


Return('libnji test')
