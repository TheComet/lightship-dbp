#!/usr/bin/env python

import os
import sys

class FileCollection(object):
    def __init__(self, *extensions):
        self.__files = list()
        self.__extensions = [x for x in extensions]

    def collect_files(self, *directories):
        self.__files = list()
        for directory in directories:
            for root, sub_folders, files in os.walk(directory):
                for file in files:
                    if any(file.endswith(extension) for extension in self.__extensions):
                        self.__files.append(os.path.join(root, file))
        return self.__files

    def exclude_directories(self, *directories):
        for directory in directories:
            self.__files = [x for x in self.__files if x.find(directory) == -1]
    
    def get_file_list(self):
        return self.__files


class Event(object):
    def __init__(self):
        self.declaration_file = str()
        self.definition_file = str()
        self.creation_file = str()

        self.declaration = str()
        self.definition = str()           # the name of 
        self.global_name = str()          # the global name of this event, including name space
        self.collected_fire_args = list() # a list containing lists of arguments extracted from all EVENT_FIRE calls to this event


class EventChecker(object):
    def __init__(self):
        self.__events = dict()

    def check(self, file_collection):
        self.__extract_data(file_collection)
        self.__list_events()
        return 0

    def __extract_data(self, file_collection):
        self.__events = dict()
        comments = False
        macro = False
        for file in file_collection.get_file_list():
            plugin_name = self.__determine_plugin_name_from_file_path(file)
            for line in open(file, 'r'):

                # ignore comments and macros
                if not line.find('/*') == -1:
                    comments = True
                if not line.find('*/') == -1:
                    comments = False
                if line.strip(' \t').startswith('#'):
                    macro = True
                elif line.strip(' \t\n').endswith('\\'):
                    macro = True
                else:
                    if macro:  # skip if last line was a macro
                        macro = False
                        continue
                if comments or macro:
                    continue
                
                # event declarations
                if not line.find('EVENT_H') == -1:
                    event_name = line.split('(')
                    if len(event_name) > 1:
                        event_name = event_name[1].strip(')\n')
                        if(not event_name in self.__events): self.__events[event_name] = Event()
                        self.__events[event_name].declaration_file = file
                        self.__events[event_name].declaration = event_name
                

                # event definitions
                if not line.find('EVENT_C') == -1:
                    event_name = line.split('(')
                    if len(event_name) > 1:
                        event_name = event_name[1].strip(')\n')
                        if(not event_name in self.__events): self.__events[event_name] = Event()
                        self.__events[event_name].definition_file = file
                        self.__events[event_name].definition = event_name

                # event creation
                if not line.find('event_create') == -1:
                    event_name = line.split('=')
                    if len(event_name) > 1:
                        event_name = event_name[0].strip(' \n')

                        # get global name of the event being created
                        event_global_name = plugin_name + '.' + line.split(',')[1].strip(' ");\n')
                        if(not event_name in self.__events): self.__events[event_name] = Event()
                        self.__events[event_name].creation_file = file
                        self.__events[event_name].global_name = event_global_name
                
                # fire events
                if not line.find('EVENT_FIRE') == -1:
                    event_args = [x.strip('\n\t ;') for x in line.split(',')]
                    event_args[0] = event_args[0].split('(')[1].strip(' ') # remove "EVENT_FIRE0(" and extract event name
                    event_args[len(event_args)-1] = event_args[len(event_args)-1][:-1] # remove ")" at the end of the last argument

                # event listener registration
                if not line.find('event_register_listener') == -1:
                    pass

                # event listeners
                if not line.find('EVENT_LISTENER') == -1:
                    pass

    def __list_events(self):
        for key, value in self.__events.iteritems():
            print(key + ': ' + value.declaration + "," + value.definition + "," + value.global_name)

    def __determine_plugin_name_from_file_path(self, path):
        if path.find('plugins/') == -1:
            return 'builtin'

        # basically, we're searching for the plugin's CMakeLists.txt file. The current directory name
        # will also be the plugin name.
        folders = path.split('/')
        for i in range(len(folders)-1, 0, -1):
            new_path = '/'.join(folders[:i])
            for file in os.listdir(new_path):
                if file == 'CMakeLists.txt':
                    # found!
                    return folders[i-1]
        return 'builtin'

if __name__ == '__main__':

    # generate list of files to scan
    files = FileCollection('.c', '.h')
    files.collect_files('util',
                        'plugins',
                        'lightship')
    files.exclude_directories('plugins/core/renderer_gl/ext',
                              'plugins/core/yaml/ext')

    # check
    event_checker = EventChecker()

    sys.exit(event_checker.check(files))
