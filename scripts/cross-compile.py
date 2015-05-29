#!/usr/bin/env python2.7

import sys
import os
import argparse
import subprocess
from py_lightship.TemplateProcessor import TemplateProcessor


class Target(object):

    def __init__(self):

        # set up argument parser
        parser = argparse.ArgumentParser(description='Assists in cross compiling')
        parser.add_argument('--platform', help='The name of the platform to compile for. If this argument is not provided, the system platform is used. This ends up being written to CMAKE_SYSTEM_NAME', type=str)
        parser.add_argument('--output-name', help='The output folder name. By default the name will be "lightship-<version>-<triplet>', type=str)
        parser.add_argument('--set-version', help='Sets the version string, e.g. "0.5.2-beta"', type=str)
        parser.add_argument('--build', help='The build number. This will be part of the archive file name if --output-name is not specified', type=str)
        parser.add_argument('--compiler-root', help='The root path of the compiler', type=str)
        parser.add_argument('--c-compiler', help='Full path to the C compiler to use', type=str)
        parser.add_argument('--cxx-compiler', help='Full path to the C++ compiler to use', type=str)
        parser.add_argument('--rc-compiler', help='Full path to the Windows resource compiler if compiling for Windows', type=str)
        parser.add_argument('--cmake', help='Additional options to pass to CMake. Note that "-D" is prepended automatically', type=str, action='append')
        parser.add_argument('--make', help='The command to use to "make" the target (Windows will use nmake, e.g.', type=str)
        parser.add_argument('--install', help='The command to use to install the target', type=str)
        parser.add_argument('--compress', help='The compression to use.', type=str, choices=['bz2', 'gz',  'xz', '7z'])
        args = parser.parse_args()
   
        # verify arguments
        if args.set_version is None:
            print('Please specify a version string')
            sys.exit(1)
        if args.compiler_root is None:
            print('Please specify the compiler root, e.g. "/usr/bin/x86_64-pc-linux-gnu"')
            sys.exit(1)
        if args.make is None:
            print('Please specify a command to make')
            sys.exit(1)

        # determine triplet
        self.triplet = args.compiler_root.split('/')[-1]
        print('triplet: {0}'.format(self.triplet))

        # determine output folder name
        build_folder_name = 'lightship-' + self.triplet
        if args.output_name is None:
            if args.build is None:
                args.build = ""
            if len(args.build) > 0:
                args.build = "-" + args.build
            args.output_name = 'lightship-' + args.set_version + args.build + '-' + self.triplet
        else
            build_folder_name = args.output_name

        # determine compilers
        if args.c_compiler is None:
            args.c_compiler = args.compiler_root + '-gcc'
        if args.cxx_compiler is None:
            args.cxx_compiler = args.compiler_root + '-g++'
        if args.rc_compiler is None:
            args.rc_compiler = args.compiler_root + '-windres'
        print('C compiler: {0}\nC++ compiler: {1}\nRC compiler (if applicable): {2}'.format(args.c_compiler, args.cxx_compiler, args.rc_compiler))

        # build target object
        self.platform                 = args.platform
        self.version                  = args.set_version
        self.build                    = args.build
        self.compiler_root            = args.compiler_root
        self.c_compiler               = args.c_compiler
        self.cxx_compiler             = args.cxx_compiler
        self.rc_compiler              = args.rc_compiler
        self.additional_cmake_args    = args.cmake
        self.make_cmd                 = args.make
        self.install_cmd              = args.install
        self.compress_type            = args.compress
        self.output_name              = args.output_name
        self.build_folder_name        = build_folder_name
        self.binary_path              = os.path.abspath(os.path.join('cross-build/build', self.build_folder_name))
        self.install_prefix           = os.path.abspath(os.path.join('cross-build/install', self.build_folder_name))
        self.archive_path             = os.path.abspath('cross-build/dist')
        self.toolchain_file = os.path.abspath(os.path.join('scripts/cmake/toolchains', 'toolchain-' + self.triplet + '.cmake'))

    def generate(self):

        t = TemplateProcessor()

        t.ensure_path(self.binary_path)
        t.ensure_path(self.install_prefix)

        # if not cross compiling, no need to generate templates
        if self.platform is None:
            return

        # destination path is the source directory so we have access to every folder
        # we are sourcing our template files from scripts/cmake
        t.set_destination_path('.')
        t.set_source_path('scripts/cmake')

        # There are two template files to parse. The toolchains file is placed in
        # scripts/cmake/toolchains and is .gitignored
        # The install-arch script needs to land in the binary directory of the target
        t.add_template('install-arch.cmake.in', os.path.join(self.binary_path, 'install-arch.cmake'))
        t.add_template('toolchain-arch.cmake.in', self.toolchain_file)

        t.add_substitution('ARCH_NAME', self.platform)
        t.add_substitution('BUILD_DIR', self.binary_path)
        t.add_substitution('C_COMPILER', self.c_compiler)
        t.add_substitution('RC_COMPILER', self.rc_compiler)
        t.add_substitution('ROOT_PATH', self.compiler_root)
        t.add_substitution(self.triplet + '_INSTALL_PREFIX', self.install_prefix)

        t.process_all()

    def configure(self):

        cmake_call = list()
        cmake_call.append('cmake')

        # cross compiling?
        if not self.platform is None:
            cmake_call.append('-DCMAKE_TOOLCHAIN_FILE=' + self.toolchain_file)
        else:
            print('Not cross compiling...')

        cmake_call.append('-DCMAKE_INSTALL_PREFIX=' + self.install_prefix)
        if not self.additional_cmake_args is None:
            for arg in self.additional_cmake_args:
                cmake_call.append('-D' + arg)

        # source CMakeLists.txt from project's source directory
        cmake_call.append(os.path.abspath('.'))

        # working directory in build dir
        process = subprocess.Popen(cmake_call, cwd=self.binary_path)
        process.wait()
        if not process.returncode == 0:
            raise Exception('Configure phase failed')

    def make(self):
        process = subprocess.Popen(self.make_cmd.split(' '), cwd=self.binary_path)
        process.wait()
        if not process.returncode == 0:
            raise Exception('Compile phase failed')

    def install(self):
        if not self.install_cmd is None:
            process = subprocess.Popen(self.install_cmd.split(' '), cwd=self.binary_path)
            process.wait()
            if not process.returncode == 0:
                raise Exception('Install failed')

    def compress(self):
        if self.compress_type is None:
            return

        if self.install_cmd is None:
            raise Exception('Can`t compress without installing! Please use --install.')

        if self.compress_type == '7z':
            cmd = '7za a -t7z -m0=lzma -mx=9 -mfb=64 -md=32m -ms=on'.split(' ')
            ext = '7z'
        elif self.compress_type == 'gz':
            cmd = 'tar --gzip -cf'.split(' ')
            ext = 'tar.gz'
        elif self.compress_type == 'bz2':
            cmd = 'tar --bzip2 -cf'.split(' ')
            ext = 'tar.bz2'
        elif self.compress_type == 'xz':
            cmd = 'tar --xz -cf'.split(' ')
            ext = 'tar.xz'
        else:
            raise Exception('ERROR: Don`t know how to compress')

        # 7z and tar all have the output folder name in this order
        cmd.append(os.path.abspath(os.path.join(self.archive_path, self.output_name + '.' + ext)))
        cmd.append(self.build_folder_name)

        # compress installed targets
        process = subprocess.Popen(cmd, cwd=os.path.abspath(os.path.join(self.install_prefix, '..')))
        process.wait()
        if not process.returncode == 0:
            raise Exception('Compress phase failed')


if __name__ == '__main__':

    if not os.path.isdir('scripts/cmake'):
        print('Please execute this script from the source directory')
        sys.exit(1)

    target = Target()
    target.generate()
    target.configure()
    target.make()
    target.install()
    target.compress()

    sys.exit(0)

