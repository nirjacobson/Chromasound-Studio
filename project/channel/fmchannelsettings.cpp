#include "fmchannelsettings.h"

FMChannelSettings::FMChannelSettings()
{

}

FMChannelSettings::FourOperatorSettings& FMChannelSettings::operators()
{
    return _operators;
}

const FMChannelSettings::FourOperatorSettings& FMChannelSettings::operators() const
{
    return _operators;
}

AlgorithmSettings& FMChannelSettings::algorithm()
{
    return _algorithm;
}

const AlgorithmSettings& FMChannelSettings::algorithm() const
{
    return _algorithm;
}

bson_t FMChannelSettings::toBSON() const
{
    bson_t bson = ChannelSettings::toBSON();

    bson_t operators;

    BSON_APPEND_ARRAY_BEGIN(&bson, "operators", &operators);
    for (int i = 0; i < 4; i++) {
        bson_t operatorSettings = _operators[i].toBSON();

        char keybuff[8];
        const char* key;
        bson_uint32_to_string(i, &key, keybuff, sizeof keybuff);
        BSON_APPEND_DOCUMENT(&operators, key, &operatorSettings);
    }
    bson_append_array_end(&bson, &operators);

    bson_t algorithm = _algorithm.toBSON();

    BSON_APPEND_DOCUMENT(&bson, "algorithm", &algorithm);

    return bson;
}

void FMChannelSettings::fromBSON(bson_iter_t& bson)
{
    ChannelSettings::fromBSON(bson);

    bson_iter_t operators;
    bson_iter_t child;
    bson_iter_t op;

    bson_iter_t algorithm;

    int i = 0;
    if (bson_iter_find_descendant(&bson, "operators", &operators) && BSON_ITER_HOLDS_ARRAY(&operators) && bson_iter_recurse(&operators, &child)) {
        while (bson_iter_next(&child) && i < 4) {
            bson_iter_recurse(&child, &op);
            _operators[i++].fromBSON(op);
        }
    }

    if (bson_iter_find_descendant(&bson, "algorithm", &algorithm) && BSON_ITER_HOLDS_DOCUMENT(&algorithm) && bson_iter_recurse(&algorithm, &algorithm)) {
        _algorithm.fromBSON(algorithm);
    }
}

bool FMChannelSettings::operator==(const FMChannelSettings& other) const
{
    return _algorithm == other._algorithm &&
           std::equal(std::begin(_operators), std::end(_operators), std::begin(other._operators));
}
