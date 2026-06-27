# FXT — Holistic Design Review

This document complements the inline `// TODO:` comments in the headers (grep for
`TODO: BUG`, `TODO: SAFETY`, etc. for localized issues). It takes a step back and
looks at the library as a whole: design choices worth revisiting, parts of the API
that arguably do not belong here, and functionality that is missing.

---

## Resolved (this refactor)

The following items from this document (and associated inline TODOs) have been
addressed. They are struck through in their original sections below.

| Item | What was done |
|------|---------------|
| `failure` hash bug | `std::hash<fxt::failure>` now hashes `message_view()` only, consistent with `operator==`. |
| `attempt` noexcept safety | `failure(exception_ptr)` is now non-allocating (message extracted lazily); `attempt`'s `noexcept` is now truthful. |
| `VariantWhen.hpp` dangling reference | All four overloads (`when`/`mwhen`, direct + curried) now return by value for rvalue inputs via `std::conditional_t` trailing return type. Constraint tightened to `std::invocable<F, T&>`. |
| `VariantTransformWhen.hpp` dangling reference | Same fix applied to all four `transform_when`/`mtransform_when` overloads. |
| `VariantHoldsAlternative.hpp` missing pipe form | Added `holds_alternative<T>()`, `mholds_alternative<T>(monad)`, and `mholds_alternative<T>()` pipe form; tests and demo written. |
| `Curry.hpp` `inline auto` → `inline constexpr auto` | Done; self-reference changed to `this auto const&`. |
| `TupleAppend.hpp` / `TuplePrepend.hpp` MSVC `N` capture | Inner lambda now receives `std::make_index_sequence<N>{}` as an argument rather than capturing `N`, fixing MSVC C3493. |
| `Lazy.hpp` — explicit copy/move constructors | `explicit` removed; `auto copy = expensive;` now compiles. |
| `Lazy.hpp` — dead `make_monad_result` trait | Removed; it was unused and broke self-containment (referenced `fxt::expected`/`fxt::failure` without including them). |
| `Lazy.hpp` — commented-out reference implementations | ~85 lines of dead code removed. |
| `Lazy.hpp` — redundant `optional<variant<V,E>>` storage | Replaced with `variant<monostate, V, E>`. |
| `Lazy.hpp` — callable never freed | `state::function` is now `optional<Fn>`; reset after `call_once` completes. |
| `Lazy.hpp` — concept under-constrains | `LazyInvocable` now requires `copy_constructible` (move-only types rejected) and excludes `exception_ptr` / `monostate` return types. |
| `Lazy.hpp` — dangling reference from `value()` | All accessors return `value_type` by value; `value()` and `operator const value_type&()` have been eliminated. |
| `~400 lines of commented-out code` (§3 table) | `Lazy.hpp` cleaned this session; `Tuple.hpp`, `Optional.hpp`, `Expected.hpp`, `Get.hpp`, `Apply.hpp`, `ApplyReplace.hpp`, `TuplePipe.hpp`, `IsOptional.hpp` were already clean. |
| `broken tap` (Priority 1) | `tap` already returns the container (not `void`); the "returns void" description was stale. The tap/tee duplication (see §3) remains open. |
| `flat_tuple` `if constexpr` dispatch branches (§3.1 remaining concern) | Added `tuple_kind<Tuple>`, `tuple_rebind_t<Tuple, NewTs...>`, and `make_tuple_like<Tuple>(args...)` to `IsTuple.hpp`. All `if constexpr (is_fxt_tuple_v) / else (is_flat_tuple_v)` dispatch branches eliminated from `TupleAppend`, `TuplePrepend`, `TupleReverse`, `TupleTake`, `TupleDrop`, `TupleSelect`, `TupleTransform`, and `ApplyReplace`. Adding a third tuple kind now requires only a `tuple_kind` specialisation — no algorithm changes. |
| `m`-prefix family: generate, don't hand-write (§3.2) | Added `fxt::lifted(adaptor)` in `monads/Lifted.hpp`. Added nullary `fxt::get<I>()` / `fxt::get<T>()` adaptor forms to `Get.hpp`. Reduced 10 m-functions (`mindex`, `mvisit`, `mget<I/T>`, `mtuple_select`, `mtuple_transform`, `mtuple_reverse`, `mtuple_take`, `mtuple_take_last`, `mtuple_drop`, `mtuple_drop_last`) to one-liners over `lifted`. The remaining 11 bespoke m-functions (`mwhen`, `mtransform_when`, `mholds_alternative`, `mtuple_append`, `mtuple_prepend`, `mtuple_cat` two-monad, `mtuple_for_each`, `mtuple_as_array`, `mapply`, `mwith`, `mzip`) have semantics that genuinely exceed a plain `.transform()` and are kept as-is. |

