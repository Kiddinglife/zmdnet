import os

Help("\nType: 'scons program' to build the production program.\n")

env = Environment()

# will guarantee that SCons will be able to execute any command 
# that you can execute from the command line
env['ENV'] = os.environ

# libname = 'zmdnetd'
# cppdefines = []
# for key, value in ARGLIST:
#     if key == 'define':
#         cppdefines.append(value)
#     elif key == 'debug':
#         if not int(value):
#             env.Append(CCFLAGS = '-O2')
#             libname = 'zmdnetr'  
        
vars = Variables(None, ARGUMENTS)
vars.Add('RELEASE', 'Set to 1 to build for release', 0)
# vars.AddVariables(
#     ('RELEASE', 'Set to 1 to build for release', 0),
#     ('CONFIG', 'Configuration file', '/etc/my_config'),
#     BoolVariable('warnings', 'compilation with -Wall and similiar', 1),
#     EnumVariable('debug', 'debug output and symbols', 'no',
#                allowed_values=('yes', 'no', 'full'),
#                map={}, ignorecase=0),  # case sensitive
#     ListVariable('shared',
#                'libraries to build as shared libraries',
#                'all',
#                names=''),
#     PackageVariable('x11',
#                   'use X11 installed here (yes = search some places)',
#                   'yes'),
#     PathVariable('qtdir', 'where the root of Qt is installed'),
#)

VariantDir('build', 'src', duplicate=0)
env = Environment(variables=vars,
                  CPPDEFINES={'RELEASE_BUILD' : '${RELEASE}'})
Help(vars.GenerateHelpText(env))

unknown = vars.UnknownVariables()
if unknown:
    print("Unknown variables: %s" % unknown.keys())
    Exit(1)

v = int(ARGUMENTS.get('RELEASE_BUILD', 0))
if int(v):
    env.Append(CCFLAGS='-O2')
    libname = 'zmdnetr'
else:
    env.Append(CCFLAGS='-g')
    libname = 'zmdnetd'
    
srcfiles = Split("""
                ./src/sys/zmd-net-socket.c
                """)
o = env.Object(srcfiles)
staticlib = env.StaticLibrary(libname, o)

# usually install to /usr/local/bin or /usr/local/lib
env.Install('build/obj', o)
env.Install('build/lib', staticlib)

# Clean(env,'',)
# opt = env.Clone(CCFLAGS = '-O2')
# dbg = env.Clone(CCFLAGS = '-g')
# 
# o = opt.Object('zmd-net-socket-r', srcfiles)
# opt.StaticLibrary('zmdnetr', o)
# 
# o = dbg.Object('zmd-net-socket-d', srcfiles)
# dbg.StaticLibrary('zmdnetd', o)

