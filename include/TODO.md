# FXT — Holistic Design Review

This document takes a step back from the code and looks at the library as a
whole: design choices worth revisiting, parts of the API that arguably do not
belong here, and functionality that is missing.

---

## 1. Library identity: adaptors over std types — embrace it

`fxt::tuple`, `fxt::optional`, `fxt::expected`, `fxt::variant` and `fxt::get`/
`fxt::apply`/`fxt::make_tuple` are plain aliases for their std counterparts.
That is the right call, but it has a consequence worth stating as a design
principle:

> **FXT is not a vocabulary-type library. It is a pipeline-adaptor library over
> the standard vocabulary types.**

Several current design decisions conflict with this identity:

- The `enums/` subsystem defines new vocabulary types unrelated to pipelines
  (see §4).
- The `tl::expected`/`tl::optional` escape hatches (`FXT_USE_TL_EXPECTED`) are
  good, but they only swap the alias — every adaptor that spells
  `fxt::expected<T, E>` concretely (e.g. `with()`, `to_expected`, `mget`,
  `mselect`) should instead be constrained on `expected_like`/`optional_like` so
  the library genuinely works with either backend, and with user-provided
  expected-likes.

**Action:** write this principle into the README, and audit every signature that
names `fxt::expected`/`fxt::optional` concretely where a concept would do.

---

## 2. Unify the pipe machinery (the single highest-leverage change)

There are currently **five independent `operator|` definitions** with three
different scoping strategies:

| Header              | LHS constraint            | Namespace |
|---------------------|---------------------------|-----------|
| `monads/Expected.hpp`  | `expected_like`         | `fxt` (found only by ordinary unqualified lookup — ADL does not help for std types) |
| `monads/Optional.hpp`  | `optional_like`         | `fxt` |
| `tuples/TuplePipe.hpp` | `tuple_like`            | **global** |
| `tuples/FlatTuple.hpp` | `flat_tuple` (4 overloads) | `fxt` (ADL works — flat_tuple lives in fxt) |
| `variants/VariantPipe.hpp` | `variant_like`      | **global** |

Plus `monads/With.hpp` and `monads/Sequence.hpp` add their own
`operator|(F&&, adaptor)` / `operator|(Container&&, adaptor)` overloads, and
`LogicalOr.hpp` adds twelve global `operator||` overloads.

Problems:

- The global-namespace operators leak into every TU that includes `fxt.hpp` and
  match *extremely* broadly, which can change overload resolution in unrelated
  code.
- The `fxt`-namespace operators for std types rely on ordinary unqualified lookup
  — fragile; users inside their own namespaces will shadow them.
- "Anything invocable with the container" as the dispatch rule means a stray
  lambda taking `auto&&` pipes happily, and diagnostics on mismatch are deep
  template errors.

**Proposed redesign:** adopt the standard-library range-adaptor-closure pattern.

1. Introduce one CRTP/base tag, e.g. `struct adaptor_closure_base {}` (or C++23
   `std::ranges::range_adaptor_closure`-style).
2. Every adaptor returned by `and_then(f)`, `transform(f)`, `take<N>()`, etc.
   derives from it.
3. Exactly **one** `operator|` exists, in namespace `fxt`, constrained on
   `derived_from<adaptor_closure_base>` for the RHS — found by ADL on the
   *adaptor*, which always lives in `fxt`.
4. Free composition falls out: `adaptor | adaptor` yields a fused closure,
   giving FXT function composition (§5.1).

This fixes the namespace pollution, the ADL fragility, the overly-broad
matching, and removes ~10 redundant operator definitions.

---

## 3. API surface that should shrink

| Keep | Remove / fold in | Why |
|------|------------------|-----|
| `tee`, `tee_error`, `tee_none` (rename to `tap`?) | `Tap.hpp` entirely | `tap`/`tee` are duplicates; `Tap.hpp` also has the dangling-reference return bug that was fixed in `VariantWhen.hpp` but not propagated here. One name, one file. |
| `mtuple_append` | `mappend` (`Append.hpp`) | Same operation, two names; mappend's error-conversion feature can be folded into mtuple_append. |
| `mapply` | The plain-tuple overloads inside `mselect` | `mselect`'s overload set mixes monadic and plain handling; `select`/`mselect` should mirror `take`/`mtake`. |
| `tuple_transform` | doc-name `transform_tuple` | One spelling; fix the ~6 files whose examples use the other. |
| `for_each` (ranges) + `tuple_for_each` | `tuple_foreach` spelling | One word-separation convention. |
| named function (e.g. `or_value`/`either`) | `operator\|\|` overloads (`LogicalOr.hpp`) | Operator syntax that silently evaluates both sides is a trap. |

### 3.1 One adaptor-invocation convention

`and_then(f)` is used bare; `value()`, `to_optional()`, `join()`, `index()`,
`take<2>()` need a trailing `()`. Half the doc examples get this wrong. With the
§2 closure design, nullary adaptors can simply *be* closure objects
(`inline constexpr join_t join{};`) so `| fxt::join` works without parentheses
— matching `std::views::reverse` precedent.

---

## 4. Parts that do not belong in this library

