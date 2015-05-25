import os

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
        self.ensure_path(self.__destination_path)
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
        for key, value in self.__substitutions.items():
            line = line.replace(key, value)
        return line

    @staticmethod
    def ensure_path(path):
        if not os.path.exists(path):
            os.makedirs(path)