---

## 1. Library identity: adaptors over std types — embrace it

`fxt::tuple`, `fxt::optional`, `fxt::expected`, `fxt::variant` and `fxt::get`/
`fxt::apply`/`fxt::make_tuple` are now plain aliases for their std counterparts.
That is the right call (the commented-out wrapper experiments in `Tuple.hpp`
confirm the alternative was painful), but it has a consequence worth stating as a
design principle:

> **FXT is not a vocabulary-type library. It is a pipeline-adaptor library over
> the standard vocabulary types.**

Several current design decisions conflict with this identity:

- `flat_tuple` is the one remaining *bespoke* vocabulary type, and it does not
  deliver its stated promise (see §3).
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
| `monads/Expected.hpp`  | `expected_like`         | `fxt` (found via... nothing — std types get no ADL into `fxt`; works only because call sites are inside `fxt` or via `using`) |
| `monads/Optional.hpp`  | `optional_like`         | `fxt` |
| `tuples/TuplePipe.hpp` | `tuple_like`            | **global** |
| `tuples/FlatTuple.hpp` | `flat_tuple` (4 overloads) | `fxt` (ADL works — flat_tuple lives in fxt) |
| `variants/VariantPipe.hpp` | `variant_like`      | **global** |

Plus `monads/With.hpp` and `monads/Sequence.hpp` add their own
`operator|(F&&, adaptor)` / `operator|(Container&&, adaptor)` overloads, and
`LogicalOr.hpp` adds twelve global `operator||` overloads.

Problems with the current scheme:

- The global-namespace operators leak into every TU that includes `fxt.hpp` and
  match *extremely* broadly (`any std::tuple | any compatible callable`), which
  can change overload resolution in unrelated code.
- The `fxt`-namespace operators for std types are only found when the call site
  can see them by ordinary unqualified lookup — subtle and fragile; users inside
  their own namespaces with their own `operator|` will shadow them.
- "Anything invocable with the container" as the dispatch rule means a stray
  lambda taking `auto&&` pipes happily, and diagnostics on mismatch are deep
  template errors rather than "not an fxt adaptor".

**Proposed redesign:** adopt the standard-library range-adaptor-closure pattern.

1. Introduce one CRTP/base tag, e.g. `struct adaptor_closure_base {}` (or C++23
   `std::ranges::range_adaptor_closure`-style).
2. Every adaptor returned by `and_then(f)`, `transform(f)`, `take<N>()`,
   `traverse(f)`, `with(ex)`, ... derives from it.
3. Exactly **one** `operator|` exists, in namespace `fxt`, constrained on
   `derived_from<adaptor_closure_base>` for the RHS — found by ADL on the
   *adaptor*, which always lives in `fxt`, so it works regardless of what the
   LHS is and never fires for non-FXT callables.
4. Free composition falls out almost for free: `adaptor | adaptor` can yield a
   fused closure (like ranges), giving FXT function composition (§5.1).

This one change fixes the namespace pollution, the ADL fragility, the
overly-broad matching, and removes ~10 redundant operator definitions.

---

## 3. API surface that should shrink

The library has grown by accretion and several areas now have two (or three)
ways to do the same thing. Pick winners:

| Keep | Remove / fold in | Why |
|------|------------------|-----|
| `tee`, `tee_error`, `tee_none` (renamed `tap`?) | `Tap.hpp` entirely | `tap`/`tee` are duplicates; `Tap.hpp` also has the dangling-reference return bug (`std::forward` on rvalue container) that was fixed in `VariantWhen.hpp` but not here. One name, one file. |
| `mtuple_append` | `mappend` (`Append.hpp`) | Same operation, two names; mappend's error-conversion feature can be folded into mtuple_append. |
| `mapply` | The plain-tuple overloads inside `mselect` | `mselect`'s overload set mixes monadic and plain handling; `select`/`mselect` should mirror `take`/`mtake`. |
| `tuple_transform` | doc-name `transform_tuple` | One spelling; fix the ~6 files whose examples use the other. |
| `for_each` (ranges) + `tuple_for_each` | `tuple_foreach` spelling | One word-separation convention. |
| named function (e.g. `or_value`/`either`) | `operator\|\|` overloads (`LogicalOr.hpp`) | Operator syntax that silently evaluates both sides is a trap; a named function makes the eager evaluation visible. |

### 3.1 `flat_tuple`: ✓ resolved

