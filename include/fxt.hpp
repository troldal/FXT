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

#include "fxt/concepts/IsExpected.hpp"
#include "fxt/concepts/IsMonad.hpp"
#include "fxt/concepts/IsOptional.hpp"
#include "fxt/concepts/IsTuple.hpp"
#include "fxt/concepts/IsVariant.hpp"
#include "fxt/enums/StringEnum.hpp"
#include "fxt/enums/TypeEnum.hpp"
#include "fxt/enums/TypedString.hpp"
#include "fxt/enums/EnumBase.hpp"
#include "fxt/monads/AndThen.hpp"
#include "fxt/monads/Expected.hpp"
#include "fxt/monads/Join.hpp"
#include "fxt/monads/Lifted.hpp"
#include "fxt/monads/LogicalOr.hpp"
#include "fxt/monads/Match.hpp"
#include "fxt/monads/Optional.hpp"
#include "fxt/monads/OrElse.hpp"
#include "fxt/monads/Tap.hpp"
#include "fxt/monads/ToExpected.hpp"
#include "fxt/monads/ToOptional.hpp"
#include "fxt/monads/Transform.hpp"
#include "fxt/monads/TransformError.hpp"
#include "fxt/monads/Value.hpp"
#include "fxt/monads/ValueOr.hpp"
#include "fxt/monads/With.hpp"
#include "fxt/monads/Sequence.hpp"
#include "fxt/ranges/ForEach.hpp"
#include "fxt/tuples/Apply.hpp"
#include "fxt/tuples/ApplyAppend.hpp"
#include "fxt/tuples/ApplyReplace.hpp"
#include "fxt/tuples/TupleAsArray.hpp"
#include "fxt/tuples/TupleDrop.hpp"
#include "fxt/tuples/FlatTuple.hpp"
#include "fxt/tuples/Get.hpp"
#include "fxt/monads/Zip.hpp"
#include "fxt/tuples/TupleSelect.hpp"
#include "fxt/tuples/TupleTake.hpp"
#include "fxt/tuples/Tuple.hpp"
#include "fxt/tuples/TupleAppend.hpp"
#include "fxt/tuples/TupleCat.hpp"
#include "fxt/tuples/TupleElement.hpp"
#include "fxt/tuples/TupleForEach.hpp"
#include "fxt/tuples/TuplePipe.hpp"
#include "fxt/tuples/TuplePrepend.hpp"
#include "fxt/tuples/TupleReverse.hpp"
#include "fxt/tuples/TupleSize.hpp"
#include "fxt/tuples/TupleTransform.hpp"
#include "fxt/utils/Attempt.hpp"
#include "fxt/utils/Curry.hpp"
#include "fxt/utils/Failure.hpp"
#include "fxt/utils/Immutable.hpp"
#include "fxt/utils/Lazy.hpp"
#include "fxt/utils/Lift.hpp"
#include "fxt/utils/Overload.hpp"
#include "fxt/utils/TypeValue.hpp"
#include "fxt/utils/Unit.hpp"
#include "fxt/variants/Variant.hpp"
#include "fxt/variants/VariantHoldsAlternative.hpp"
#include "fxt/variants/VariantIndex.hpp"
#include "fxt/variants/VariantPipe.hpp"
#include "fxt/variants/VariantTransformWhen.hpp"
#include "fxt/variants/VariantVisit.hpp"
#include "fxt/variants/VariantWhen.hpp"
