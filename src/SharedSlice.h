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

/*
 * TODO The main design question is: what's the underlying type? e.g.
 * std::shared_ptr<uint8_t const> or
 * std::shared_ptr<Buffer<uint8_t>> or
 * std::shared_ptr<Slice>
 * or something else?
 */

class SharedSlice {
 public:
  explicit SharedSlice(std::shared_ptr<uint8_t const>&& slice);
  explicit SharedSlice(std::shared_ptr<uint8_t const> const& slice);
  explicit SharedSlice(std::shared_ptr<Buffer<uint8_t const> const>&& buffer);
  explicit SharedSlice(std::shared_ptr<Buffer<uint8_t const> const> const& buffer);

  [[nodiscard]] Slice slice() const noexcept;

 private:
  std::shared_ptr<uint8_t const> _start;
};

}  // namespace arangodb::velocypack

#endif  // SRC_SHAREDSLICE_H
