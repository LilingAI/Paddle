/* Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#include <gtest/gtest.h>
#include "gflags/gflags.h"
#include "paddle/fluid/inference/tests/test_helper.h"

DEFINE_string(dirname, "", "Directory of the inference model.");

TEST(inference, rnn_encoder_decoder) {
  if (FLAGS_dirname.empty()) {
    LOG(FATAL) << "Usage: ./example --dirname=path/to/your/model";
  }

  LOG(INFO) << "FLAGS_dirname: " << FLAGS_dirname << std::endl;
  std::string dirname = FLAGS_dirname;

  // 0. Call `paddle::framework::InitDevices()` initialize all the devices
  // In unittests, this is done in paddle/testing/paddle_gtest_main.cc

  paddle::framework::LoDTensor word_data, trg_word;
  paddle::framework::LoD lod{{0, 4, 10}};

  SetupLoDTensor(
      word_data, lod, static_cast<int64_t>(0), static_cast<int64_t>(1));
  SetupLoDTensor(
      trg_word, lod, static_cast<int64_t>(0), static_cast<int64_t>(1));

  std::vector<paddle::framework::LoDTensor*> cpu_feeds;
  cpu_feeds.push_back(&word_data);
  cpu_feeds.push_back(&trg_word);

  paddle::framework::LoDTensor output1;
  std::vector<paddle::framework::LoDTensor*> cpu_fetchs1;
  cpu_fetchs1.push_back(&output1);

  // Run inference on CPU
  TestInference<paddle::platform::CPUPlace>(dirname, cpu_feeds, cpu_fetchs1);
  LOG(INFO) << output1.lod();
  LOG(INFO) << output1.dims();

#ifdef PADDLE_WITH_CUDA
  paddle::framework::LoDTensor output2;
  std::vector<paddle::framework::LoDTensor*> cpu_fetchs2;
  cpu_fetchs2.push_back(&output2);

  // Run inference on CUDA GPU
  TestInference<paddle::platform::CUDAPlace>(dirname, cpu_feeds, cpu_fetchs2);
  LOG(INFO) << output2.lod();
  LOG(INFO) << output2.dims();

  CheckError<float>(output1, output2);
#endif
}
