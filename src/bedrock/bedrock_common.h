// Copyright (c) 2023, The Endstone Project. (https://endstone.dev) All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "endstone/common.h"

namespace Core {
class FilePathManager;
}

#if defined(_WIN32)
#ifdef BEDROCK_EXPORTS
#define BEDROCK_API __declspec(dllexport)
#elif defined(BEDROCK_IMPORTS)
#define BEDROCK_API __declspec(dllimport)
#endif
#else
#if defined(BEDROCK_EXPORTS) || defined(BEDROCK_IMPORTS)
#if defined(__GNUC__) || defined(__clang__)
#define BEDROCK_API __attribute__((visibility("default")))
#endif
#endif
#endif

#ifndef BEDROCK_API
#define BEDROCK_API
#endif

namespace {
/**
 * @brief Construct a std::function from a class virtual method (non-const, no ref-qualifier)
 */
template <typename Return, typename Class, typename... Arg>
inline std::function<Return(Arg...)> get_virtual_function(Return (Class::*)(Arg...), Class *obj, int index)
{
    auto vf_table = *reinterpret_cast<char **>(obj);
    auto func = *reinterpret_cast<Return (**)(Class *, Arg...)>(vf_table + index * sizeof(size_t));
    return [func, obj](Arg... args) -> Return {
        return func(obj, std::forward<Arg>(args)...);
    };
}

/**
 * @brief Construct a std::function from a class virtual method (const, no ref-qualifier)
 */
template <typename Return, typename Class, typename... Arg>
inline std::function<Return(Arg...)> get_virtual_function(Return (Class::*)(Arg...) const, const Class *obj, int index)
{
    auto vf_table = *reinterpret_cast<char **>(const_cast<Class *>(obj));
    auto func = *reinterpret_cast<Return (**)(const Class *, Arg...)>(vf_table + index * sizeof(size_t));
    return [func, obj](Arg... args) -> Return {
        return func(obj, std::forward<Arg>(args)...);
    };
}

/**
 * @brief Construct a std::function from a class virtual method (non-const, no ref-qualifier)
    with considerations for Return Value Optimization (RVO).
 */
template <typename Return, typename Class, typename... Arg>
inline std::function<Return *(Return *, Arg...)> get_virtual_function_rvo(Return (Class::*)(Arg...), Class *obj,
                                                                          int index)
{
    auto vf_table = *reinterpret_cast<char **>(obj);
    auto func = *reinterpret_cast<Return *(**)(Class *, Arg...)>(vf_table + index * sizeof(size_t));
    return [func, obj](Return *ret, Arg... args) -> Return * {
        return func(obj, ret, std::forward<Arg>(args)...);
    };
}

/**
 * @brief Construct a std::function from a class virtual method (const, no ref-qualifier)
 *  with considerations for Return Value Optimization (RVO).
 */
template <typename Return, typename Class, typename... Arg>
inline std::function<Return *(Return *, Arg...)> get_virtual_function_rvo(Return (Class::*)(Arg...) const,
                                                                          const Class *obj, int index)
{
    auto vf_table = *reinterpret_cast<char **>(const_cast<Class *>(obj));
    auto func = *reinterpret_cast<Return *(**)(const Class *, Return *, Arg...)>(vf_table + index * sizeof(size_t));
    return [func, obj](Return *ret, Arg... args) -> Return * {
        return func(obj, ret, std::forward<Arg>(args)...);
    };
}

#define BEDROCK_VIRTUAL_CALL(index, fp, obj, ...)     get_virtual_function(fp, obj, index)(__VA_ARGS__);
#define BEDROCK_VIRTUAL_CALL_RVO(index, fp, obj, ...) get_virtual_function_rvo(fp, obj, index)(__VA_ARGS__);
} // namespace
