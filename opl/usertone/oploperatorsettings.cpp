#include "oploperatorsettings.h"

OPLOperatorSettings::OPLOperatorSettings()
    : _am(false)
    , _vib(false)
    , _ksr(false)
    , _multi(1)
    , _ksl(0)
    , _d(false)
{

}

OPLEnvelopeSettings& OPLOperatorSettings::envelopeSettings()
{
    return _envelopeSettings;
}

const OPLEnvelopeSettings& OPLOperatorSettings::envelopeSettings() const
{
    return _envelopeSettings;
}

bool OPLOperatorSettings::am() const
{
    return _am;
}

bool OPLOperatorSettings::vib() const
{
    return _vib;
}

bool OPLOperatorSettings::ksr() const
{
    return _ksr;
}

int OPLOperatorSettings::multi() const
{
    return _multi;
}

int OPLOperatorSettings::ksl() const
{
    return _ksl;
}

bool OPLOperatorSettings::d() const
{
    return _d;
}

void OPLOperatorSettings::setEnvelopeSettings(const OPLEnvelopeSettings& settings)
{
    _envelopeSettings = settings;
}

void OPLOperatorSettings::setAm(const bool am)
{
    _am = am;
}

void OPLOperatorSettings::setVib(const bool vib)
{
    _vib = vib;
}

void OPLOperatorSettings::setKsr(const bool ksr)
{
    _ksr = ksr;
}

void OPLOperatorSettings::setMulti(const int multi)
{
    _multi = multi;
}

void OPLOperatorSettings::setKsl(const int ksl)
{
    _ksl = ksl;
}

void OPLOperatorSettings::setD(const bool d)
{
    _d = d;
}

bool OPLOperatorSettings::operator==(const OPLOperatorSettings& other) const
{
    return _envelopeSettings == other._envelopeSettings &&
           _am == other._am &&
           _vib == other._vib &&
           _ksr == other._ksr &&
           _multi == other._multi &&
           _ksl == other._ksl &&
           _d == other._d;
}

bson_t OPLOperatorSettings::toBSON() const
{
    bson_t bson;

    bson_init(&bson);

    bson_t envelope = _envelopeSettings.toBSON();
    BSON_APPEND_DOCUMENT(&bson, "envelope", &envelope);
    BSON_APPEND_BOOL(&bson, "am", _am);
    BSON_APPEND_BOOL(&bson, "vib", _vib);
    BSON_APPEND_BOOL(&bson, "ksr", _ksr);
    BSON_APPEND_INT32(&bson, "multi", _multi);
    BSON_APPEND_INT32(&bson, "ksl", _ksl);
    BSON_APPEND_BOOL(&bson, "d", _d);

    return bson;
}

void OPLOperatorSettings::fromBSON(bson_iter_t& bson)
{
    bson_iter_t envelope;
    bson_iter_t envelopeInner;
    bson_iter_t am;
    bson_iter_t vib;
    bson_iter_t ksr;
    bson_iter_t multi;
    bson_iter_t ksl;
    bson_iter_t d;

    if (bson_iter_find_descendant(&bson, "envelope", &envelope) && BSON_ITER_HOLDS_DOCUMENT(&envelope) && bson_iter_recurse(&envelope, &envelopeInner)) {
        _envelopeSettings.fromBSON(envelopeInner);
    }
    if (bson_iter_find_descendant(&bson, "am", &am) && BSON_ITER_HOLDS_BOOL(&am)) {
        _am = bson_iter_bool(&am);
    }
    if (bson_iter_find_descendant(&bson, "vib", &vib) && BSON_ITER_HOLDS_BOOL(&vib)) {
        _vib = bson_iter_bool(&vib);
    }
    if (bson_iter_find_descendant(&bson, "ksr", &ksr) && BSON_ITER_HOLDS_BOOL(&ksr)) {
        _ksr = bson_iter_bool(&ksr);
    }
    if (bson_iter_find_descendant(&bson, "multi", &multi) && BSON_ITER_HOLDS_INT(&multi)) {
        _multi = bson_iter_int32(&multi);
    }
    if (bson_iter_find_descendant(&bson, "ksl", &ksl) && BSON_ITER_HOLDS_INT(&ksl)) {
        _ksl = bson_iter_int32(&ksl);
    }
    if (bson_iter_find_descendant(&bson, "d", &d) && BSON_ITER_HOLDS_BOOL(&d)) {
        _d = bson_iter_bool(&d);
    }
}
