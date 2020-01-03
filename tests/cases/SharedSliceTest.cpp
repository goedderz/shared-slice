////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2020 ArangoDB GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Tobias Gödderz
////////////////////////////////////////////////////////////////////////////////

#include "gtest/gtest.h"

#include "velocypack/SharedSlice.h"

#include <velocypack/Builder.h>
#include <velocypack/Slice.h>

#include <variant>

using namespace arangodb;
using namespace arangodb::velocypack;

void initTestCases(std::vector<Builder>& testCases) {
  {
    auto& builder = testCases.emplace_back();
    builder.add(Slice::noneSlice());
  }
  {
    auto& builder = testCases.emplace_back();
    builder.add(Slice::illegalSlice());
  }
  {
    auto& builder = testCases.emplace_back();
    builder.add(Slice::nullSlice());
  }
  {
    auto& builder = testCases.emplace_back();
    builder.add(Slice::trueSlice());
  }
  {
    auto& builder = testCases.emplace_back();
    builder.add(Slice::falseSlice());
  }
  {
    auto& builder = testCases.emplace_back();
    builder.add(Slice::zeroSlice());
  }
  {
    auto& builder = testCases.emplace_back();
    builder.add(Slice::emptyStringSlice());
  }
  {
    auto& builder = testCases.emplace_back();
    builder.add(Slice::emptyArraySlice());
  }
  {
    auto& builder = testCases.emplace_back();
    builder.add(Slice::emptyObjectSlice());
  }
  {
    auto& builder = testCases.emplace_back();
    builder.add(Slice::minKeySlice());
  }
  {
    auto& builder = testCases.emplace_back();
    builder.add(Slice::maxKeySlice());
  }
  {
    auto& builder = testCases.emplace_back();
    builder.openArray();
    builder.add(Value(42));
    builder.close();
  }
  {
    auto& builder = testCases.emplace_back();
    builder.openArray();
    builder.add(Value("42"));
    builder.close();
  }
  {
    auto& builder = testCases.emplace_back();
    builder.openObject();
    builder.add("foo", Value(42));
    builder.close();
  }
  {
    auto& builder = testCases.emplace_back();
    builder.openObject();
    builder.add("bar", Value("42"));
    builder.close();
  }
  {
    auto& builder = testCases.emplace_back();
    builder.addTagged(42, Value("42"));
  }
}

std::vector<Builder> const& testCases() {
  static std::vector<Builder> _testCases{};

  if (_testCases.empty()) {
    initTestCases(_testCases);
  }

  return _testCases;
}

template<typename F>
void forAllTestCases(F f) {
  for (auto const& builder : testCases()) {
    ASSERT_TRUE(builder.isClosed());
    auto slice = builder.slice();
    auto sharedSlice = SharedSlice(builder.buffer());
    // both should point to the exact same data
    ASSERT_EQ(slice.begin(), sharedSlice.slice().begin());
    f(slice, sharedSlice);
  }
}

/**
 * @brief Can hold either a value or an exception, and is constructed by
 *        executing a callback. Holds either the value the callback returns, or
 *        the velocypack::Exception it throws.
 *        Can then be used to compare the result/exception with another instance.
 */
template<typename V>
class ResultOrException {
 public:
  template<typename F>
  explicit ResultOrException(F f) {
    try {
      variant.template emplace<V>(f());
    } catch (Exception& e) {
      variant.template emplace<Exception>(e);
    }
  }

  bool operator==(ResultOrException<V> const& other) const {
    if (variant.index() != other.variant.index()) {
      return false;
    }

    switch(variant.index()) {
      case 0: {
        auto const& left = std::get<0>(variant);
        auto const& right = std::get<0>(other.variant);
        return left == right;
      }
      case 1: {
        auto const& left = std::get<1>(variant);
        auto const& right = std::get<1>(other.variant);
        return left.errorCode() == right.errorCode();
      }
      case std::variant_npos:
        throw std::exception();
    }

    throw std::exception();
  }

  std::variant<V, Exception> variant;
};

template<typename F> ResultOrException(F) -> ResultOrException<std::invoke_result_t<F>>;

#define R(a) ResultOrException([&](){ return a; })

TEST(SharedSliceTest, value) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
    ASSERT_EQ(slice.value().begin(), sharedSlice.value().slice().begin());
  });
}

TEST(SharedSliceTest, getFirstTag) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
    ASSERT_EQ(R(slice.getFirstTag()), R(sharedSlice.getFirstTag()));
  });
}

TEST(SharedSliceTest, getTags) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
    ASSERT_EQ(R(slice.getTags()), R(sharedSlice.getTags()));
  });
}

TEST(SharedSliceTest, hasTag) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
    ASSERT_EQ(R(slice.hasTag(42)), R(sharedSlice.hasTag(42)));
  });
}

TEST(SharedSliceTest, valueStart) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
    ASSERT_EQ(R(slice.valueStart()), R(sharedSlice.valueStart().get()));
  });
}

TEST(SharedSliceTest, start) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
    ASSERT_EQ(R(slice.start()), R(sharedSlice.start().get()));
  });
}

TEST(SharedSliceTest, startAs) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
    ASSERT_EQ(R(slice.startAs<void*>()), R(sharedSlice.startAs<void*>().get()));
  });
}

TEST(SharedSliceTest, head) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
    ASSERT_EQ(R(slice.head()), R(sharedSlice.head()));
  });
}
// TODO continue writing the following tests
TEST(SharedSliceTest, begin) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, end) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, type) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, typeName) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, hash) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, hash32) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, hashSlow) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, normalizedHash) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, normalizedHash32) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, hashString) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, hashString32) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isType) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isNone) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isIllegal) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isNull) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isBool) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isBoolean) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isTrue) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isFalse) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isArray) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isObject) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isDouble) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isUTCDate) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isExternal) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isMinKey) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isMaxKey) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isInt) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isUInt) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isSmallInt) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isString) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isBinary) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isBCD) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isCustom) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isTagged) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isInteger) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isNumber) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isNumberT) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isSorted) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getBool) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getBoolean) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getDouble) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, at) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, operatorIndexPValueLength) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, length) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, keyAt) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, valueAt) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getNthValue) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getPVector) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getPStringRef) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getPString) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getPCharPtr) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getPCharPtrLen) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, operatorIndexPStringRef) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, operatorIndexPString) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, hasKeyPStringRef) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, hasKeyPString) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, hasKeyPCharPtr) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, hasKeyPCharPtrLen) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, hasKeyPVector) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getExternal) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, resolveExternal) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, resolveExternals) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isEmptyArray) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isEmptyObject) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, translate) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getInt) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getUInt) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getSmallInt) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getNumber) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getNumericValue) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getUTCDate) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getString) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getStringUnchecked) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getStringLength) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, copyString) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, stringRef) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, stringView) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getBinary) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getBinaryLength) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, copyBinary) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, byteSize) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, valueByteSize) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, findDataOffset) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getNthOffset) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, makeKey) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, compareStringPStringRef) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, compareStringPString) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, compareStringPCharPtrLen) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, compareStringUncheckedPStringRef) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, compareStringUncheckedPString) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, compareStringUncheckedPCharPtrLen) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isEqualStringPStringRef) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isEqualStringPString) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isEqualStringUncheckedPStringRef) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, isEqualStringUncheckedPString) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, binaryEquals) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, toHex) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, toJson) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, toString) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, hexType) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getIntUnchecked) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getUIntUnchecked) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getSmallIntUnchecked) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}

TEST(SharedSliceTest, getBCD) {
  forAllTestCases([&](Slice slice, SharedSlice sharedSlice) {
  });
}
