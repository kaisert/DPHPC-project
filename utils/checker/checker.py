import libxml2
import sys

#
# This script computes the start and end positions for the matches in "test.xml" 
# selected by the XPath queries from "queries.txt". It writes the results into
# the files (specified below) for easier unit testing.
#
# Estimated execution time (including) correctness check:
# number_of_matches * file_size_in_MiB <= 300'000 MiB
# results in approximately ~2.5 minutes, tested with a 10 MiB XML and 4 queries
#
#
# Important:
# If the same result string is not matched in every occurence by a query, this
# code might determine a wrong position value.
# To do this computationally intensive check, use:
# $ python checker.py correct
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

if len(sys.argv) > 1 and (sys.argv[1] == 'correct' or sys.argv[1] == 'Correct'):
    verifyMatchCounts = True
else:
    verifyMatchCounts = False


for i in range(0, len(queries)):
    if queries[i] == '':
        continue

    print 'Processing query '+str(i)+' ...'

    matchPos = {}
    matchDuplicates = {}
    matchCount = {}
    lastPos = 0

    ctxt = doc.xpathNewContext()
    res = ctxt.xpathEval(queries[i])
    for match in res:
        content = match.serialize()
        hashkey = str(content)

        if hashkey in matchPos:
            pos = matchPos[hashkey]
        else:
            pos = -1

        #first/next zero based position
        pos = rawxml.find(content, max(lastPos,pos+1))
        endPos = pos+len(content)-1;
        # WARNING:
        # As long as libxml2 reports the matches (by start position) in file
        # order this lastPos optimization works well.
        if pos == -1:
            print 'Bug: XPath result below was not found in rawxml at',
            print str(max(lastPos,pos+1))+':'
            print content
            sys.exit(1);

        matchPos[hashkey] = pos
        if not hashkey in matchCount:
            matchCount[hashkey] = 1
        else:
            matchCount[hashkey] = matchCount[hashkey]+1


        #print '\nQuery '+str(i)+' from', pos, 'to', str(endPos)+':\n', match
        

        #fill first CSV file
        foutMatches.write(str(i) + sep + str(pos) + sep + str(endPos)+'\n' )

        records.append(Record(i,pos,0))
        records.append(Record(i,endPos,1))



    if verifyMatchCounts:
        #Verify that all identical XPath result strings are captured by this query
        #and hence the computed positions are correct.
        print 'Verifying '+str(len(matchCount))+' matches in '+str(len(rawxml))+' bytes'
        for k,count in matchCount.iteritems():
            xmlCount = rawxml.count(k)
            if xmlCount != count:
                print '\nAmbiguous position: Current XPath query did not',
                print 'capture all identical result strings for:\n'+k
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
lastEntry = "";
for i in range(1, len(records)):
    entry = str(records[i].position) + sep + str(records[i].isEnd) + '\n'

    if entry != lastEntry: #remove indistinguishible duplicates
        foutNoQuery.write(entry)
        lastEntry = entry



if not verifyMatchCounts:
    print '\nAll occurences of each XPath result string in the raw xml must be',
    print 'matches of the same query.'
    print 'To do this computationally intensive check, use:'
    print '$ python checker.py correct'
