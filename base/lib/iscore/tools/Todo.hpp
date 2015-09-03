#pragma once
#include "iscore_compiler_detection.hpp"
#include "iscore_git_info.hpp"
#include <exception>
#include <typeinfo>
#include <QDebug>
#include <QObject>

#ifdef _WIN32
#include <Windows.h>
#define DEBUG_BREAK DebugBreak()
#else
#include <csignal>
#define DEBUG_BREAK std::raise(SIGTRAP)
#endif

#define ISCORE_TODO do { qDebug() << "TODO"; } while (0)
#if defined(ISCORE_DEBUG)
#define ISCORE_BREAKPOINT do { DEBUG_BREAK; } while (0)
#else
#define ISCORE_BREAKPOINT do {} while (0)
#endif

#ifdef ISCORE_DEBUG
#define ISCORE_ASSERT(arg) do { \
        bool b = (arg); \
        if(!b) { DEBUG_BREAK; Q_ASSERT( b ); } \
    } while (0)
#else
#define ISCORE_ASSERT(arg) ((void)(0))
#endif

#define ISCORE_ABORT do { DEBUG_BREAK; std::terminate(); } while(0)


#if ISCORE_COMPILER_CXX_RELAXED_CONSTEXPR
#define ISCORE_RELAXED_CONSTEXPR constexpr
#else
#define ISCORE_RELAXED_CONSTEXPR
#endif

template<typename T>
using remove_qualifs_t = std::decay_t<std::remove_pointer_t<std::decay_t<T>>>;

#ifdef ISCORE_DEBUG
template<typename T,
         typename U>
T safe_cast(U* other)
{
    auto res = dynamic_cast<T>(other);
    ISCORE_ASSERT(res);
    return res;
}

template<typename T,
         typename U>
T safe_cast(U&& other)
try
{
        auto&& res = dynamic_cast<T>(other);
        return res;
}
catch(std::bad_cast& e)
{
    qDebug() << e.what();
    ISCORE_ABORT;
}

#else
#define safe_cast static_cast
#endif

#define ISCORE_METADATA(str) public: static constexpr const char className[]{ str }; private:

/**
 * @brief con A wrapper around Qt's connect
 *
 * Allows the first argument to be a reference
 */
template<typename T, typename... Args>
auto con(const T& t, Args&&... args)
{
    return QObject::connect(&t, std::forward<Args&&>(args)...);
}
