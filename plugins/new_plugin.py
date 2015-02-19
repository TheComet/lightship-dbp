#!/usr/bin/env python

import sys
import os
import string


class TemplateProcessor(object):
    
    def __init__(self):
        self.__files = list()             # list of tuples containing [file_in, file_out]
        self.__substitutions = dict()      # key,value substitutions to make on the files
        self.__source_path = './'         # a path prefix to a folder containing the input template files
        self.__destination_path = './'    # the destination path
    
    def set_source_path(self, path):
        self.__source_path = path
        return self
    
    def set_destination_path(self, path):
        self.__destination_path = path
        return self
        
    def add_template(self, file_in, file_out):
        if any(file_in == x[0] for x in self.__files):
            return self
        self.__files.append((file_in, file_out))
        return self
    
    def add_substitution(self, key, value):
        final_key = '@' + key + '@'
        if final_key in self.__substitutions:
            return self
        self.__substitutions[final_key] = value
        return self

    def process_all(self):
        ensure_path(self.__destination_path)
        # process all files
        for file_tuple in self.__files:
            print('processing files: (' + file_tuple[0] + ', ' + file_tuple[1] + ')')
            self.process_template(file_tuple[0], file_tuple[1])

    def process_template(self, file_in, file_out):
        # open source template file and destination file, and append path prefixes
        with open(os.path.join(self.__destination_path, file_out), 'w') as f:
            for line in open(os.path.join(self.__source_path, file_in), 'r'):
                f.write(self.__process_line(line))
    
    def __process_line(self, line):
        for key, value in self.__substitutions.iteritems():
            line = line.replace(key, value)
        return line


def ensure_path(path):
    if not os.path.exists(path):
        os.makedirs(path)


def get_user_info(msg, spaces=True, digits=True, special=True, default_info=None):
    special_whitelist = string.letters + string.digits + string.whitespace + '_'
    while True:
        if default_info is None:
            sys.stdout.write(msg + ': ')
        else:
            sys.stdout.write(msg + ' (default: "' + default_info + '"): ')

        line = sys.stdin.readline()
        
        # handle empty inputs
        if len(line) < 3:
            if not default_info is None:
                return default_info
            print('please enter something')
            continue
        # handle spaces
        if not spaces and ' ' in line:
            print('spaces are not allowed')
            continue
        # handle special characters
        if not special and any(not x in special_whitelist for x in line):
            print('special characters are not allowed')
            continue
        return line.strip('\r\n')

if __name__ == '__main__':
    
    # get name of new plugin
    if not len(sys.argv) == 2:
        print('Usage: ' + sys.argv[0] + ' <new_plugin_name>')
        print('Note: Paths are allowed. They are relative to this directory.')
        sys.exit(0)
    plugin_name = os.path.basename(sys.argv[1])
    plugin_path = sys.argv[1]

    tp = TemplateProcessor()
    tp.set_destination_path(plugin_path)
    tp.set_source_path('plugin_template')

    # prepare template files
    header_dir = os.path.join('include', 'plugin_' + plugin_name)
    source_dir = os.path.join('src')
    ensure_path(os.path.join(plugin_path, header_dir))
    ensure_path(os.path.join(plugin_path, source_dir))
    (tp.add_template('config.h.in.in', os.path.join(header_dir, 'config.h.in'))
       .add_template('events.c.in',    os.path.join(source_dir, 'events.c'))
       .add_template('events.h.in',    os.path.join(header_dir, 'events.h'))
       .add_template('plugin.c.in',    os.path.join(source_dir, 'plugin_' + plugin_name + '.c'))
       .add_template('services.c.in',  os.path.join(source_dir, 'services.c'))
       .add_template('services.h.in',  os.path.join(header_dir, 'services.h'))
       .add_template('glob.c.in',      os.path.join(source_dir, 'glob.c'))
       .add_template('glob.h.in',      os.path.join(header_dir, 'glob.h'))
       .add_template('CMakeLists.txt.in', 'CMakeLists.txt')
    )
    
    # prepare substitutions
    (tp.add_substitution('NAME', plugin_name)
       .add_substitution('NAME_CAPS', plugin_name.upper())
       .add_substitution('VERSION_MAJOR', get_user_info(
           'version major', spaces=False, special=False, default_info='0'))
       .add_substitution('VERSION_MINOR', get_user_info(
           'version minor', spaces=False, special=False, default_info='0'))
       .add_substitution('VERSION_PATCH', get_user_info(
           'version patch', spaces=False, special=False, default_info='1'))
       .add_substitution('AUTHOR', get_user_info(
           'Author of this plugin', default_info='unknown'))
       .add_substitution('CATEGORY', get_user_info(
           'Category of this plugin: Used for service/event discovery', default_info='unknown'))
       .add_substitution('DESCRIPTION', get_user_info(
           'Short description of what this plugin does', default_info='unknown'))
       .add_substitution('WEBSITE', get_user_info(
           'Website this plugin should refer to', default_info='unknown'))
    )
    
    # process
    tp.process_all()
    
    sys.exit(0)