The previous criticism (variant-per-slot storage, no tuple protocol, no
comparisons, non-constexpr/non-noexcept `get`) no longer applies. The
implementation was substantially refactored:

- **Storage**: each element `Ts[I]` lives in a `flat_leaf<I, T>` base class.
  Memory layout is identical to `struct { T0 e0; T1 e1; ... }` — no variant,
  no discriminator, no `max(sizeof(Ts)...)` overhead.
- **Access**: `static_cast` to the appropriate base — O(1), `constexpr`,
  `noexcept`, no runtime branching.
- **Instantiation depth**: O(1) via a single pack-expansion inheritance step
  (vs. `std::tuple`'s O(N) recursive chain) — the stated design goal is
  **compilation speed**, which is legitimate and measurable.
- **Tuple protocol**: `std::tuple_size` and `std::tuple_element` are specialised
  in `TupleSize.hpp` / `TupleElement.hpp`; structured bindings and `std::apply`
  work.
- **Comparisons**: `operator==` and `operator<=>` are implemented.
- **`get`**: `constexpr` and `noexcept`.
- **Algorithm dispatch**: `fxt::impl::tuple_kind<Tuple>` / `tuple_rebind_t` /
  `make_tuple_like` added to `IsTuple.hpp`. All `if constexpr (is_fxt_tuple_v) /
  else (is_flat_tuple_v)` branches in the eight affected algorithm headers have
  been eliminated. Adding a new tuple kind requires only a `tuple_kind`
  specialisation — no algorithm changes.

### 3.2 The `m`-prefix family: ✓ resolved

`fxt::lifted(adaptor)` has been added in `monads/Lifted.hpp`. It is the single
implementation of the pattern `M&& m -> m.transform(adaptor)` shared by all
trivially-liftable m-functions.

Ten m-functions are now one-liners over `lifted`:

| m-function | delegates to |
|---|---|
| `mindex` | `lifted(index())` |
| `mvisit(f)` | `lifted(visit(f))` |
| `mget<I/T>()` | `lifted(get<I/T>())` |
| `mtuple_select<Is/Ts...>()` | `lifted(tuple_select<...>())` |
| `mtuple_transform(f)` | `lifted(tuple_transform(f))` |
| `mtuple_reverse` | `lifted(tuple_reverse())` |
| `mtuple_take<X>` / `mtuple_take_last<X>` | `lifted(tuple_take<X>())` / `lifted(tuple_take_last<X>())` |
| `mtuple_drop<X>` / `mtuple_drop_last<X>` | `lifted(tuple_drop<X>())` / `lifted(tuple_drop_last<X>())` |

Nullary `fxt::get<I>()` and `fxt::get<T>()` adaptor forms were added to
`Get.hpp` as part of this change.

The remaining 11 bespoke m-functions (`mwhen`, `mtransform_when`,
`mholds_alternative`, `mtuple_append`, `mtuple_prepend`, `mtuple_cat`
two-monad form, `mtuple_for_each`, `mtuple_as_array`, `mapply`, `mwith`,
`mzip`) have semantics that genuinely exceed a plain `.transform()` and are
kept as hand-written implementations.

### 3.3 One adaptor-invocation convention

`and_then(f)` is used bare; `value()`, `to_optional()`, `join()`, `index()`,
`take<2>()` need a trailing `()`. Half the doc examples get this wrong, which is
strong evidence users will too. With the §2 closure design, nullary adaptors can
simply *be* closure objects (`inline constexpr join_t join{};`) so `| fxt::join`
works without parentheses — matching `std::views::reverse` precedent.

---

## 4. Parts that do not belong in this library

- **`enums/` (string_enum, type_enum, typed_string, EnumBase).** Well-built, but
  it is a *reflection/vocabulary* utility, not a functional-composition tool: it
  has zero interaction with the pipe/monad machinery, pulls in an external
  dependency (`fixed_string.hpp`) that nothing else uses, uses a different
  naming convention (PascalCase helpers), and accounts for ~1,600 lines. It
  would thrive as a sibling library (`xte`?), keeping FXT dependency-free.
- **`utils/Immutable.hpp` and `utils/Lazy.hpp`.** General-purpose utilities with
  no connection to pipelines. Less urgent than `enums/` (they're small and
  dependency-free), but they widen the library's stated scope. If kept, `lazy`
  at least deserves monadic integration (e.g. `lazy<...> | fxt::transform(f)`
  producing a lazy) to justify its presence in an FP library.
- **`utils/Lift.hpp` macros (`FWD`, `LIFT`).** The functionality belongs; the
  unprefixed global macro names do not. `FXT_FWD`/`FXT_LIFT`, with `#ifndef`
  guards.
- **`LogicalOr.hpp`** in its operator form — see §3 table.
- **`utils/TypeValue.hpp`.** Tag-dispatch utility used by nothing else in the
  library. The docs still refer to `fxt::Type<...>` which does not exist (the
  actual type is `fxt::type_value`); fix or remove.

A reasonable end-state: `fxt` = concepts + monads + tuples + variants + ranges +
the FP utils (curry, overload, attempt/failure, unit), everything else split out.

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
- `filter` / `take_if`: `optional<T> -> optional<T>` given a predicate
  (and `expected` -> `expected` given predicate + error). Very common; currently
  needs a hand-written `and_then`.
- `flatten`/`join` for `optional<optional<T>>` (join is expected-only today —
  already flagged inline).
- `zip_with` (`mzip` + apply in one step): `mzip(a, b) | mapply(f)` is the
  documented idiom; a direct `zip_with(f, a, b...)` reads better.
- `to_expected` overload taking an error *factory* (lazy error construction)
  — the current version constructs the error even on the success path.
- `attempt`-style adaptor form: `| fxt::attempted` to wrap a throwing step
  mid-pipeline, complementing the free function.
- An `expects`/`ensure` assertion adaptor: `| fxt::ensure(pred, err)` for
  validation chains, which is the library's flagship use case (XLThermo
  validators).

