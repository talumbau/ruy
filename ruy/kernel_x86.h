/* Copyright 2019 Google LLC. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef RUY_RUY_KERNEL_X86_H_
#define RUY_RUY_KERNEL_X86_H_

#include <cstdint>

#include "ruy/common.h"
#include "ruy/internal_matrix.h"
#include "ruy/kernel_common.h"
#include "ruy/matrix.h"
#include "ruy/opt_set.h"
#include "ruy/path.h"
#include "ruy/platform.h"
#include "ruy/spec.h"
#include "ruy/tune.h"

namespace ruy {

#if RUY_PLATFORM(X86)
// TODO(b/147376783): SSE 4.2 and AVX-VNNI support is incomplete / placeholder.
// Optimization is not finished. In particular the dimensions of the kernel
// blocks can be changed as desired.
//
void Kernel8bitSse42(const KernelParams8bit<8, 8>& params);

template <typename DstScalar>
struct Kernel<Path::kSse42, std::int8_t, std::int8_t, DstScalar,
              BasicSpec<std::int32_t, DstScalar>> {
  Tuning tuning = Tuning::kAuto;
  using LhsLayout = FixedKernelLayout<Order::kColMajor, 4, 8>;
  using RhsLayout = FixedKernelLayout<Order::kColMajor, 4, 8>;
  explicit Kernel(Tuning tuning_) : tuning(tuning_) {}
  void Run(const PackedMatrix<std::int8_t>& lhs,
           const PackedMatrix<std::int8_t>& rhs,
           const BasicSpec<std::int32_t, DstScalar>& spec, int start_row,
           int start_col, int end_row, int end_col,
           Matrix<DstScalar>* dst) const {
    KernelParams8bit<LhsLayout::kCols, RhsLayout::kCols> params;
    MakeKernelParams8bit(lhs, rhs, spec, start_row, start_col, end_row, end_col,
                         dst, &params);
    Kernel8bitSse42(params);
  }
};

void KernelFloatSse42(const KernelParamsFloat<8, 8>& params);

template <>
struct Kernel<Path::kSse42, float, float, float, BasicSpec<float, float>> {
  Tuning tuning = Tuning::kAuto;
  using LhsLayout = FixedKernelLayout<Order::kRowMajor, 1, 8>;
  using RhsLayout = FixedKernelLayout<Order::kRowMajor, 1, 8>;
  explicit Kernel(Tuning tuning_) : tuning(tuning_) {}
  void Run(const PackedMatrix<float>& lhs, const PackedMatrix<float>& rhs,
           const BasicSpec<float, float>& spec, int start_row, int start_col,
           int end_row, int end_col, Matrix<float>* dst) const {
    KernelParamsFloat<LhsLayout::kCols, RhsLayout::kCols> params;
    MakeKernelParamsFloat(lhs, rhs, spec, start_row, start_col, end_row,
                          end_col, dst, &params);
    KernelFloatSse42(params);
  }
};

void Kernel8bitAvx512(const KernelParams8bit<16, 16>& params);
void Kernel8bitAvx512SingleCol(const KernelParams8bit<16, 16>& params);

template <typename DstScalar>
struct Kernel<Path::kAvx512, std::int8_t, std::int8_t, DstScalar,
              BasicSpec<std::int32_t, DstScalar>> {
  Tuning tuning = Tuning::kAuto;
  using LhsLayout = FixedKernelLayout<Order::kColMajor, 4, 16>;
  using RhsLayout = FixedKernelLayout<Order::kColMajor, 4, 16>;
  explicit Kernel(Tuning tuning_) : tuning(tuning_) {}
  void Run(const PackedMatrix<std::int8_t>& lhs,
           const PackedMatrix<std::int8_t>& rhs,
           const BasicSpec<std::int32_t, DstScalar>& spec, int start_row,
           int start_col, int end_row, int end_col,
           Matrix<DstScalar>* dst) const {
    KernelParams8bit<LhsLayout::kCols, RhsLayout::kCols> params;
    MakeKernelParams8bit(lhs, rhs, spec, start_row, start_col, end_row, end_col,
                         dst, &params);
    if (dst->layout.cols == 1) {
      Kernel8bitAvx512SingleCol(params);
    } else {
      Kernel8bitAvx512(params);
    }
  }
};

void KernelFloatAvx512(const KernelParamsFloat<16, 16>& params);
void KernelFloatAvx512SingleCol(const KernelParamsFloat<16, 16>& param);

template <>
struct Kernel<Path::kAvx512, float, float, float, BasicSpec<float, float>> {
  Tuning tuning = Tuning::kAuto;
  using LhsLayout = FixedKernelLayout<Order::kRowMajor, 1, 16>;
  using RhsLayout = FixedKernelLayout<Order::kRowMajor, 1, 16>;
  explicit Kernel(Tuning tuning_) : tuning(tuning_) {}
  void Run(const PackedMatrix<float>& lhs, const PackedMatrix<float>& rhs,
           const BasicSpec<float, float>& spec, int start_row, int start_col,
           int end_row, int end_col, Matrix<float>* dst) const {
    KernelParamsFloat<LhsLayout::kCols, RhsLayout::kCols> params;
    MakeKernelParamsFloat(lhs, rhs, spec, start_row, start_col, end_row,
                          end_col, dst, &params);
    if (dst->layout.cols == 1) {
      KernelFloatAvx512SingleCol(params);
    } else {
      KernelFloatAvx512(params);
    }
  }
};

void Kernel8bitAvx2(const KernelParams8bit<8, 8>& params);
void Kernel8bitAvx2SingleCol(const KernelParams8bit<8, 8>& params);

template <typename DstScalar>
struct Kernel<Path::kAvx2, std::int8_t, std::int8_t, DstScalar,
              BasicSpec<std::int32_t, DstScalar>> {
  Tuning tuning = Tuning::kAuto;
  using LhsLayout = FixedKernelLayout<Order::kColMajor, 4, 8>;
  using RhsLayout = FixedKernelLayout<Order::kColMajor, 4, 8>;
  explicit Kernel(Tuning tuning_) : tuning(tuning_) {}
  void Run(const PackedMatrix<std::int8_t>& lhs,
           const PackedMatrix<std::int8_t>& rhs,
           const BasicSpec<std::int32_t, DstScalar>& spec, int start_row,
           int start_col, int end_row, int end_col,
           Matrix<DstScalar>* dst) const {
    KernelParams8bit<LhsLayout::kCols, RhsLayout::kCols> params;
    MakeKernelParams8bit(lhs, rhs, spec, start_row, start_col, end_row, end_col,
                         dst, &params);
    if (dst->layout.cols == 1) {
      Kernel8bitAvx2SingleCol(params);
    } else {
      Kernel8bitAvx2(params);
    }
  }
};

void KernelFloatAvx2(const KernelParamsFloat<8, 8>& params);
void KernelFloatAvx2SingleCol(const KernelParamsFloat<8, 8>& params);

template <>
struct Kernel<Path::kAvx2, float, float, float, BasicSpec<float, float>> {
  Tuning tuning = Tuning::kAuto;
  using LhsLayout = FixedKernelLayout<Order::kRowMajor, 1, 8>;
  using RhsLayout = FixedKernelLayout<Order::kRowMajor, 1, 8>;
  explicit Kernel(Tuning tuning_) : tuning(tuning_) {}
  void Run(const PackedMatrix<float>& lhs, const PackedMatrix<float>& rhs,
           const BasicSpec<float, float>& spec, int start_row, int start_col,
           int end_row, int end_col, Matrix<float>* dst) const {
    KernelParamsFloat<LhsLayout::kCols, RhsLayout::kCols> params;
    MakeKernelParamsFloat(lhs, rhs, spec, start_row, start_col, end_row,
                          end_col, dst, &params);
    if (dst->layout.cols == 1) {
      KernelFloatAvx2SingleCol(params);
    } else {
      KernelFloatAvx2(params);
    }
  }
};

// TODO(b/147376783): SSE 4.2 and AVX-VNNI support is incomplete / placeholder.
// Optimization is not finished. In particular the dimensions of the kernel
// blocks can be changed as desired.
//
void Kernel8bitAvxVnni(const KernelParams8bit<16, 16>& params);

template <typename DstScalar>
struct Kernel<Path::kAvxVnni, std::int8_t, std::int8_t, DstScalar,
              BasicSpec<std::int32_t, DstScalar>> {
  Tuning tuning = Tuning::kAuto;
  using LhsLayout = FixedKernelLayout<Order::kColMajor, 4, 16>;
  using RhsLayout = FixedKernelLayout<Order::kColMajor, 4, 16>;
  explicit Kernel(Tuning tuning_) : tuning(tuning_) {}
  void Run(const PackedMatrix<std::int8_t>& lhs,
           const PackedMatrix<std::int8_t>& rhs,
           const BasicSpec<std::int32_t, DstScalar>& spec, int start_row,
           int start_col, int end_row, int end_col,
           Matrix<DstScalar>* dst) const {
    KernelParams8bit<LhsLayout::kCols, RhsLayout::kCols> params;
    MakeKernelParams8bit(lhs, rhs, spec, start_row, start_col, end_row, end_col,
                         dst, &params);
    Kernel8bitAvxVnni(params);
  }
};

void KernelFloatAvxVnni(const KernelParamsFloat<16, 16>& params);

template <>
struct Kernel<Path::kAvxVnni, float, float, float, BasicSpec<float, float>> {
  Tuning tuning = Tuning::kAuto;
  using LhsLayout = FixedKernelLayout<Order::kRowMajor, 1, 16>;
  using RhsLayout = FixedKernelLayout<Order::kRowMajor, 1, 16>;
  explicit Kernel(Tuning tuning_) : tuning(tuning_) {}
  void Run(const PackedMatrix<float>& lhs, const PackedMatrix<float>& rhs,
           const BasicSpec<float, float>& spec, int start_row, int start_col,
           int end_row, int end_col, Matrix<float>* dst) const {
    KernelParamsFloat<LhsLayout::kCols, RhsLayout::kCols> params;
    MakeKernelParamsFloat(lhs, rhs, spec, start_row, start_col, end_row,
                          end_col, dst, &params);
    KernelFloatAvxVnni(params);
  }
};

#endif  // RUY_PLATFORM(X86)

}  // namespace ruy

#endif  // RUY_RUY_KERNEL_X86_H_
