#ifndef DPHPC15_CONFIG_LOCAL_H
#define DPHPC15_CONFIG_LOCAL_H

#include "loader/MMapLoader.h"
#include "tokenizer/SuperChunkTokenizer.h"
#include "matcher/DFAMatcher.h"
#include "compression/MyVector.h"
#include "compression/SupressClosingVSizeDeflator.h"

#define MB ((unsigned long long) (1 << 20))
#define DEFAULT_CHUNK_SIZE 8*MB
#define STREAM_RESERVE_MEMORY (MB)


namespace config {
    typedef uint64_t Bitmask_t;
    typedef uint8_t Cmpr_Token_t;

#define CONFIG_DESCRIPTION "";

//    using TokenContainerType = MyVector<token_type_t>;
    using TokenContainerType = SupressClosingVSizeDeflator<Bitmask_t, Cmpr_Token_t>;

    using OffsetContainerType = vector<char *>;
    using TokenizerType = SuperChunkTokenizer;
    using MatchContainerType = vector<Match>;
    using Loader = MMapLoader;
    using Matcher = DFAMatcher;
}

#endif //DPHPC15_CONFIG_LOCAL_H
