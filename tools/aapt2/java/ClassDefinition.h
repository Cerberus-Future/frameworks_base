/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AAPT_JAVA_CLASSDEFINITION_H
#define AAPT_JAVA_CLASSDEFINITION_H

#include <ostream>
#include <string>

#include "android-base/macros.h"
#include "androidfw/StringPiece.h"

#include "Resource.h"
#include "java/AnnotationProcessor.h"
#include "util/Util.h"

namespace aapt {

// The number of attributes to emit per line in a Styleable array.
constexpr static size_t kAttribsPerLine = 4;
constexpr static const char* kIndent = "  ";

class ClassMember {
 public:
  virtual ~ClassMember() = default;

  AnnotationProcessor* GetCommentBuilder() { return &processor_; }

  virtual bool empty() const = 0;

  virtual void WriteToStream(const android::StringPiece& prefix, bool final,
                             std::ostream* out) const {
    processor_.WriteToStream(out, prefix);
  }

 private:
  AnnotationProcessor processor_;
};

template <typename T>
class PrimitiveMember : public ClassMember {
 public:
  PrimitiveMember(const android::StringPiece& name, const T& val)
      : name_(name.to_string()), val_(val) {}

  bool empty() const override { return false; }

  void WriteToStream(const android::StringPiece& prefix, bool final,
                     std::ostream* out) const override {
    ClassMember::WriteToStream(prefix, final, out);

    *out << prefix << "public static " << (final ? "final " : "") << "int "
         << name_ << "=" << val_ << ";";
  }

 private:
  std::string name_;
  T val_;

  DISALLOW_COPY_AND_ASSIGN(PrimitiveMember);
};

/**
 * Specialization for strings so they get the right type and are quoted with "".
 */
template <>
class PrimitiveMember<std::string> : public ClassMember {
 public:
  PrimitiveMember(const android::StringPiece& name, const std::string& val)
      : name_(name.to_string()), val_(val) {}

  bool empty() const override { return false; }

  void WriteToStream(const android::StringPiece& prefix, bool final,
                     std::ostream* out) const override {
    ClassMember::WriteToStream(prefix, final, out);

    *out << prefix << "public static " << (final ? "final " : "") << "String "
         << name_ << "=\"" << val_ << "\";";
  }

 private:
  std::string name_;
  std::string val_;

  DISALLOW_COPY_AND_ASSIGN(PrimitiveMember);
};

using IntMember = PrimitiveMember<uint32_t>;
using ResourceMember = PrimitiveMember<ResourceId>;
using StringMember = PrimitiveMember<std::string>;

template <typename T>
class PrimitiveArrayMember : public ClassMember {
 public:
  explicit PrimitiveArrayMember(const android::StringPiece& name) : name_(name.to_string()) {}

  void AddElement(const T& val) { elements_.push_back(val); }

  bool empty() const override { return false; }

  void WriteToStream(const android::StringPiece& prefix, bool final,
                     std::ostream* out) const override {
    ClassMember::WriteToStream(prefix, final, out);

    *out << prefix << "public static final int[] " << name_ << "={";

    const auto begin = elements_.begin();
    const auto end = elements_.end();
    for (auto current = begin; current != end; ++current) {
      if (std::distance(begin, current) % kAttribsPerLine == 0) {
        *out << "\n" << prefix << kIndent << kIndent;
      }

      *out << *current;
      if (std::distance(current, end) > 1) {
        *out << ", ";
      }
    }
    *out << "\n" << prefix << kIndent << "};";
  }

 private:
  std::string name_;
  std::vector<T> elements_;

  DISALLOW_COPY_AND_ASSIGN(PrimitiveArrayMember);
};

using ResourceArrayMember = PrimitiveArrayMember<ResourceId>;

enum class ClassQualifier { None, Static };

class ClassDefinition : public ClassMember {
 public:
  static bool WriteJavaFile(const ClassDefinition* def, const android::StringPiece& package,
                            bool final, std::ostream* out);

  ClassDefinition(const android::StringPiece& name, ClassQualifier qualifier, bool createIfEmpty)
      : name_(name.to_string()), qualifier_(qualifier), create_if_empty_(createIfEmpty) {}

  void AddMember(std::unique_ptr<ClassMember> member) {
    members_.push_back(std::move(member));
  }

  bool empty() const override;
  void WriteToStream(const android::StringPiece& prefix, bool final,
                     std::ostream* out) const override;

 private:
  std::string name_;
  ClassQualifier qualifier_;
  bool create_if_empty_;
  std::vector<std::unique_ptr<ClassMember>> members_;

  DISALLOW_COPY_AND_ASSIGN(ClassDefinition);
};

}  // namespace aapt

#endif /* AAPT_JAVA_CLASSDEFINITION_H */