- **`enums/` (string_enum, type_enum, typed_string, EnumBase).** A
  reflection/vocabulary utility with no interaction with the pipe/monad
  machinery. It pulls in an external dependency (`fixed_string.hpp`) that
  nothing else uses and accounts for ~1,600 lines. It would thrive as a sibling
  library, keeping FXT dependency-free.
- **`utils/Immutable.hpp` and `utils/Lazy.hpp`.** General-purpose utilities with
  no connection to pipelines. If kept, `lazy` at least deserves monadic
  integration (e.g. `lazy<...> | fxt::transform(f)` producing a lazy) to
  justify its presence in an FP library.
- **`utils/Lift.hpp` macros (`FWD`, `LIFT`).** The functionality belongs; the
  unprefixed global macro names do not. Rename to `FXT_FWD`/`FXT_LIFT` with
  `#ifndef` guards.
- **`LogicalOr.hpp`** in its operator form — see §3 table.
- **`utils/TypeValue.hpp`.** Tag-dispatch utility used by nothing else in the
  library. The docs still refer to `fxt::Type<...>` which does not exist (the
  actual type is `fxt::type_value`); fix or remove.

---

## 5. Missing functionality worth adding

### 5.1 Function composition
For a functional library, the most conspicuous absence: there is no way to
compose two functions or two adaptors without a value flowing through.

```cpp
auto validate = fxt::transform(parse) | fxt::and_then(check);   // adaptor fusion (falls out of §2)
auto h = fxt::compose(f, g);                                    // h(x) == f(g(x))
```

### 5.2 Optional symmetry for the applicative layer
`with`/`mwith`, `sequence`, `traverse` are expected-only. The optional versions
(`Container<optional<T>> -> optional<Container<T>>`, `curry(f) | with(opt)`)
are natural, cheap, and users will expect them given everything else in the
library handles both monads.

### 5.3 Missing core monad operations
- `filter` / `take_if`: `optional<T> -> optional<T>` given a predicate.
  Very common; currently needs a hand-written `and_then`.
- `flatten`/`join` for `optional<optional<T>>` (join is expected-only today).
- `zip_with` (`mzip` + apply in one step): `mzip(a, b) | mapply(f)` is the
  documented idiom; a direct `zip_with(f, a, b...)` reads better.
- `to_expected` overload taking an error *factory* (lazy error construction)
  — the current version constructs the error even on the success path.
- `attempt`-style adaptor form: `| fxt::attempted` to wrap a throwing step
  mid-pipeline, complementing the free function.
- An `expects`/`ensure` assertion adaptor: `| fxt::ensure(pred, err)` for
  validation chains, the library's flagship use case (XLThermo validators).

### 5.4 Ranges story
`ranges/ForEach.hpp` is a lone outpost. Either commit to a small ranges layer
(`Container | traverse(f)` plus `sequence` covers most of it) or document that
std::ranges is the intended tool and keep only the monad×range bridges. A
`fxt::to_vector` / `ranges::to` bridge for sequencing into non-push_back
containers belongs here too.

### 5.5 Diagnostics & interop
- `std::format`/`operator<<` support for `expected`/`optional` pipeline results
  would make `tee`-based debugging pleasant. `failure` already has
  `operator<<` — extend the idea.
- A `fxt::result<T> = expected<T, failure>` alias: `attempt` already returns
  it; naming it makes signatures throughout user code shorter.

### 5.6 Quality infrastructure
- **Compile the documentation.** Several `@code` examples in doc comments do not
  compile (`fxt::value` vs `fxt::value()`, `fxt::append`, `transform_tuple`,
  `Index<>` vs `IndexOf<>`). Extract `@code` blocks into a doc-snippet test
  target so they can never rot.
- **Single-header / module distribution.** The 39-line license banner is
  duplicated in ~60 files (~2,300 lines); consider `// SPDX-License-Identifier:
  MIT` per file with the full text in LICENSE, and optionally a generated
  single-header + C++23 `export module fxt;` build.
- **IWYU pass.** Several headers compile only via transitive includes
  (`TupleForEach.hpp` uses `fxt::apply` without including `Apply.hpp`;
  `ranges/ForEach.hpp` lacks `<ranges>`/`<utility>`; `TuplePipe.hpp` lacks
  `<functional>`).
- **Adaptor reusability tests.** Store every curried adaptor in a variable and
  apply it twice — this would catch move-from-capture bugs immediately.

---

## 6. Suggested priority order

1. **Fix remaining bugs** — tap/tee duplication (dangling-return bug still
   present in `Tap.hpp`); `mapply` Case 2b (no inline TODO found — needs
   investigation); `TypeValue.hpp` doc/name mismatch.
2. **§2 pipe unification** — everything else (composition, bare nullary
   adaptors, removing global operators) builds on it.
3. **§3 surface reduction** (tap/tee consolidation, mappend, naming cleanup)
   — do this *before* 1.0, while breaking changes are free.
4. **§5.6 doc-snippet tests** — locks in the cleanup.
5. **§5.2/5.3 completeness** — optional applicative layer, missing monad ops.
6. **§4 scope split** (`enums/` extraction) — whenever a second consumer
   appears or the fixed_string dependency becomes annoying.
