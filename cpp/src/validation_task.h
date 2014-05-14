// Copyright (C) 2014 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef I18N_ADDRESSINPUT_VALIDATION_TASK_H_
#define I18N_ADDRESSINPUT_VALIDATION_TASK_H_

#include <libaddressinput/address_field.h>
#include <libaddressinput/address_problem.h>
#include <libaddressinput/address_validator.h>
#include <libaddressinput/util/basictypes.h>
#include <libaddressinput/util/scoped_ptr.h>

#include "metadata_loader.h"

namespace i18n {
namespace addressinput {

class LookupKey;
class Rule;
struct AddressData;

// A ValidationTask object encapsulates the information necessary to perform
// validation of one particular address and call a callback when that has been
// done. Calling the Run() method will load required metadata, then perform
// validation, call the callback and delete the ValidationTask object itself.
class ValidationTask {
 public:
  ValidationTask(const AddressData& address,
                 bool allow_postal,
                 bool require_name,
                 const FieldProblemMap* filter,
                 FieldProblemMap* problems,
                 const AddressValidator::Callback& validated);

  ~ValidationTask();

  // Calls metadata->Load(), with Validate() as callback.
  void Run(MetadataLoader* metadata) const;

 private:
  friend class ValidationTaskTest;

  // Uses the address metadata of |hierarchy| to validate |address_|, writing
  // problems found into |problems_|, then calls the |validated_| callback and
  // deletes this ValidationTask object.
  void Validate(bool success,
                const LookupKey& lookup_key,
                const MetadataLoader::RuleHierarchy& hierarchy);

  // Checks all fields for UNEXPECTED_FIELD problems.
  void CheckUnexpectedField(
      const MetadataLoader::RuleHierarchy& hierarchy) const;

  // Checks all fields for MISSING_REQUIRED_FIELD problems.
  void CheckMissingRequiredField(
      const MetadataLoader::RuleHierarchy& hierarchy) const;

  // Checks the hierarchical fields for UNKNOWN_VALUE problems.
  void CheckUnknownValue(
      const MetadataLoader::RuleHierarchy& hierarchy) const;

  // Checks the POSTAL_CODE field for problems.
  void CheckPostalCodeFormatAndValue(
      const MetadataLoader::RuleHierarchy& hierarchy) const;

  // Checks the STREET_ADDRESS field for USES_P_O_BOX problems.
  void CheckUsesPoBox(
      const MetadataLoader::RuleHierarchy& hierarchy) const;

  // Writes (|field|,|problem|) to |problems_|.
  void ReportProblem(AddressField field, AddressProblem problem) const;

  // Writes (|field|,|problem|) to |problems_|, if this pair should be reported.
  void ReportProblemMaybe(AddressField field, AddressProblem problem) const;

  // Returns whether (|field|,|problem|) should be reported.
  bool ShouldReport(AddressField field, AddressProblem problem) const;

  // Returns whether |field| is expected according to |rule|.
  static bool IsUnexpected(const Rule& rule, AddressField field);

  const AddressData& address_;
  const bool allow_postal_;
  const bool require_name_;
  const FieldProblemMap* filter_;
  FieldProblemMap* const problems_;
  const AddressValidator::Callback& validated_;
  const scoped_ptr<const MetadataLoader::Callback> loaded_;
  const scoped_ptr<LookupKey> lookup_key_;

  DISALLOW_COPY_AND_ASSIGN(ValidationTask);
};

}  // namespace addressinput
}  // namespace i18n

#endif  // I18N_ADDRESSINPUT_VALIDATION_TASK_H_