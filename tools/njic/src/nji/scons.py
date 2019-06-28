import SCons.Tool
import SCons.Util
import os


__all__ = [
    'exists',
    'generate',
]


NjiAction = SCons.Action.Action('$NJICCOM', '$NJICCOMSTR')


def NjiEmitter(target, source, env):
    targetDir, targetFile = os.path.split(SCons.Util.to_String(target[0]))
    targetName, targetExt = os.path.splitext(targetFile)
    if env['NJICPATHH']:
        targetDir = SCons.Util.to_String(env['NJICPATHH'])
    target.append(os.path.join(targetDir, targetName) + '.h')
    if env['NJICTEMPLATEC']:
        env.Depends(target[0], env['NJICTEMPLATEC'])
    if env['NJICTEMPLATEH']:
        env.Depends(target[1], env['NJICTEMPLATEH'])
    return target, source


class pathopt(object):
    """Copied from javac tool"""
    def __init__(self, opt, var, default=None):
        self.opt = opt
        self.var = var
        self.default = default

    def __call__(self, target, source, env, for_signature):
        path = env[self.var]
        if path and not SCons.Util.is_List(path):
            path = [path]
        if self.default:
            default = env[self.default]
            if default:
                if not SCons.Util.is_List(default):
                    default = [default]
                path = path + default
        if path:
            return [self.opt, os.pathsep.join(map(str, path))]
        else:
            return []


def generate(env):
    c_file, cxx_file = SCons.Tool.createCFileBuilders(env)
    c_file.add_action('.nji', NjiAction)
    c_file.add_emitter('.nji', NjiEmitter)
    env['NJIC'] = 'njic'
    env['NJICFLAGS'] = SCons.Util.CLVar('')
    env['NJICPATHH'] = None
    env['NJICTEMPLATEC'] = None
    env['NJICTEMPLATEH'] = None
    env['NJICCLASSPATH'] = []
    env['_njicpathopt'] = pathopt
    env['_NJICTEMPLATEC'] = '${_njicpathopt("--template-c", "NJICTEMPLATEC")} '
    env['_NJICTEMPLATEH'] = '${_njicpathopt("--template-h", "NJICTEMPLATEH")} '
    env['_NJICCLASSPATH'] = '${_njicpathopt("--classpath", "NJICCLASSPATH", "JAVACLASSPATH")} '
    env['NJICCOM'] = '$NJIC $NJICFLAGS $_NJICTEMPLATEC $_NJICTEMPLATEH $_NJICCLASSPATH --output-c ${TARGETS[0]} --output-h ${TARGETS[1]} $SOURCE'


def exists(env):
    return env.Detect(['njic'])
