// Copyright (c) 2017, Guillaume Fraux
// All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
// SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
// OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
// IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
// OF SUCH DAMAGE.

#ifndef PACIF_VALUE_HPP
#define PACIF_VALUE_HPP

#include "types.hpp"

namespace pacif {

/// Possible values in CIF data block.
///
/// A `pacif::value` can be a floating point number, a string or a vector of
/// `pacif::value`. It is represented as a tagged union.
class value final {
public:
    /// Available kinds of value
    enum class kind {
        /// A missing value
        missing,
        /// A string value
        string,
        /// A real value, represented as a floating point number
        real,
        /// A vector of `pacif::value`
        vec,
    };

    /// Create a missing value
    value(): kind_(kind::missing) {}
    /// Create a string value containing `string`
    explicit value(string_t string): kind_(kind::string), string_(std::move(string)) {}
    /// Create a real value containing `real`
    explicit value(real_t real): kind_(kind::real), real_(real) {}
    /// Create a vector value containing `vec`
    explicit value(vec_t vec): kind_(kind::vec), vec_(std::move(vec)) {}

    /// The copy constructor for values
    value(const value& other): value() {
        *this = other;
    }

    /// The copy assignement operator for values
    value& operator=(const value& other) {
        this->~value();
        this->kind_ = other.kind_;
        switch (this->kind_) {
        case kind::missing:
            break; // nothing to do
        case kind::string:
            new (&this->string_) string_t(other.string_);
            break;
        case kind::vec:
            new (&this->vec_) vec_t(other.vec_);
            break;
        case kind::real:
            new (&this->real_) real_t(other.real_);
            break;
        }
        return *this;
    }

    /// The move constructor for values
    value(value&& other): value() {
        *this = std::move(other);
    }

    /// The move assignement operator for values
    value& operator=(value&& other) {
        this->~value();
        this->kind_ = other.kind_;
        switch (this->kind_) {
        case kind::missing:
            break; // nothing to do
        case kind::string:
            new (&this->string_) string_t(std::move(other.string_));
            break;
        case kind::vec:
            new (&this->vec_) vec_t(std::move(other.vec_));
            break;
        case kind::real:
            new (&this->real_) real_t(std::move(other.real_));
            break;
        }
        return *this;
    }

    /// The destructor for values
    ~value() {
        switch (this->kind_) {
        case kind::string:
            this->string_.~string_t();
            break;
        case kind::vec:
            this->vec_.~vec_t();
            break;
        case kind::real:
            break; // nothing to do
        case kind::missing:
            break; // nothing to do
        }
    }

    /// Check if this value is a missing value
    bool is_missing() const {
        return this->kind_ == kind::missing;
    }

    /// Check if this value is a string
    bool is_string() const {
        return this->kind_ == kind::string;
    }

    /// Check if this value is a vector
    bool is_vec() const {
        return this->kind_ == kind::vec;
    }

    /// Check if this value is a real
    bool is_real() const {
        return this->kind_ == kind::real;
    }

    /// Get the kind of this value
    kind get_kind() const {
        return this->kind_;
    }

    /// Get this value as a string
    ///
    /// @throw if the value is not a string
    const string_t& as_string() const {
        if (this->kind_ == kind::string) {
            return this->string_;
        } else {
            throw error("called value::as_string, but this is not a string value");
        }
    }

    /// Get this value as a real
    ///
    /// @throw if the value is not a real
    real_t as_real() const {
        if (this->kind_ == kind::real) {
            return this->real_;
        } else {
            throw error("called value::as_real, but this is not a real value");
        }
    }

    /// Get this value as a vector
    ///
    /// @throw if the value is not a vector
    const vec_t& as_vec() const {
        if (this->kind_ == kind::vec) {
            return this->vec_;
        } else {
            throw error("called value::as_vec, but this is not a vector value");
        }
    }

private:
    /// Kind of the stored value
    kind kind_;
    /// Value data storage, as an union
    union {
        string_t string_;
        real_t real_;
        vec_t vec_;
    };
};

}

#endif
