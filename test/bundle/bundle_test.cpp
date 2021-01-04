#define CATCH_CONFIG_MAIN
#include "../../deps/catch2/catch.hpp"

#include <algorithm>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>

#include "../../src/BidirectedGraph.hpp"
#include "../../src/algorithms/find_balanced_bundles.hpp"
#include "../../src/algorithms/bundle.hpp"
#include "../../deps/handlegraph/handle_graph.hpp"
#include "../../deps/json/json/json.h"

#define JSON_TEST_GRAPHS_DIR "graphs"

using namespace std;
namespace fs = filesystem;

/// At this point a bundle comparison operator has not been implemented
typedef unordered_set<handle_t> bundleside;
typedef pair<bundleside, bundleside> bundle;
typedef pair<bundle, bundle> bundle_pair;
vector<bundle_pair> get_true_bundles(const string& path, const BidirectedGraph& g);
int check_bundle_match(vector<Bundle*>& found_bundles, const vector<bundle_pair>& true_bundles,
    const BidirectedGraph& g);

TEST_CASE("JSON Bundle Tests") {
    /// Get test cases
    vector<string> test_cases;
    for (auto& entry : fs::directory_iterator(JSON_TEST_GRAPHS_DIR))
        if (entry.path().extension() == ".json")
            test_cases.push_back(entry.path());
    sort(test_cases.begin(), test_cases.end());

    /// Test cases
    for (auto& case_path : test_cases) {
        SECTION(case_path) {
            ifstream json_file(case_path, ifstream::binary);
            BidirectedGraph g;
            REQUIRE(g.deserialize(json_file));
            auto found_bundles = find_balanced_bundles(g);
            auto true_bundles = get_true_bundles(case_path, g);

            /// Check bundles count matches
            REQUIRE(found_bundles.size() == true_bundles.size());
            
            /// Check bundles match
            REQUIRE(check_bundle_match(found_bundles, true_bundles, g) == 0);
        }
    }
}

vector<bundle_pair> get_true_bundles(const string& path, const BidirectedGraph& g) {
    /// Mostly assumes properly formatted file
    ifstream graph_file(path, ifstream::binary);
    Json::CharReaderBuilder reader;
    Json::Value graph_json;
    std::string errs;
    REQUIRE(Json::parseFromStream(reader, graph_file, &graph_json, &errs));
    graph_file.close();

    vector<bundle_pair> bundles;
    Json::Value bundles_json = graph_json["bundle"];
    for (auto& bundle_json : bundles_json) {
        /// Construct left side
        bundleside left, left_flipped;
        for (auto& left_node : bundle_json["left"]) {
            int64_t id = left_node["id"].asInt64();
            bool reversed = left_node["is_reverse"].asBool();
            left.insert(g.get_handle(id, reversed));
            left_flipped.insert(g.get_handle(id, !reversed));
        }

        /// Construct right side
        bundleside right, right_flipped;
        for (auto& right_node : bundle_json["right"]) {
            int64_t id = right_node["id"].asInt64();
            bool reversed = right_node["is_reverse"].asBool();
            right.insert(g.get_handle(id, reversed));
            right_flipped.insert(g.get_handle(id, !reversed));
        }

        bundles.push_back(pair(pair(left, right), pair(left_flipped, right_flipped)));
    }

    return bundles;
}

bundle_pair bundle_to_bundle_pair(Bundle& bundle, const BidirectedGraph& g) {
    bundleside left, left_flipped;
    for (const auto& handle : bundle.get_left()) {
        left.insert(handle);
        left_flipped.insert(g.flip(handle));
    }

    bundleside right, right_flipped;
    for (const auto& handle : bundle.get_right()) {
        right.insert(handle);
        right_flipped.insert(g.flip(handle));
    }

    return pair(pair(left, right), pair(left_flipped, right_flipped));
}

bool is_match(bundle_pair& b1, bundle_pair& b2) {
    bundle b1_norm = b1.first;
    bundle b1_flip = b1.second;
    bundle b2_norm = b2.first;
    bundle b2_flip = b2.second;
    return (b1_norm.first == b2_norm.first && b1_norm.second == b2_norm.second) ||
        (b1_norm.first == b2_norm.second && b1_norm.second == b2_norm.first) ||
        (b1_norm.first == b2_flip.first && b1_norm.second == b2_flip.second) ||
        (b1_norm.first == b2_flip.second && b1_norm.second == b2_flip.first) ||
        (b1_flip.first == b2_norm.first && b1_flip.second == b2_norm.second) ||
        (b1_flip.first == b2_norm.second && b1_flip.second == b2_norm.first) ||
        (b1_flip.first == b2_flip.first && b1_flip.second == b2_flip.second) ||
        (b1_flip.first == b2_flip.second && b1_flip.second == b2_flip.first);
}

int check_bundle_match(vector<Bundle*>& found_bundles, const vector<bundle_pair>& true_bundles,
    const BidirectedGraph& g
) {
    auto true_bp = true_bundles;
    /// If a matching true bundle is found, remove it
    for (auto& bundle : found_bundles) {
        bundle_pair found = bundle_to_bundle_pair(*bundle, g); 
        for (auto it = true_bp.begin(); it < true_bp.end(); it++) {
            if (is_match(found, *it)) {
                true_bp.erase(it);
                break;
            }
        }
    }

    return true_bp.size();
}