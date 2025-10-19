/*
 * Copyright 2017 - 2021 Justas Masiulis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef JM_XORSTR_HPP
#define JM_XORSTR_HPP

#if defined(_MSC_VER)
#define XORSTR_INLINE __forceinline
#else
#define XORSTR_INLINE __attribute__((always_inline))
#endif

#if defined(__clang__) || defined(__GNUC__)
#define JM_XORSTR_LOAD_FROM_REG(x) ::jm::detail::load_from_reg(x)
#else
#define JM_XORSTR_LOAD_FROM_REG(x) (x)
#endif


#include <immintrin.h>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <type_traits>

#define xorstr(str) ::jm::xor_string(XORSTR_STR(str))
#define XORSTR_STR(str) \
    []() { \
        constexpr ::jm::detail::string_builder< \
            std::decay_t<decltype(*str)>, \
            sizeof(str) / sizeof(*str)> \
            builder(str); \
        return builder.data; \
    }()

#ifdef _MSC_VER
#define JM_XORSTR_ALWAYS_INLINE __forceinline
#else
#define JM_XORSTR_ALWAYS_INLINE __attribute__((always_inline))
#endif

namespace jm {

    namespace detail {

        template<std::size_t N>
        struct uint_selector;

        template<>
        struct uint_selector<1> {
            using type = std::uint8_t;
        };
        template<>
        struct uint_selector<2> {
            using type = std::uint16_t;
        };
        template<>
        struct uint_selector<4> {
            using type = std::uint32_t;
        };
        template<>
        struct uint_selector<8> {
            using type = std::uint64_t;
        };

        template<std::size_t N>
        using uint_t = typename uint_selector<N>::type;


        template<std::size_t Size>
        XORSTR_INLINE constexpr std::size_t storage_size()
        {
            return ((Size / 16) + (Size % 16 != 0)) * 2;
        }


        template<typename CharT, std::size_t Size>
        class xor_string {
            alignas(16) std::uint64_t _storage[storage_size<Size>()];

        public:
            using value_type = CharT;
            using size_type  = std::size_t;

            XORSTR_INLINE constexpr xor_string(const CharT* str) noexcept
                : _storage{}
            {
                // TODO implement this properly instead of relying on luck
                for(size_type i = 0; i < Size; ++i)
                    reinterpret_cast<CharT*>(_storage)[i] = str[i] ^ (static_cast<CharT>(i) + 0x55);
            }

            XORSTR_INLINE const value_type* get() const noexcept
            {
                return reinterpret_cast<const value_type*>(_storage);
            }

            XORSTR_INLINE value_type* get() noexcept
            {
                return reinterpret_cast<value_type*>(_storage);
            }

            XORSTR_INLINE value_type* crypt_get() noexcept
            {
                for(size_type i = 0; i < Size; ++i)
                    get()[i] ^= (static_cast<CharT>(i) + 0x55);
                return get();
            }
        };

        template<typename CharT, std::size_t Size>
        struct string_builder {
            xor_string<CharT, Size> data;
        };

        template<typename CharT, std::size_t Size>
        XORSTR_INLINE constexpr auto make_xor_string(const CharT* str)
        {
            return xor_string<CharT, Size>{ str };
        }

        template<typename T>
        XORSTR_INLINE T load_from_reg(T value)
        {
#if defined(__clang__) || defined(__GNUC__)
            asm("" : "+r"(value));
#endif
            return value;
        }

    } // namespace detail


    template<typename CharT, std::size_t Size>
    XORSTR_INLINE auto make_xor_string(const CharT(&str)[Size])
    {
        return detail::make_xor_string<CharT, Size - 1>(str);
    }

    template<typename CharT, std::size_t Size>
    using xor_string = detail::xor_string<CharT, Size>;

} // namespace jm


#endif // include guard