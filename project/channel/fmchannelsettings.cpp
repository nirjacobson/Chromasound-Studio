#include "fmchannelsettings.h"

FMChannelSettings::FMChannelSettings()
    : _algorithm(0)
    , _feedback(0)
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

int FMChannelSettings::algorithm() const
{
    return _algorithm;
}

int FMChannelSettings::feedback() const
{
    return _feedback;
}

void FMChannelSettings::setAlgorithm(const int alg)
{
    _algorithm = alg;
}

void FMChannelSettings::setFeedback(const int fb)
{
    _feedback = fb;
}

bson_t FMChannelSettings::toBSON() const
{
    bson_t bson;

    bson_init(&bson);

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

    BSON_APPEND_INT32(&bson, "algorithm", _algorithm);
    BSON_APPEND_INT32(&bson, "feedback", _feedback);

    return bson;
}

void FMChannelSettings::fromBSON(bson_iter_t& bson)
{
    bson_iter_t operators;
    bson_iter_t child;
    bson_iter_t op;

    bson_iter_t algorithm;
    bson_iter_t feedback;

    int i = 0;
    if (bson_iter_find_descendant(&bson, "operators", &operators) && BSON_ITER_HOLDS_ARRAY(&operators) && bson_iter_recurse(&operators, &child)) {
        while (bson_iter_next(&child) && i < 4) {
            bson_iter_recurse(&child, &op);
            _operators[i++].fromBSON(op);
        }
    }
    if (bson_iter_find_descendant(&bson, "algorithm", &algorithm) && BSON_ITER_HOLDS_INT(&algorithm)) {
        _algorithm = bson_iter_int32(&algorithm);
    }
    if (bson_iter_find_descendant(&bson, "feedback", &feedback) && BSON_ITER_HOLDS_INT(&feedback)) {
        _feedback = bson_iter_int32(&feedback);
    }
}

bool FMChannelSettings::operator==(const FMChannelSettings& other) const
{
    return _algorithm == other._algorithm &&
           _feedback == other._feedback &&
           std::equal(std::begin(_operators), std::end(_operators), std::begin(other._operators));
}
