import libxml2
import sys

#
# This script computes the start and end positions for the matches in "test.xml" 
# selected by the XPath queries from "queries.txt". It writes the results into
# the files (specified below) for easier unit testing.
#

#separator <sep>
sep = '\t'

#file format: <query-nr><sep><start><sep><end>
foutMatches = open('matches.csv', 'w')

#file format: <query-nr><sep><start/end position><sep><0 for start, 1 for end event>
foutQuery = open('events_with_query.csv', 'w')

#file format: <start/end position><sep><0 for start, 1 for end event>
foutNoQuery = open('events_without_query.csv', 'w')


class Record:
    queryNr = -1
    position = -1
    isEnd = -1

    def __init__(self, queryNr, position, isEnd):
        self.queryNr = queryNr
        self.position = position
        self.isEnd = isEnd


inputFilename = 'test.xml'
queries = open('queries.txt').read().split('\n')
rawxml = open(inputFilename).read()
doc = libxml2.parseFile(inputFilename)

records = []

for i in range(0, len(queries)):
    if queries[i] == '':
        continue

    ctxt = doc.xpathNewContext()
    res = ctxt.xpathEval(queries[i])
    for match in res:
        content = match.serialize()
        pos = rawxml.find(content) #zero based
        endPos = pos+len(content)-1;
        
        #print '\nQuery '+str(i)+' from', pos, 'to', str(endPos)+':\n', match
        

        #fill first CSV file
        foutMatches.write(str(i) + sep + str(pos) + sep + str(endPos)+'\n' )

        records.append(Record(i,pos,0))
        records.append(Record(i,endPos,1))

        if pos == -1:
            print 'Bug: XPath result below was not found in rawxml:'
            print content
            sys.exit(1);

        if rawxml.find(content, pos+1) > -1:
            print 'Computed position is ambiguous: the same XPath result',
            print 'string below must not occur multiple times in the xml file.'
            print content
            sys.exit(1)


    ctxt.xpathFreeContext()


doc.freeDoc()


#sort and write the other two CSV files
records = sorted(records, key=lambda r: (r.queryNr, r.position, r.isEnd))
for i in range(0, len(records)):
    foutQuery.write(str(records[i].queryNr)+sep)
    foutQuery.write(str(records[i].position)+sep)
    foutQuery.write(str(records[i].isEnd)+'\n')


records = sorted(records, key=lambda r: (r.position, r.isEnd, r.queryNr))
for i in range(0, len(records)):
    foutNoQuery.write(str(records[i].position)+sep)
    foutNoQuery.write(str(records[i].isEnd)+'\n')

