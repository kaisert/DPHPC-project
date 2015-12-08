#ifndef DPHPC15_CONFIG_LOCAL_H
#define DPHPC15_CONFIG_LOCAL_H

#include "loader/MMapLoader.h"
#include "tokenizer/SuperChunkTokenizer.h"
#include "matcher/DFAMatcher.h"

#define MB ((unsigned long long) (1 << 20))
#define DEFAULT_CHUNK_SIZE 8*MB
#define STREAM_RESERVE_MEMORY (MB)

namespace config {
    using TokenContainerType = VectorWrapper<token_type_t>;
    using OffsetContainerType = vector<char *>;
    using TokenizerType = SuperChunkTokenizer;
    using MatchContainerType = vector<Match>;
    using Loader = MMapLoader;
    using Matcher = DFAMatcher;
}

#endif //DPHPC15_CONFIG_LOCAL_H
