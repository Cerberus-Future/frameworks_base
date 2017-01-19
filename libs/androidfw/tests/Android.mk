#
# Copyright (C) 2014 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# ==========================================================
# Setup some common variables for the different build
# targets here.
# ==========================================================
LOCAL_PATH:= $(call my-dir)

testFiles := \
    ApkAssets_test.cpp \
    AppAsLib_test.cpp \
    Asset_test.cpp \
    AssetManager2_test.cpp \
    AttributeFinder_test.cpp \
    AttributeResolution_test.cpp \
    ByteBucketArray_test.cpp \
    Config_test.cpp \
    ConfigLocale_test.cpp \
    Idmap_test.cpp \
    LoadedArsc_test.cpp \
    ResTable_test.cpp \
    Split_test.cpp \
    StringPiece_test.cpp \
    TestHelpers.cpp \
    TestMain.cpp \
    Theme_test.cpp \
    TypeWrappers_test.cpp \
    ZipUtils_test.cpp

benchmarkFiles := \
    AssetManager2_bench.cpp \
    BenchMain.cpp \
    TestHelpers.cpp \
    Theme_bench.cpp

androidfw_test_cflags := \
    -Wall \
    -Werror \
    -Wunused \
    -Wunreachable-code \
    -Wno-missing-field-initializers

# gtest is broken.
androidfw_test_cflags += -Wno-unnamed-type-template-args

# ==========================================================
# Build the host tests: libandroidfw_tests
# ==========================================================
include $(CLEAR_VARS)

LOCAL_MODULE := libandroidfw_tests
LOCAL_CFLAGS := $(androidfw_test_cflags)
LOCAL_SRC_FILES := $(testFiles)
LOCAL_STATIC_LIBRARIES := \
    libandroidfw \
    libbase \
    libutils \
    libcutils \
    liblog \
    libz \
    libziparchive
LOCAL_PICKUP_FILES := $(LOCAL_PATH)/data

include $(BUILD_HOST_NATIVE_TEST)

# ==========================================================
# Build the device tests: libandroidfw_tests
# ==========================================================
ifneq ($(SDK_ONLY),true)
include $(CLEAR_VARS)

LOCAL_MODULE := libandroidfw_tests
LOCAL_CFLAGS := $(androidfw_test_cflags)
LOCAL_SRC_FILES := $(testFiles) \
    BackupData_test.cpp \
    ObbFile_test.cpp \

LOCAL_SHARED_LIBRARIES := \
    libandroidfw \
    libbase \
    libcutils \
    libutils \
    libui \
    libziparchive 
LOCAL_PICKUP_FILES := $(LOCAL_PATH)/data

include $(BUILD_NATIVE_TEST)

# ==========================================================
# Build the device benchmarks: libandroidfw_benchmarks
# ==========================================================
include $(CLEAR_VARS)

LOCAL_MODULE := libandroidfw_benchmarks
LOCAL_CFLAGS := $(androidfw_test_cflags)
LOCAL_SRC_FILES := $(benchmarkFiles)
LOCAL_STATIC_LIBRARIES := \
    libgoogle-benchmark
LOCAL_SHARED_LIBRARIES := \
    libandroidfw \
    libbase \
    libcutils \
    libutils \
    libziparchive
LOCAL_PICKUP_FILES := $(LOCAL_PATH)/data

include $(BUILD_NATIVE_TEST)
endif # Not SDK_ONLY

