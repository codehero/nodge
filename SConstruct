#Default construction environment

import os

mydir = os.getcwd()

#Set default C++ building flags for both libraries and executables
default_env = Environment(ENV = os.environ)
default_env.Append(CPPPATH = [mydir + '/include'])
default_env.Append(CCFLAGS = ' -std=gnu++14 -Wall -pedantic ')
default_env.Append(CCFLAGS = ' -O0 -g')

default_env.LibDest = mydir + '/lib'
default_env.BinDest = mydir + '/bin'
default_env.IncDest = mydir + '/include'

#Generate CPP files from XML schemas
def fun(target, source, env):
	gen_dir = env['ENV']['GEN_DIR'];
	gen_dir = gen_dir + '/';
	header_dir = mydir + '/include' + gen_dir;
	env.Execute(str(source[0]) + '|' + 'cpp_gen --cpppath .' + gen_dir + ' --headerpath ' + header_dir)
	return None

gen_bld = Builder(action = fun);
default_env['BUILDERS']['GenCPP'] = gen_bld;

#Set linking flags for executables
bin_env = default_env.Clone()
bin_env.Append(LIBPATH = [bin_env.LibDest])
bin_env.Append(LINKFLAGS = ['-Wl,-rpath,'+bin_env.LibDest])

#set linking flags for libraries
lib_env = default_env.Clone()
lib_env.Append(LINKFLAGS = ['-fPIC'])


SConscript('src/SConscript', exports='bin_env lib_env', variant_dir='build', duplicate=0)
#SConscript('unit/SConscript', exports='bin_env lib_env', variant_dir='unit_build', duplicate=0)
