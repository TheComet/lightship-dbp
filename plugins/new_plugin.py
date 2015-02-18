#!/usr/bin/env python

import sys
import os


class TemplateProcessor(object):
    
    def __init__(self):
        self.__files = list()             # list of tuples containing [file_in, file_out]
        self.__replacements = dict()      # key,value replacements to make on the files
        self.__source_path = "./"         # a path prefix to a folder containing the input template files
        self.__destination_path = "./"    # the destination path
    
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
    
    def add_replacement(self, key, value):
        final_key = "@" + key + "@"
        if final_key in self.__replacements:
            return self
        self.__replacements[final_key] = value
        return self

    def process_all(self):
        # make sure destination path exists
        if not os.path.exists(self.__destination_path):
            os.makedirs(self.__destination_path)
        # process all files
        for file_tuple in self.__files:
            print("processing files: (" + file_tuple[0] + ", " + file_tuple[1] + ")")
            self.process_template(file_tuple[0], file_tuple[1])

    def process_template(self, file_in, file_out):
        # open source template file and destination file, and append path prefixes
        with open(os.path.join(self.__destination_path, file_out), 'w') as f:
            for line in open(os.path.join(self.__source_path, file_in), 'r'):
                f.write(self.__process_line(line))
    
    def __process_line(self, line):
        for key, value in self.__replacements.iteritems():
            line = line.replace(key, value)
        return line

if __name__ == '__main__':
    
    # get name of new plugin
    if not len(sys.argv) == 2:
        print("Usage: " + sys.argv[0] + " <new_plugin_name>")
        print("Note: Paths are allowed. They are relative to this directory.")
        sys.exit(0)
    plugin_name = os.path.basename(sys.argv[1])
    plugin_path = sys.argv[1]
    
    # prepare template files
    tp = TemplateProcessor()
    tp.set_destination_path(plugin_path)
    tp.set_source_path("plugin_template")
    (tp.add_template("config.h.in.in", "config.h.in")
       .add_template("events.c.in",    "events.c")
       .add_template("events.h.in",    "events.h")
       .add_template("plugin.c.in",    "plugin.c")
       .add_template("plugin.h.in",    "plugin.h")
       .add_template("services.c.in",  "services.c")
       .add_template("services.h.in",  "services.h")
       .add_template("glob.c.in",      "glob.c")
       .add_template("glob.h.in",      "glob.h")
    (tp.add_replacement("NAME", plugin_name)
       .add_replacement("NAME_CAPS", plugin_name.upper()))
    
    # process
    tp.process_all()
    
    sys.exit(0)
