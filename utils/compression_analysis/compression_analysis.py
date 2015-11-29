import libxml2
import sys
import re
import zlib
import math
import time


TOKENSIZE = 1 #bytes
inputFilename = 'test.xml'
manualTokenList = 'tokens.txt'
rawxml = open(inputFilename).read()


#convert the integer to a TOKENSIZE bytearray
def convBytes(intval):
    result = []
    for i in range(0, TOKENSIZE):
        result.append(chr(intval >> (TOKENSIZE-1-i)*8  & 255))

    return result



def compressionTest(uncompressedData, taskInfo):
    uncompressedSize = len(uncompressedData)
    print 'Summary for "'+taskInfo+'"'
    print 'uncompressed size: '+str(uncompressedSize)

    for i in range(1,10):
        bytesData = bytes(uncompressedData)

        start = time.time()
        compressed = zlib.compress(bytesData,i)
        end = time.time()
        compressedSize = len(compressed)

        print 'level',i, 'ratio: '+'{0:.4f}'.format(compressedSize/(uncompressedSize+0.0)),
        print 'execution time:','{0:.2f}ms'.format((end-start)*1000.0),
        print 'compressed size:',compressedSize

    print ''



def compress(xmlTags, tokens, fnClosingToken, taskInfo):
    vfile = []
    uncompressedSize = 0;

    for t in xmlTags:
        if t == '':
            continue

        if t[0] == '/':
            if t[1:] in tokens:
                val = fnClosingToken(tokens[t[1:]])
            else:
                val = fnClosingToken(tokens['<unknown>'])
        else:
            if t[1:] in tokens:
                val = tokens[t]
            else:
                val = tokens['<unknown>']
        
        for char in convBytes(val):
            vfile.append(char)

    compressionTest(vfile, taskInfo)


allTokens = re.findall('<([a-zA-Z]*)[ />]',rawxml)
tokens = {}
c = 1
for t in allTokens:
    if not t in tokens:
        tokens[t] = c
        c += 1
tokens['<unknown>'] = c


if c >= (1<<(TOKENSIZE*8-1))-1 :
    print 'WARNING: not all tokens fit into the range of TOKENSIZE'


print 'Total number of opening tokens:',c,'  bitwidth needed:',
print '{0:.2f}'.format(math.log(c,2)), '(only opening tokens)'




xmlTags = re.findall('<([a-zA-Z/]*)[ />]',rawxml)

compress(xmlTags, tokens, lambda x: ~x, 'negated closing tokens')
compress(xmlTags, tokens, lambda x: -1, '-1 closing tokens')




"""
tokens = open(manualTokenList).read().split('\n')
reducedTokens = {}
c = 1
for t in tokens:
    if t == '':
        continue

    if not t in reducedTokens:
        reducedTokens[t] = c
        c += 1
reducedTokens['<unknown>'] = c

print 'Total number of opening tokens (small list):',c,'  bitwidth needed:',
print '{0:.2f}'.format(math.log(c,2)), '(only opening tokens)'
compress(xmlTags, reducedTokens, lambda x: ~x, 'reduced tokenset, negated closing tokens')
"""




