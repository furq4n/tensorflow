/* Copyright 2016 The TensorFlow Authors All Rights Reserved.

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

// Build a tree structure based on the TensorFlow model's python code stacks.
// Stats are aggregated from descendants from ancestors.

#ifndef THIRD_PARTY_TENSORFLOW_CORE_PROFILER_INTERNAL_TFPROF_CODE_H_
#define THIRD_PARTY_TENSORFLOW_CORE_PROFILER_INTERNAL_TFPROF_CODE_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "tensorflow/c/checkpoint_reader.h"
#include "tensorflow/core/framework/graph.pb.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/profiler/internal/tfprof_node.h"
#include "tensorflow/core/profiler/internal/tfprof_options.h"
#include "tensorflow/core/profiler/internal/tfprof_show_multi.h"
#include "tensorflow/core/profiler/internal/tfprof_timeline.h"
#include "tensorflow/core/profiler/internal/tfprof_utils.h"
#include "tensorflow/core/profiler/profile.pb.h"
#include "tensorflow/core/profiler/tfprof_log.pb.h"
#include "tensorflow/core/profiler/tfprof_output.pb.h"

namespace tensorflow {
namespace tfprof {

class PprofProfile {
 public:
  virtual ~PprofProfile() {}

  virtual uint64 AddLocation(const CodeNode* callee,
                             const CodeNode* caller) = 0;

  virtual void AddSample(const CodeNode* leaf,
                         std::vector<uint64>* call_ids) = 0;

  virtual Status WritePprofProfile(const string& filename) = 0;
};

class TFCode : public TFMultiShow {
 public:
  TFCode() {}
  ~TFCode() override {}

  void AddNode(TFGraphNode* node) override;

  void Build() override;

 private:
  const ShowMultiNode* ShowInternal(const Options& opts,
                                    Timeline* timeline) override;

  std::vector<CodeNode*> SearchRoot(std::vector<CodeNode*> roots,
                                    const std::vector<string>& regexes);

  std::vector<CodeNode*> PrintScope(const std::vector<CodeNode*> roots,
                                    const Options& opts, int depth,
                                    int last_ident);

  std::vector<CodeNode*> Account(const std::vector<CodeNode*>& roots,
                                 const Options& opts);

  void Format(const CodeNode* root, const std::vector<CodeNode*>& nodes,
              const Options& opts, string* display_str,
              MultiGraphNodeProto* proto, std::vector<uint64>* call_ids);

  string FormatNode(CodeNode* node, const Options& opts, int64 indent) const;
  string FormatNodeMemory(CodeNode* node, int64 bytes, int64 total_bytes) const;

  std::unique_ptr<CodeNode> root_;
  std::unique_ptr<TFMultiGraphNode> graph_root_;
  std::unique_ptr<PprofProfile> pprof_profile_;
};
}  // namespace tfprof
}  // namespace tensorflow

#endif  // THIRD_PARTY_TENSORFLOW_CORE_PROFILER_INTERNAL_TFPROF_CODE_H_
