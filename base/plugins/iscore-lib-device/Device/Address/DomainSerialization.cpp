#include <State/ValueConversion.hpp>
#include <State/ValueSerialization.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <qdatastream.h>
#include <qglobal.h>
#include <qjsonarray.h>
#include <qjsonvalue.h>

#include "DomainSerialization.hpp"
#include "State/Value.hpp"

template <typename T> class Reader;
template <typename T> class Writer;

template<>
void Visitor<Reader<DataStream>>::readFrom(const iscore::Domain& n)
{
    m_stream << n.min << n.max << n.values;
}

template<>
void Visitor<Writer<DataStream>>::writeTo(iscore::Domain& n)
{
    m_stream >> n.min >> n.max >> n.values;
}

QJsonObject DomainToJson(const iscore::Domain& d)
{
    QJsonObject obj;
    obj["Min"] = ValueToJson(d.min);
    obj["Max"] = ValueToJson(d.max);

    QJsonArray arr;
    for(auto& val : d.values)
        arr.append(ValueToJson(val));
    obj["Values"] = arr;

    return obj;
}

iscore::Domain JsonToDomain(const QJsonObject& obj, const QString& t)
{
    iscore::Domain d;
    if(obj.contains("Min"))
    {
        d.min = iscore::convert::toValue(obj["Min"], t);
    }
    if(obj.contains("Max"))
    {
        d.max = iscore::convert::toValue(obj["Max"], t);
    }

    for(const QJsonValue& val : obj["Values"].toArray())
        d.values.append(iscore::convert::toValue(val, t));

    return d;
}
