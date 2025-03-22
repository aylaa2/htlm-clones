## html clones ##


The core goal of this task is to:

Group similar HTML files based on the visible text content within them.

This isn't just about matching raw content—it’s about semantic similarity. The trick here is to:

Extract meaningful visible text from HTML (ignoring styles, scripts).

Convert raw text into a vectorized representation.

Compare document similarity using cosine similarity.

Cluster the similar pages together

# HTML Clustering with Cosine Similarity & DBSCAN

This project tackles the problem of **identifying clusters of similar HTML pages** based on their visible text content. At a high level, it reads HTML files, extracts meaningful content (ignoring scripts and styles), vectorizes them using term frequency, and clusters them using a lightweight density-based approach inspired by **DBSCAN**.

---

##  Problem Statement

Modern businesses often deal with vast amounts of semi-structured data scraped from the web—often duplicated or slightly varied versions of the same content. Grouping similar HTML pages can uncover:

- **Duplicate content**
- **Fake or malicious clones**
- **Content syndication patterns**

While simple hashing fails for semantically similar content, we want to **understand similarity based on actual readable content**. That’s where Natural Language Processing (NLP) and vector similarity shine.

---

## 🎯 Objective

- Extract **visible, meaningful text** from HTML files.
- Preprocess and vectorize content using **Term Frequency (TF)**.
- Compare documents using **cosine similarity**.
- Cluster documents using a simplified **DBSCAN** algorithm.
- Output the clusters for further manual inspection or downstream automation.

---

## 🧠 Design Thinking & Reasoning

Before jumping into code, I tried to deeply understand what "similarity" really means in this context.

- **HTML is noisy**: Includes scripts, styles, navbars, etc.
- **Content structure matters**: But we only care about **textual meaning**.
- **Word overlap is meaningful**: So we use TF and cosine similarity.
- **No fixed number of clusters**: DBSCAN is ideal since it’s density-based.

### 🧪 Options Considered

| Option | Why Considered | Why Rejected |
|-------|----------------|--------------|
| **Hashing (e.g., MD5)** | Fast for duplicates | Doesn’t catch semantic similarity |
| **TF-IDF** | More nuanced weighting | For small dataset, TF is simpler and good enough |
| **K-Means** | Easy to implement | Requires predefining `k`, not ideal for this use-case |
| **Cosine + DBSCAN** ✅ | Unsupervised, density-aware, interpretable | Chosen as best balance of accuracy and simplicity |

---

## ⚙️ Technical Overview

### 🛠 Stack
- **C++17** for performance and low-level control
- **Gumbo Parser** to safely parse and traverse HTML
- **Standard Library** for text processing, math, and filesystem

### 🧩 Components

#### 1. HTML Parsing
Uses `gumbo-parser` to recursively extract **visible** text (ignores `<script>`, `<style>`).

#### 2. Tokenization
Lowercases and splits the text into words. (No stopword removal for now — could improve.)

#### 3. TF Vectorization
Counts frequency of each word, normalized by total words. Lightweight and effective for small corpora.

#### 4. Cosine Similarity
Computes similarity between every pair of documents as:

#### 5. DBSCAN Clustering
- For each document, finds its neighbors with similarity > `eps`.
- Forms clusters by grouping all such close neighbors.
- No need to predefine number of clusters.

---

## 🖥️ Example Output


Cluster 1: ./clones/tier1/aemails.org.html ./clones/tier1/aerex.eu.html ...
Cluster 2: ./clones/tier1/someotherpage.html ...


to run the program 
1. build it: make
2. Run it: ./main
3. Make sure to have a directory like: ./clones/tier1/
4.  Install Gumbo Parser, If not already installed: sudo apt-get install libgumbo-dev


This project pushed me to balance text understanding and algorithm design under real-world constraints. Instead of relying on heavyweight NLP libraries, I leaned on fundamental concepts—TF, cosine similarity, density clustering—and brought them together with C++’s efficiency.

It may not scale to billions of pages yet—but the approach is principled, flexible, and a solid foundation to build on.

Thanks for reading 🙌