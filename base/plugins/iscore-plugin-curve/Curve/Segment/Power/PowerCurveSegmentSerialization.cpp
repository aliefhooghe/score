#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <qjsonobject.h>
#include <qjsonvalue.h>

#include "PowerCurveSegmentModel.hpp"

template <typename T> class Reader;
template <typename T> class Writer;

template<>
void Visitor<Reader<DataStream>>::readFrom(const PowerCurveSegmentModel& segmt)
{
    m_stream << segmt.gamma;
}

template<>
void Visitor<Writer<DataStream>>::writeTo(PowerCurveSegmentModel& segmt)
{
    m_stream >> segmt.gamma;
}

template<>
void Visitor<Reader<JSONObject>>::readFrom(const PowerCurveSegmentModel& segmt)
{
    m_obj["Power"] = segmt.gamma;
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(PowerCurveSegmentModel& segmt)
{
    segmt.gamma = m_obj["Power"].toDouble();
}


template<>
void Visitor<Reader<DataStream>>::readFrom(const PowerCurveSegmentData& segmt)
{
    m_stream << segmt.gamma;
}

template<>
void Visitor<Writer<DataStream>>::writeTo(PowerCurveSegmentData& segmt)
{
    m_stream >> segmt.gamma;
}

template<>
void Visitor<Reader<JSONObject>>::readFrom(const PowerCurveSegmentData& segmt)
{
    m_obj["Power"] = segmt.gamma;
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(PowerCurveSegmentData& segmt)
{
    segmt.gamma = m_obj["Power"].toDouble();
}
