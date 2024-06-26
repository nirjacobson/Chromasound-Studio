#include "oplsettings.h"

OPLSettings::OPLSettings()
    : _tl(0)
    , _fb(0)
{

}

OPLSettings::TwoOperatorSettings& OPLSettings::operators()
{
    return _operators;
}

const OPLSettings::TwoOperatorSettings& OPLSettings::operators() const
{
    return _operators;
}

int OPLSettings::tl() const
{
    return _tl;
}

void OPLSettings::setTl(const int tl)
{
    _tl = tl;
    _operators[0].envelopeSettings().setTl(tl);
    _operators[1].envelopeSettings().setTl(tl);
}

int OPLSettings::fb() const
{
    return _fb;
}

void OPLSettings::setFb(const int fb)
{
    _fb = fb;
}

bool OPLSettings::operator==(const OPLSettings& other) const
{
    return _operators[0] == other._operators[0] &&
           _operators[1] == other._operators[1] &&
           _tl == other._tl &&
           _fb == other._fb;
}

bson_t OPLSettings::toBSON() const
{
    bson_t bson;

    bson_init(&bson);

    bson_t operators;

    BSON_APPEND_ARRAY_BEGIN(&bson, "operators", &operators);
    for (int i = 0; i < 2; i++) {
        bson_t operatorSettings = _operators[i].toBSON();

        char keybuff[8];
        const char* key;
        bson_uint32_to_string(i, &key, keybuff, sizeof keybuff);
        BSON_APPEND_DOCUMENT(&operators, key, &operatorSettings);
    }
    bson_append_array_end(&bson, &operators);

    BSON_APPEND_INT32(&bson, "tl", _tl);
    BSON_APPEND_INT32(&bson, "fb", _fb);

    return bson;
}

void OPLSettings::fromBSON(bson_iter_t& bson)
{
    bson_iter_t operators;
    bson_iter_t child;
    bson_iter_t op;

    bson_iter_t tl;
    bson_iter_t fb;

    int i = 0;
    if (bson_iter_find_descendant(&bson, "operators", &operators) && BSON_ITER_HOLDS_ARRAY(&operators) && bson_iter_recurse(&operators, &child)) {
        while (bson_iter_next(&child) && i < 2) {
            bson_iter_recurse(&child, &op);
            _operators[i++].fromBSON(op);
        }
    }

    if (bson_iter_find_descendant(&bson, "tl", &tl) && BSON_ITER_HOLDS_INT(&tl)) {
        _tl = bson_iter_int32(&tl);
    }

    if (bson_iter_find_descendant(&bson, "fb", &fb) && BSON_ITER_HOLDS_INT(&fb)) {
        _fb = bson_iter_int32(&fb);
    }
}
