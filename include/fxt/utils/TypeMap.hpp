/*
    8 8888888888 `8.`8888.      ,8' 8888888 8888888888
    8 8888        `8.`8888.    ,8'        8 8888
    8 8888         `8.`8888.  ,8'         8 8888
    8 8888          `8.`8888.,8'          8 8888
    8 888888888888   `8.`88888'           8 8888
    8 8888           .88.`8888.           8 8888
    8 8888          .8'`8.`8888.          8 8888
    8 8888         .8'  `8.`8888.         8 8888
    8 8888        .8'    `8.`8888.        8 8888
    8 8888       .8'      `8.`8888.       8 8888

         FXT - Functional Extensions for C++23

    ==================================================

    MIT License

    Copyright (c) 2025 Kenneth Troldal Balslev

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

*/

#pragma once

#include <any>
#include <cstddef>
#include <typeindex>
#include <unordered_map>

/**
 * @file utils/TypeMap.hpp
 * @brief `fxt::type_map` — a type-keyed heterogeneous map with value
 *        semantics: at most one entry per key TYPE.
 *
 * The key participates only as a compile-time identity — it is never
 * stored, constructed, or otherwise required to be complete beyond what
 * `typeid` needs. The stored value is, by default, an instance of the key
 * type itself (the "bag of strongly typed values" pattern), but any
 * associated value type may be stored under a key instead — which is what
 * lets domain wrappers keep per-entry metadata (a name, a formatter, a
 * callable) alongside the payload in a single map.
 *
 * Retrieval is by pointer, honest about both absence and type mismatch:
 * `get<Key, Value>()` returns nullptr when the key is absent OR when the
 * entry stored under the key is not a `Value`. `for_each<Value>` visits
 * exactly the entries that hold a `Value` (a wrapper storing one uniform
 * entry type gets full enumeration; foreign entries are skipped).
 *
 * Copies are independent (`std::any` copies its contents), so a
 * `type_map` composes into value types without surprises.
 *
 * @note Identity is `std::type_index`, i.e. RTTI. Within one binary this
 *       is exact; across shared-library boundaries `typeid` identity is
 *       toolchain-dependent — the usual RTTI caveat applies to any keys
 *       exchanged across such a boundary.
 */

namespace fxt
{
    /**
     * @brief A type-keyed heterogeneous map: at most one entry per key
     *        type. See the file comment for the semantics.
     */
    class type_map
    {
        std::unordered_map<std::type_index, std::any> m_entries;

    public:
        /**
         * @brief Stores @p value under the type key @p Key, replacing any
         *        previous entry of that key.
         *
         * The common, homogeneous case stores the key type itself:
         * `map.set<Config>(Config{...})`. The heterogeneous case stores an
         * associated value under an explicit key:
         * `map.set<Config>(Entry{...})`. The value must be retrieved as
         * the same (decayed) type it was stored as.
         */
        template<typename Key, typename Value = Key>
        type_map& set(Value value)
        {
            m_entries.insert_or_assign(std::type_index(typeid(Key)), std::any(std::move(value)));
            return *this;
        }

        /**
         * @brief The entry stored under @p Key, or nullptr — either because
         *        the key is absent or because its entry is not a @p Value.
         */
        template<typename Key, typename Value = Key>
        [[nodiscard]] const Value* get() const
        {
            const auto it = m_entries.find(std::type_index(typeid(Key)));
            return it != m_entries.end() ? std::any_cast<Value>(&it->second) : nullptr;
        }

        /// Whether an entry (of any stored type) exists under @p Key.
        template<typename Key>
        [[nodiscard]] bool contains() const
        {
            return m_entries.contains(std::type_index(typeid(Key)));
        }

        /// Removes the entry under @p Key; returns whether one existed.
        template<typename Key>
        bool erase()
        {
            return m_entries.erase(std::type_index(typeid(Key))) > 0;
        }

        /**
         * @brief Visits every entry whose stored value is a @p Value, in
         *        unspecified order: `f(const Value&)`. Entries of other
         *        stored types are skipped.
         */
        template<typename Value, typename F>
        void for_each(F&& f) const
        {
            for (const auto& [key, value] : m_entries)
                if (const auto* v = std::any_cast<Value>(&value)) f(*v);
        }

        void clear() noexcept { m_entries.clear(); }

        [[nodiscard]] bool        empty() const noexcept { return m_entries.empty(); }
        [[nodiscard]] std::size_t size() const noexcept { return m_entries.size(); }
    };

}    // namespace fxt
