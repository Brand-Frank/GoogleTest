#!/bin/bash

set -euox pipefail

if [[ -z ${GTEST_ROOT:-} ]]; then
  GTEST_ROOT="$(realpath $(dirname ${0})/..)"
fi

# Test the CMake build
for cmake_off_on in OFF ON; do
  BUILD_DIR=$(mktemp -d build_dir.XXXXXXXX)
  cd ${BUILD_DIR}
  time cmake ${GTEST_ROOT} \
    -DCMAKE_CXX_STANDARD=14 \
    -Dgtest_build_samples=ON \
    -Dgtest_build_tests=ON \
    -Dgmock_build_tests=ON \
    -Dcxx_no_exception=${cmake_off_on} \
    -Dcxx_no_rtti=${cmake_off_on}
  time make
  time ctest -j$(nproc) --output-on-failure
done

# Test the Bazel build

# If we are running on Kokoro, check for a versioned Bazel binary.
KOKORO_GFILE_BAZEL_BIN="bazel-7.0.0-darwin-x86_64"
if [[ ${KOKORO_GFILE_DIR:-} ]] && [[ -f ${KOKORO_GFILE_DIR}/${KOKORO_GFILE_BAZEL_BIN} ]]; then
  BAZEL_BIN="${KOKORO_GFILE_DIR}/${KOKORO_GFILE_BAZEL_BIN}"
  chmod +x ${BAZEL_BIN}
else
  BAZEL_BIN="bazel"
fi

cd ${GTEST_ROOT}
for absl in 0 1; do
  ${BAZEL_BIN} test ... \
    --copt="-Wall" \
    --copt="-Werror" \
    --copt="-Wundef" \
    --cxxopt="-std=c++14" \
    --define="absl=${absl}" \
    --enable_bzlmod=true \
    --features=external_include_paths \
    --keep_going \
    --show_timestamps \
    --test_output=errors
done
