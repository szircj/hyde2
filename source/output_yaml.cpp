

// identity
#include "output_yaml.hpp"

// stdc++
#include <iostream>

// yaml-cpp
#include "yaml-cpp/yaml.h"

// application
#include "emitters/yaml_class_emitter.hpp"
#include "emitters/yaml_enum_emitter.hpp"
#include "emitters/yaml_function_emitter.hpp"
#include "emitters/yaml_library_emitter.hpp"
#include "emitters/yaml_sourcefile_emitter.hpp"
#include "json.hpp"

/**************************************************************************************************/

namespace hyde2 {

/**************************************************************************************************/

void output_yaml(json j,
                 const std::filesystem::path& src_root,
                 const std::filesystem::path& dst_root,
                 json& out_emitted,
                 yaml_mode mode,
                 const emit_options& options) {
    bool failure{false};
    auto& library_emitted = out_emitted;
    const json no_inheritance_k;

    // Process top-level library
    yaml_library_emitter(src_root, dst_root, mode, options).emit(j, library_emitted, no_inheritance_k);

    // Process sourcefile
    yaml_sourcefile_emitter sourcefile_emitter(src_root, dst_root, mode, options);
    auto sourcefile_emitted = hyde::json::object();
    failure |= sourcefile_emitter.emit(j, sourcefile_emitted, no_inheritance_k);

    // Process classes
    yaml_class_emitter class_emitter(src_root, dst_root, mode, options);
    for (const auto& c : j["classes"]) {
        auto class_emitted = hyde::json::object();
        failure |= class_emitter.emit(c, class_emitted, no_inheritance_k);
        sourcefile_emitted["classes"].push_back(std::move(class_emitted));
    }

    // Process enums
    yaml_enum_emitter enum_emitter(src_root, dst_root, mode, options);
    for (const auto& c : j["enums"]) {
        auto enum_emitted = hyde::json::object();
        failure |= enum_emitter.emit(c, enum_emitted, no_inheritance_k);
        sourcefile_emitted["enums"].push_back(std::move(enum_emitted));
    }

    // Process functions
    yaml_function_emitter function_emitter(src_root, dst_root, mode, options, false);
    const auto& functions = j["functions"];
    for (auto it = functions.begin(); it != functions.end(); ++it) {
        function_emitter.set_key(it.key());
        auto function_emitted = hyde::json::object();
        failure |= function_emitter.emit(it.value(), function_emitted, no_inheritance_k);
        sourcefile_emitted["functions"].push_back(std::move(function_emitted));
    }

    library_emitted["sourcefiles"].push_back(std::move(sourcefile_emitted));

    // Check for extra files. Always do this last.
    if (!options._ignore_extraneous_files) {
        failure |= sourcefile_emitter.extraneous_file_check();
    }

    if (failure && mode == yaml_mode::validate)
        throw std::runtime_error("YAML documentation failed to validate.");
}

/**************************************************************************************************/

} // namespace hyde

/**************************************************************************************************/
