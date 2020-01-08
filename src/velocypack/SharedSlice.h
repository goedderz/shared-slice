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

#ifndef SRC_SHAREDSLICE_H
#define SRC_SHAREDSLICE_H

#include <velocypack/Buffer.h>
#include <velocypack/Slice.h>

#include <memory>

namespace arangodb::velocypack {

class SharedSlice {
 public:
  explicit SharedSlice(std::shared_ptr<uint8_t const>&& data) noexcept;
  explicit SharedSlice(std::shared_ptr<uint8_t const> const& data) noexcept;
  explicit SharedSlice(std::shared_ptr<Buffer<uint8_t> const>&& buffer) noexcept;
  explicit SharedSlice(std::shared_ptr<Buffer<uint8_t> const> const& buffer) noexcept;

  // Aliasing constructor
  explicit SharedSlice(SharedSlice&& sharedPtr, Slice slice) noexcept;
  explicit SharedSlice(SharedSlice const& sharedPtr, Slice slice) noexcept;

  // Default constructor, points to a (static) None slice
  SharedSlice() noexcept;

  // Copy & move constructor & assignment
  SharedSlice(SharedSlice const&) = default;
  SharedSlice(SharedSlice&&) noexcept;
  SharedSlice& operator=(SharedSlice const&) = default;
  SharedSlice& operator=(SharedSlice&&) noexcept;
  ~SharedSlice() = default;

  // Accessor of the SharedSlice's buffer
  [[nodiscard]] std::shared_ptr<uint8_t const> const& buffer() const noexcept { return _start; }

  // Access the buffer as a Slice
  [[nodiscard]] Slice slice() const noexcept;

  /**************************************
   * Everything else delegates to Slice
   **************************************/

  static constexpr auto defaultSeed = Slice::defaultSeed;
  static constexpr auto defaultSeed32 = Slice::defaultSeed32;
  static constexpr auto defaultSeed64 = Slice::defaultSeed64;

  [[nodiscard]] SharedSlice value() const noexcept;

  [[nodiscard]] uint64_t getFirstTag() const;

  [[nodiscard]] std::vector<uint64_t> getTags() const;

  [[nodiscard]] bool hasTag(uint64_t tagId) const;

  [[nodiscard]] std::shared_ptr<uint8_t const> valueStart() const noexcept;

  [[nodiscard]] std::shared_ptr<uint8_t const> start() const noexcept;

  template <typename T>
  [[nodiscard]] std::shared_ptr<T const> startAs() const {
    return aliasPtr(slice().startAs<T>());
  }

  [[nodiscard]] uint8_t head() const noexcept;

  [[nodiscard]] std::shared_ptr<uint8_t const> begin() const noexcept;

  [[nodiscard]] std::shared_ptr<uint8_t const> end() const;

  [[nodiscard]] ValueType type() const noexcept;

  [[nodiscard]] char const* typeName() const;

  // I didn't implement set(), it seems to fragile here.
  // void set(uint8_t const* s);

  [[nodiscard]] uint64_t hash(uint64_t seed = defaultSeed64) const;

  [[nodiscard]] uint32_t hash32(uint32_t seed = defaultSeed32) const;

  [[nodiscard]] uint64_t hashSlow(uint64_t seed = defaultSeed64) const;

  [[nodiscard]] uint64_t normalizedHash(uint64_t seed = defaultSeed64) const;

  [[nodiscard]] uint32_t normalizedHash32(uint32_t seed = defaultSeed32) const;

  [[nodiscard]] uint64_t hashString(uint64_t seed = defaultSeed64) const noexcept;

  [[nodiscard]] uint32_t hashString32(uint32_t seed = defaultSeed32) const noexcept;

  [[nodiscard]] bool isType(ValueType t) const;

  [[nodiscard]] bool isNone() const noexcept;

  [[nodiscard]] bool isIllegal() const noexcept;

  [[nodiscard]] bool isNull() const noexcept;

  [[nodiscard]] bool isBool() const noexcept;

  [[nodiscard]] bool isBoolean() const noexcept;

  [[nodiscard]] bool isTrue() const noexcept;

  [[nodiscard]] bool isFalse() const noexcept;

  [[nodiscard]] bool isArray() const noexcept;

  [[nodiscard]] bool isObject() const noexcept;

  [[nodiscard]] bool isDouble() const noexcept;

  [[nodiscard]] bool isUTCDate() const noexcept;

  [[nodiscard]] bool isExternal() const noexcept;

  [[nodiscard]] bool isMinKey() const noexcept;

  [[nodiscard]] bool isMaxKey() const noexcept;

  [[nodiscard]] bool isInt() const noexcept;

  [[nodiscard]] bool isUInt() const noexcept;

  [[nodiscard]] bool isSmallInt() const noexcept;

  [[nodiscard]] bool isString() const noexcept;

  [[nodiscard]] bool isBinary() const noexcept;

  [[nodiscard]] bool isBCD() const noexcept;

  [[nodiscard]] bool isCustom() const noexcept;

  [[nodiscard]] bool isTagged() const noexcept;

  [[nodiscard]] bool isInteger() const noexcept;

  [[nodiscard]] bool isNumber() const noexcept;

  template <typename T>
  [[nodiscard]] bool isNumber() const noexcept {
    return slice().isNumber<T>();
  }

  [[nodiscard]] bool isSorted() const noexcept;

  [[nodiscard]] bool getBool() const;

  [[nodiscard]] bool getBoolean() const;

  [[nodiscard]] double getDouble() const;

  [[nodiscard]] SharedSlice at(ValueLength index) const;

