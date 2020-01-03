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

#include "SharedSlice.h"

using namespace arangodb;
using namespace arangodb::velocypack;

Slice SharedSlice::slice() const noexcept { return Slice(_start.get()); }

SharedSlice::SharedSlice(std::shared_ptr<uint8_t const>&& buffer)
    : _start(std::move(buffer)) {}

SharedSlice::SharedSlice(std::shared_ptr<uint8_t const> const& buffer)
    : _start(buffer) {}

SharedSlice::SharedSlice(std::shared_ptr<Buffer<uint8_t const> const>&& buffer)
    : _start(std::move(buffer), buffer->data()) {}

SharedSlice::SharedSlice(std::shared_ptr<Buffer<uint8_t const> const> const& buffer)
    : _start(buffer, buffer->data())  {}
