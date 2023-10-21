#include "algorithmsettings.h"

AlgorithmSettings::AlgorithmSettings()
    : _algorithm(0)
    , _feedback(0)
{

}

int AlgorithmSettings::algorithm() const
{
    return _algorithm;
}

int AlgorithmSettings::feedback() const
{
    return _feedback;
}

void AlgorithmSettings::setAlgorithm(const int algorithm)
{
    _algorithm = algorithm;
}

void AlgorithmSettings::setFeedback(const int feedback)
{
    _feedback = feedback;
}

bool AlgorithmSettings::operator==(const AlgorithmSettings& other) const
{
    return _algorithm == other._algorithm && _feedback == other._feedback;
}

bson_t AlgorithmSettings::toBSON() const
{
    bson_t bson;

    bson_init(&bson);

    BSON_APPEND_INT32(&bson, "algorithm", _algorithm);
    BSON_APPEND_INT32(&bson, "feedback", _feedback);

    return bson;
}

void AlgorithmSettings::fromBSON(bson_iter_t& bson)
{
    bson_iter_t algorithm;
    bson_iter_t feedback;

    if (bson_iter_find_descendant(&bson, "algorithm", &algorithm) && BSON_ITER_HOLDS_INT32(&algorithm)) {
        _algorithm = bson_iter_int32(&algorithm);
    }

    if (bson_iter_find_descendant(&bson, "feedback", &feedback) && BSON_ITER_HOLDS_INT32(&feedback)) {
        _feedback = bson_iter_int32(&feedback);
    }
}