  [[nodiscard]] SharedSlice operator[](ValueLength index) const;

  [[nodiscard]] ValueLength length() const;

  [[nodiscard]] SharedSlice keyAt(ValueLength index, bool translate = true) const;

  [[nodiscard]] SharedSlice valueAt(ValueLength index) const;

  [[nodiscard]] SharedSlice getNthValue(ValueLength index) const;

  template <typename T>
  [[nodiscard]] SharedSlice get(std::vector<T> const& attributes, bool resolveExternals = false) const {
    return alias(slice().get(attributes, resolveExternals));
  }

  [[nodiscard]] SharedSlice get(StringRef const& attribute) const;

  [[nodiscard]] SharedSlice get(std::string const& attribute) const;

  [[nodiscard]] SharedSlice get(char const* attribute) const;

  [[nodiscard]] SharedSlice get(char const* attribute, std::size_t length) const;

  [[nodiscard]] SharedSlice operator[](StringRef const& attribute) const;

  [[nodiscard]] SharedSlice operator[](std::string const& attribute) const;

  [[nodiscard]] bool hasKey(StringRef const& attribute) const;

  [[nodiscard]] bool hasKey(std::string const& attribute) const;

  [[nodiscard]] bool hasKey(char const* attribute) const;

  [[nodiscard]] bool hasKey(char const* attribute, std::size_t length) const;

  [[nodiscard]] bool hasKey(std::vector<std::string> const& attributes) const;

  [[nodiscard]] std::shared_ptr<char const> getExternal() const;

  [[nodiscard]] SharedSlice resolveExternal() const;

  [[nodiscard]] SharedSlice resolveExternals() const;

  [[nodiscard]] bool isEmptyArray() const;

  [[nodiscard]] bool isEmptyObject() const;

  [[nodiscard]] SharedSlice translate() const;

  [[nodiscard]] int64_t getInt() const;

  [[nodiscard]] uint64_t getUInt() const;

  [[nodiscard]] int64_t getSmallInt() const;

  template <typename T>
  [[nodiscard]] T getNumber() const {
    return slice().getNumber<T>();
  }

  template <typename T>
  [[nodiscard]] T getNumericValue() const {
    return slice().getNumericValue<T>();
  }

  [[nodiscard]] int64_t getUTCDate() const;

  [[nodiscard]] std::shared_ptr<char const> getString(ValueLength& length) const;

  [[nodiscard]] std::shared_ptr<char const> getStringUnchecked(ValueLength& length) const noexcept;

  [[nodiscard]] ValueLength getStringLength() const;

  [[nodiscard]] std::string copyString() const;

  [[nodiscard]] StringRef stringRef() const;
#ifdef VELOCYPACK_HAS_STRING_VIEW
  [[nodiscard]] std::string_view stringView() const;
#endif

  [[nodiscard]] std::shared_ptr<uint8_t const> getBinary(ValueLength& length) const;

  [[nodiscard]] ValueLength getBinaryLength() const;

  [[nodiscard]] std::vector<uint8_t> copyBinary() const;

  [[nodiscard]] ValueLength byteSize() const;

  [[nodiscard]] ValueLength valueByteSize() const;

  [[nodiscard]] ValueLength findDataOffset(uint8_t head) const noexcept;

  [[nodiscard]] ValueLength getNthOffset(ValueLength index) const;

  [[nodiscard]] SharedSlice makeKey() const;

  [[nodiscard]] int compareString(StringRef const& value) const;

  [[nodiscard]] int compareString(std::string const& value) const;

  [[nodiscard]] int compareString(char const* value, std::size_t length) const;

  [[nodiscard]] int compareStringUnchecked(StringRef const& value) const noexcept;

  [[nodiscard]] int compareStringUnchecked(std::string const& value) const noexcept;

  [[nodiscard]] int compareStringUnchecked(char const* value, std::size_t length) const noexcept;

  [[nodiscard]] bool isEqualString(StringRef const& attribute) const;

  [[nodiscard]] bool isEqualString(std::string const& attribute) const;

  [[nodiscard]] bool isEqualStringUnchecked(StringRef const& attribute) const noexcept;

  [[nodiscard]] bool isEqualStringUnchecked(std::string const& attribute) const noexcept;

  [[nodiscard]] bool binaryEquals(Slice const& other) const;
  [[nodiscard]] bool binaryEquals(SharedSlice const& other) const;

  bool operator==(SharedSlice const& other) const = delete;
  bool operator!=(SharedSlice const& other) const = delete;

  [[nodiscard]] std::string toHex() const;
  [[nodiscard]] std::string toJson(Options const* options = &Options::Defaults) const;
  [[nodiscard]] std::string toString(Options const* options = &Options::Defaults) const;
  [[nodiscard]] std::string hexType() const;

  [[nodiscard]] int64_t getIntUnchecked() const noexcept;

  [[nodiscard]] uint64_t getUIntUnchecked() const noexcept;

  [[nodiscard]] int64_t getSmallIntUnchecked() const noexcept;

  [[nodiscard]] std::shared_ptr<uint8_t const> getBCD(int8_t& sign, int32_t& exponent,
                                        ValueLength& mantissaLength) const;

 private:
  [[nodiscard]] SharedSlice alias(Slice slice) const noexcept;

  template <typename T>
  [[nodiscard]] std::shared_ptr<T> aliasPtr(T* t) const noexcept {
    return std::shared_ptr<T>(_start, t);
  }

 private:
  std::shared_ptr<uint8_t const> _start;
};

}  // namespace arangodb::velocypack

#endif  // SRC_SHAREDSLICE_H