### 5.4 Ranges story
`ranges/ForEach.hpp` is a lone outpost. Either commit to a small ranges layer —
`fxt::traverse` already overlaps with it; `Container | traverse(f)` plus
`sequence` is most of what's needed — or document that std::ranges is the
intended tool and keep only the monad×range bridges (`sequence`/`traverse`).
A `fxt::to_vector` / `ranges::to` bridge for sequencing into non-push_back
containers belongs here too.

### 5.5 Diagnostics & interop
- `std::format`/`operator<<` support for `expected`/`optional` pipelines'
  results would make `tee`-based debugging pleasant (`| fxt::tee(print)`).
  `string_enum` already has a formatter; `failure` has `operator<<` — extend
  the idea.
- A `fxt::result<T> = expected<T, failure>` alias: `attempt` already returns
  it; naming it makes signatures throughout user code shorter and nudges users
  toward a single error type per codebase.

### 5.6 Quality infrastructure
- **Compile the documentation.** A large fraction of the inline-TODO findings
  were doc examples that do not compile (`fxt::value` vs `fxt::value()`,
  `fxt::append`, `transform_tuple`, `Index<>` vs `IndexOf<>`). Note: `lazy`'s
  `auto copy = expensive;` now compiles correctly. Extract remaining `@code`
  blocks into a doc-snippet test target so they can never rot again.
- **Single-header / module distribution.** The 39-line license banner is
  duplicated in ~60 files (~2,300 lines); consider a one-line SPDX header
  (`// SPDX-License-Identifier: MIT`) per file with the full text in LICENSE,
  and optionally a generated single-header + C++23 `export module fxt;` build.
- **IWYU pass.** Several headers compile only via transitive includes
  (`TupleForEach.hpp` uses `fxt::apply` without including `Apply.hpp`;
  `ranges/ForEach.hpp` lacks `<ranges>`/`<utility>`; `TuplePipe.hpp` lacks
  `<functional>`).
- **Adaptor reusability tests.** Add tests that store every curried adaptor in
  a variable and apply it twice — this would have caught the single-use
  (move-from-capture) bugs and the broken `tap` immediately.

---

## 6. Suggested priority order

1. ~~**Fix the outright bugs**~~ (`failure` hash ✓, `attempt` noexcept ✓,
   `VariantWhen`/`VariantTransformWhen` dangling returns ✓, `flat_tuple` algorithm
   dispatch branches ✓) — **Remaining:** tap/tee duplication (dangling-return bug
   still present in `Tap.hpp`), `mapply` Case 2b (no inline TODO found — needs
   investigation), `TypeValue.hpp` doc/name mismatch.
2. **§2 pipe unification** — everything else (composition, bare nullary
   adaptors, removing global operators) builds on it.
3. **§3 surface reduction** (tap/tee, mappend, dead code, flat_tuple decision)
   — do this *before* 1.0, while breaking changes are free.
4. **§5.6 doc-snippet tests** — locks in the cleanup.
5. ~~**§3.2 lifted()**~~ ✓ — **§5.2/5.3 completeness** — the feature work.
6. **§4 scope split** (`enums/` extraction) — whenever a second consumer
   appears or the dependency on fixed_string becomes annoying.
