import os

env = Environment()
# will guarantee that SCons will be able to execute any command 
# that you can execute from the command line
env['ENV'] = os.environ

# scons  -Q zmdnet_debug=0 zmdnet_run_time_checks=0 zmdnet_support_ipv6=0 zmdnet_support_ipv4=0

#################################################
# CHECK CMD-LINE COMPILIE OPTIONS
#################################################
print('\nCHECK FOR COMPILIE OPTIONS')
if int(ARGUMENTS.get('zmdnet_debug', True)):
    print('debug build...')
    env.Append(CCFLAGS='-g')
    env.Append(CPPDEFINES=['zmdnet_debug'])
    libname = 'zmdnetd'
else:
    print('release build...')
    env.Append(CCFLAGS='-O2')
    libname = 'zmdnetr'
    
if int(ARGUMENTS.get('zmdnet_run_time_checks', True)):
    print('do run time checks ...')
    env.Append(CPPDEFINES=['zmdnet_run_time_checks'])
else:
    print('not do run time checks ...')
    
if int(ARGUMENTS.get('zmdnet_support_ipv4', True)):
    print('support_ipv4 ...')
    env.Append(CPPDEFINES=['zmdnet_support_ipv4'])
else:
    print('not support_ipv4 ...')
    
if int(ARGUMENTS.get('zmdnet_support_ipv6', True)):
    print('support_ipv6 ...')
    env.Append(CPPDEFINES=['zmdnet_support_ipv6'])
else:
    print('not support_ipv6 ...')
   
#################################################
# CHECK FOR TYPES AND FUNCTIONS
#################################################
print('\nCHECK FOR TYPES AND FUNCTIONS')
conf = Configure(env)
if conf.CheckCHeader('sys/queue.h'):
    env.Append(CPPDEFINES=['have_sys_queue_header'])
    print 'have_sys_queue_header_file'
#     Exit(1)

if conf.CheckCHeader(['sys/socket.h', 'linux/if_addr.h']):
    print 'have_linux_if_addr_header'
    env.Append(CPPDEFINES=['have_linux_if_addr_header'])


    
env = conf.Finish()

# o = env.Object(Glob('./src/adaption/*.c'))
staticlib = env.StaticLibrary(libname, Glob('./src/*/*.c'))

