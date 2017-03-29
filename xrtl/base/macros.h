// Copyright 2017 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef XRTL_BASE_MACROS_H_
#define XRTL_BASE_MACROS_H_

#include <cstring>
#include <memory>

// Pulls in the XRTL_CONFIG_* defines.
#include "xrtl/tools/target_config/target_config.h"  // IWYU pragma: export
// Pulls in the XRTL_PLATFORM_* and XRTL_ARCH_* defines.
#include "xrtl/tools/target_platform/target_platform.h"  // IWYU pragma: export

// Temporary make_unique (until C++14 is everywhere).
#include "xrtl/base/make_unique.h"  // IWYU pragma: export

#if defined(__GNUC__) || defined(__APPLE__)

#define XRTL_ALWAYS_INLINE __attribute__((always_inline)) inline
#define XRTL_ATTRIBUTE_NORETURN __attribute__((noreturn))
#define XRTL_ATTRIBUTE_NOINLINE __attribute__((noinline))
#define XRTL_ATTRIBUTE_UNUSED __attribute__((unused))
#define XRTL_ATTRIBUTE_COLD __attribute__((cold))
#define XRTL_ATTRIBUTE_WEAK __attribute__((weak))

#elif defined(XRTL_COMPILER_MSVC)

#define XRTL_ALWAYS_INLINE __forceinline
#define XRTL_ATTRIBUTE_NORETURN __declspec(noreturn)
#define XRTL_ATTRIBUTE_NOINLINE
#define XRTL_ATTRIBUTE_UNUSED
#define XRTL_ATTRIBUTE_COLD
#define XRTL_ATTRIBUTE_WEAK

#else

#define XRTL_ALWAYS_INLINE inline
#define XRTL_ATTRIBUTE_NORETURN
#define XRTL_ATTRIBUTE_NOINLINE
#define XRTL_ATTRIBUTE_UNUSED
#define XRTL_ATTRIBUTE_COLD
#define XRTL_ATTRIBUTE_WEAK

#endif  // GNUC / MSVC / etc

#define XRTL_PREDICT_FALSE(x) (x)
#define XRTL_PREDICT_TRUE(x) (x)

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
// Define this to 1 if the code is compiled in C++11 mode.
#define LANG_CXX11 1
#endif

#if defined(__clang__) && defined(LANG_CXX11) && defined(__has_warning)
#if __has_feature(cxx_attributes) && __has_warning("-Wimplicit-fallthrough")
#define XRTL_FALLTHROUGH_INTENDED [[clang::fallthrough]]  // NOLINT
#endif  // -Wimplicit-fallthrough
#endif  // clang && c++11+
#ifndef XRTL_FALLTHROUGH_INTENDED
#define XRTL_FALLTHROUGH_INTENDED \
  do {                            \
  } while (0)
#endif  // !XRTL_FALLTHROUGH_INTENDED

// We're redefining this here, because pulling base/macros ends up pulling a
// ton of stuff that prevents us from bare metal compiling.
#define XRTL_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;           \
  void operator=(const TypeName&) = delete

// Macro from //base/allow_rvalue_references.h to disable the rvalue ref
// warning.
#if defined(GOOGLE_GLIBCXX_VERSION) && defined(__clang__) && !defined(__APPLE__)
#define XRTL_ALLOW_RVALUE_REFERENCES_PUSH \
  _Pragma("GCC diagnostic push")          \
      _Pragma("GCC diagnostic ignored \"-Wgoogle3-rvalue-reference\"")
#define XRTL_ALLOW_RVALUE_REFERENCES_POP _Pragma("GCC diagnostic pop")
#else  // other compiler case follows
#define XRTL_ALLOW_RVALUE_REFERENCES_PUSH
#define XRTL_ALLOW_RVALUE_REFERENCES_POP
#endif  // other compiler case

namespace xrtl {

// Type-safe countof for determining constant array length.
template <typename T, std::size_t N>
inline constexpr std::size_t count_of(T const (&)[N]) noexcept {
  return N;
}

// Casts the bits of one type to another of equal size without conversion.
// Example:
//   float f = 3.14159265358979;
//   int i = bit_cast<int32_t>(f);
//   // i = 0x40490fdb
template <class Dest, class Source>
inline Dest bit_cast(const Source& source) {
  static_assert(sizeof(Dest) == sizeof(Source), "Type sizes must match");
  Dest dest;
  std::memcpy(&dest, &source, sizeof(dest));
  return dest;
}

}  // namespace xrtl

#endif  // XRTL_BASE_MACROS_H_