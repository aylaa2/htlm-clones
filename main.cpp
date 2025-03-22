#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include "gumbo.h"  

namespace fs = std::filesystem;

// Recursively extracts visible text from a Gumbo node
void extract_text_from_node(GumboNode* node, std::string& output) {
    if (node->type == GUMBO_NODE_TEXT) {
        output += node->v.text.text;
        output += " ";
    } else if (node->type == GUMBO_NODE_ELEMENT && node->v.element.tag != GUMBO_TAG_SCRIPT && node->v.element.tag != GUMBO_TAG_STYLE) {
        // Recursively extract text from children nodes
        GumboVector* children = &node->v.element.children;
        for (size_t i = 0; i < children->length; ++i) {
            extract_text_from_node(static_cast<GumboNode*>(children->data[i]), output);
        }
    }
}

// Extracts clean text from an HTML file
std::string extract_text_from_html(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << file_path << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string html_content = buffer.str();
    file.close();

    GumboOutput* output = gumbo_parse(html_content.c_str());
    std::string extracted_text;
    extract_text_from_node(output->root, extracted_text);
    gumbo_destroy_output(&kGumboDefaultOptions, output);

    // Trim and return cleaned text
    extracted_text.erase(std::remove(extracted_text.begin(), extracted_text.end(), '\n'), extracted_text.end());
    return extracted_text;
}

// Tokenize text and remove stopwords
std::vector<std::string> tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::istringstream iss(text);
    std::string word;
    while (iss >> word) {
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        tokens.push_back(word);
    }
    return tokens;
}

// Compute term frequency (TF)
std::unordered_map<std::string, double> compute_tf(const std::vector<std::string>& words) {
    std::unordered_map<std::string, double> tf;
    for (const std::string& word : words) {
        tf[word]++;
    }
    for (auto& [word, count] : tf) {
        count /= words.size();
    }
    return tf;
}

// Compute cosine similarity
double cosine_similarity(const std::unordered_map<std::string, double>& tf1,
                         const std::unordered_map<std::string, double>& tf2) {
    double dot_product = 0.0, norm1 = 0.0, norm2 = 0.0;
    for (const auto& [word, tf_value] : tf1) {
        if (tf2.count(word)) {
            dot_product += tf_value * tf2.at(word);
        }
        norm1 += tf_value * tf_value;
    }
    for (const auto& [word, tf_value] : tf2) {
        norm2 += tf_value * tf_value;
    }
    if (norm1 == 0.0 || norm2 == 0.0) return 0.0;
    return dot_product / (std::sqrt(norm1) * std::sqrt(norm2));
}

// Density-Based Spatial Clustering of Applications with Noise
std::vector<std::vector<std::string>> dbscan(std::map<std::string, std::unordered_map<std::string, double>>& tfidf_map, double eps) {
    std::vector<std::vector<std::string>> clusters;
    std::set<std::string> visited;

    for (const auto& [file, tfidf] : tfidf_map) {
        if (visited.count(file)) continue;
        visited.insert(file);
        std::vector<std::string> cluster = {file};
        for (const auto& [other_file, other_tfidf] : tfidf_map) {
            if (file != other_file && cosine_similarity(tfidf, other_tfidf) > eps) {
                cluster.push_back(other_file);
                visited.insert(other_file);
            }
        }
        clusters.push_back(cluster);
    }
    return clusters;
}

int main() {
    std::string directory = "./clones/tier1";
    std::vector<std::string> html_files;
    
    // Collect HTML files
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.path().extension() == ".html") {
            html_files.push_back(entry.path().string());
        }
    }
    
    std::map<std::string, std::unordered_map<std::string, double>> tfidf_map;
    for (const std::string& file : html_files) {
        std::string text = extract_text_from_html(file);
        std::vector<std::string> tokens = tokenize(text);
        tfidf_map[file] = compute_tf(tokens);
    }
    
    double similarity_threshold = 0.4; // Adjust as needed
    std::vector<std::vector<std::string>> clusters = dbscan(tfidf_map, similarity_threshold);
    
    // Output clusters
    for (size_t i = 0; i < clusters.size(); ++i) {
        std::cout << "Cluster " << i + 1 << ": ";
        for (const std::string& file : clusters[i]) {
            std::cout << file << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}
