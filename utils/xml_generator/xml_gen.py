
from itertools import product
from random import randint
from sys import exit, argv
from math import floor

MAX_DEPTH=30
MIN_DEPTH=3
UNKNOWN="X"

root_elem = 'root'
first_level = ['orange', 'apple', 'banana', 'strawberry']
leaf_level = ['Munich', 'Zurich', 'Bern', 'Berlin', 'Rome',
    'Stockholm', 'Oslo', UNKNOWN]
words = ['lorem', 'ipsum', 'foo', 'bar']


def rand_item(l):
    return l[randint(0, len(l)-1)]


def random_sublist(s, min_sz, max_sz):
    result_size = min_sz + randint(0, max_sz - min_sz - 1)
    res = list()
    for i in xrange(result_size):
        res.append(rand_item(s))
    return res


def random_sublist_generator(s, min_sz, max_sz):
    def _random_sublist():
        return random_sublist(s, min_sz, max_sz)
    return _random_sublist


meta_structure = map(lambda l: (l, random_sublist_generator(leaf_level, 1, 5)),
        [3,4,10,15,10])


class XMLFile(object):
    def __init__(self, f_xml):
        self.size = 0;
        self.stack = list()
        self.query_set = dict()
        self.f = f_xml


    def set_first_level(self, new_first):
        if len(self.stack) == 0:
            self.push(root_elem)
        self.establish_level(1)
        self.push(new_first)


    def establish_level(self, level):
        if level == len(self.stack):
            return

        if level > len(self.stack):
            for i in xrange(level - len(self.stack)):
                self.push(UNKNOWN)
        elif level < len(self.stack):
            for i in xrange(len(self.stack) - level):
                self.pop()

        assert len(self.stack) == level


    def push(self, tag):
        off = self.emit('<{0}>\n', tag)
        self.stack.append(tag)
        return off


    def pop(self):
        return self.emit('</{0}>\n', self.stack.pop())
        

    def append_meta_structure(self, meta_struct):
        subtrees = map(lambda x: (x[0], x[1]()), meta_struct)
        for subtree in subtrees:
            tag = subtree[1]
            # 2 is the min level for arbitrary subtrees
            self.establish_level(2 + subtree[0]) 
            for tag in subtree[1]:
                self.push_match(tag)
                self.emit("{0}\n", ' '.join(random_sublist(words, 10, 20)))
                self.pop_match()


    def emit(self, fmt, *args):
        s = '    '*len(self.stack)
        tag_offset = self.size + len(s)
        s += fmt.format(*args)
        self.f.write(s)
        self.size += len(s)
        return tag_offset


    def get_query(self, leaf_tag):
        return '/{0}/{1}//{2}'.format(root_elem, self.stack[1], leaf_tag)


    def add_query_match(self, tag, offset, typ):
        query = self.get_query(tag)
        if query in self.query_set:
            self.query_set[query].append( (offset, typ) )
        else:
            self.query_set[query] = [ (offset, typ) ]


    def push_match(self, tag):
        off = self.push(tag)
        if tag != UNKNOWN:
            self.add_query_match(tag, off, 'o')


    def pop_match(self):
        tag = self.stack[-1]
        off = self.pop()
        if tag != UNKNOWN:
            self.add_query_match(tag, off, 'c')


    def close(self):
        self.establish_level(1)
        self.pop() # pop root 


def main():
    try:
        fname_xml = argv[1]
        fname_xpath = fname_xml[:-3] + 'xpath'
        fname_matches = fname_xml[:-3] + 'matches'
        f_xml, f_xpath, f_matches, target_size = (
                open(argv[1], 'w'),
                open(fname_xpath, 'w'),
                open(fname_matches, 'w'),
                float(argv[2]))
    except Exception as e:
        print("Exception: ", e) 
        print("usage: python script.py <xml-file>"+
            "<target-size-in-MB>")
        exit(1)

    xml_file = XMLFile(f_xml)

    target_size = floor(target_size * 2**20)

    while xml_file.size < target_size:
        # choose a root
        xml_file.set_first_level(rand_item(first_level))
        # generate random subtrees
        no_subtrees = randint(1, 16)
        for i in xrange(no_subtrees):
            xml_file.append_meta_structure(meta_structure)

    xml_file.close()

    # output results
    query_set = xml_file.query_set.items()
    for (query, matches), q_idx in zip(query_set, xrange(len(query_set))):
        f_xpath.write(query + '\n')
        matches = sorted(matches)
        for match in matches:
            f_matches.write('{0}, {1}, {2}\n'.format(q_idx, match[0], match[1]))

if __name__=="__main__":
    main()
