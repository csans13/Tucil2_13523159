#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <iostream>
#include <vector>
#include <array>
#include <utility>
#include <stdexcept>
#include <memory>

using namespace std;

struct RGB
{
    int r, g, b;
};

struct Rect
{
    int x, y, width, height;
};

enum ErrorMethod
{
    Variance,
    MAD,
    MaxPixelDiff,
    Entropy
};

class QuadTreeNode
{
    private:
        Rect bounds;
        bool isLeaf;
        RGB avgColor;
        array<QuadTreeNode*, 4> childNode;

    public:
        QuadTreeNode();
        QuadTreeNode(int x, int y, int width, int height);
        ~QuadTreeNode();

        const Rect& getBounds() const noexcept;
        bool isLeafNode() const noexcept;
        bool hasChildren() const noexcept;
        RGB getAvgColor() const noexcept;
        QuadTreeNode* getChild(int idx) const noexcept;

        void setAvgColor(RGB avgColor) noexcept;
        void setLeaf(bool isLeaf) noexcept;
        void setChild(int idx, QuadTreeNode* node) noexcept;
        void setBounds(int x, int y, int width, int height) noexcept;

        void split();
        RGB calculateAvgColor(const vector<vector<RGB>>& image) const;
};

class QuadTree
{
    private:
        QuadTreeNode* root;
        int threshold;
        int minSize;

    public:
        QuadTree();
        ~QuadTree();

        QuadTreeNode* getRoot() const noexcept;
        int getThreshold() const noexcept;
        int getMinSize() const noexcept;

        int getMaxDepth() const;
        int getMaxDepth(QuadTreeNode* node) const;

        int getNodeCount() const;
        int getNodeCount(QuadTreeNode* node) const;

        float calculateError(const vector<vector<RGB>>& image, int x, int y, int width, int height, ErrorMethod method);

        void buildTree(const vector<vector<RGB>>& image, int x, int y, int width, int height, ErrorMethod method, int threshold, int minSize);
        QuadTreeNode* buildRecursive(const vector<vector<RGB>>& image, int x, int y, int width, int height, ErrorMethod method);

        void reconstructImage(vector<vector<RGB>>& image);
        void reconstructRecursive(QuadTreeNode* node, vector<vector<RGB>>& image);
};

#endif
