import os

env = Environment()

# will guarantee that SCons will be able to execute any command 
# that you can execute from the command line
env['ENV'] = os.environ

srcfiles = Split("""
                ./src/zmd-net-socket.c
                """)

opt = env.Clone(CCFLAGS = '-O2')
dbg = env.Clone(CCFLAGS = '-g')

o = opt.Object('zmd-net-socket-r', srcfiles)
opt.StaticLibrary('zmdnetr', o)

o = dbg.Object('zmd-net-socket-d', srcfiles)
dbg.StaticLibrary('zmdnetd', o)


