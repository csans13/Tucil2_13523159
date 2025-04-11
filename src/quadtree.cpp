#include "header/quadtree.hpp"
#include "header/errormeasurement.hpp"

QuadTreeNode::QuadTreeNode(): bounds{0, 0, 0, 0}, isLeaf(true), avgColor{0, 0, 0}
{
    childNode.fill(nullptr);
} 

QuadTreeNode::QuadTreeNode(int x, int y, int width, int height): bounds{x, y, width, height}, isLeaf(true), avgColor{0, 0, 0}
{
    childNode.fill(nullptr);
}

QuadTreeNode::~QuadTreeNode()
{
    for (auto* child : childNode)
    {
        if (child != nullptr)
        {
            delete child;
            child = nullptr;
        }
    }
}

const Rect& QuadTreeNode::getBounds() const noexcept
{
    return bounds;
}

bool QuadTreeNode::isLeafNode() const noexcept
{
    return isLeaf;
}

bool QuadTreeNode::hasChildren() const noexcept
{
    return !isLeaf;
}

RGB QuadTreeNode::getAvgColor() const noexcept
{
    return avgColor;
}

QuadTreeNode* QuadTreeNode::getChild(int idx) const noexcept
{
    if (idx > 3 || idx < 0)
    {
        return nullptr;
    }
    return childNode[idx];
}

void QuadTreeNode::setAvgColor(RGB avgColor) noexcept
{
    this->avgColor = avgColor;
}

void QuadTreeNode::setLeaf(bool isLeaf) noexcept
{
    this->isLeaf = isLeaf;
}

void QuadTreeNode::setChild(int idx, QuadTreeNode* node) noexcept
{
    this->childNode[idx] = node;
}

void QuadTreeNode::setBounds(int x, int y, int width, int height) noexcept
{
    this->bounds = {x, y, width, height};
}

void QuadTreeNode::split()
{
    int midW = bounds.width/2;
    int midH = bounds.height/2;

    setLeaf(false);

    childNode[0] = new QuadTreeNode(bounds.x, bounds.y, midW, midH);
    childNode[1] = new QuadTreeNode(bounds.x + midW, bounds.y, bounds.width - midW, midH);
    childNode[2] = new QuadTreeNode(bounds.x, bounds.y + midH, midW, bounds.height - midH);
    childNode[3] = new QuadTreeNode(bounds.x + midW, bounds.y + midH, bounds.width - midW, bounds.height - midH);
}

RGB QuadTreeNode::calculateAvgColor(const vector<vector<RGB>>& image) const
{
    long long sumR = 0, sumG = 0, sumB = 0;
    int totalPixels = bounds.width * bounds.height;

    for (int i = bounds.y; i < bounds.y + bounds.height; ++i)
    {
        for (int j = bounds.x; j < bounds.x + bounds.width; ++j)
        {
            const RGB& pixel = image[i][j];
            sumR += pixel.r;
            sumG += pixel.g;
            sumB += pixel.b;
        }
    }
    
    return RGB{
        static_cast<int>(sumR/totalPixels),
        static_cast<int>(sumG/totalPixels),
        static_cast<int>(sumB/totalPixels)
    };
}

QuadTree::QuadTree() : root(nullptr), threshold(0), minSize(1) {}

QuadTree::~QuadTree()
{
    if (root != nullptr)
    {
        delete root;
        root = nullptr;
    }
}

QuadTreeNode* QuadTree::getRoot() const noexcept
{
    return root;
}

int QuadTree::getThreshold() const noexcept
{
    return threshold;
}

int QuadTree::getMinSize() const noexcept
{
    return minSize;
}

int QuadTree::getMaxDepth() const
{
    return getMaxDepth(root);
}

int QuadTree::getMaxDepth(QuadTreeNode* node) const
{
    if (node == nullptr)
    {
        return 0;
    }
    if (node->isLeafNode())
    {
        return 1;
    }
    
    int maxDepth = 0;
    for (int i = 0; i < 4; ++i)
    {
        maxDepth = max(maxDepth, getMaxDepth(node->getChild(i)));
    }
    return 1 + maxDepth;
}

int QuadTree::getNodeCount() const
{
    getNodeCount(root);
}

int QuadTree::getNodeCount(QuadTreeNode* node) const
{
    if (!node)
    {
        return 0;
    }

    int count = 1;
    for (int i = 0; i < 4; ++i)
    {
        count += getNodeCount(node->getChild(i));
    }
    return count;
}

float QuadTree::calculateError(const vector<vector<RGB>>& image, int x, int y, int width, int height, ErrorMethod method)
{
    switch (method)
    {
    case 0:
        return ErrorMeasurement::computeVariance(image, x, y, width, height);

    case 1:
        return ErrorMeasurement::computeMAD(image, x, y, width, height);

    case 2:
        return ErrorMeasurement::computeMaxPixelDiff(image, x, y, width, height);

    case 3:
        return ErrorMeasurement::computeEntropy(image, x, y, width, height);
    
    default:
        return 0.0f;
    }
}

void QuadTree::buildTree(const vector<vector<RGB>>& image, int x, int y, int width, int height, ErrorMethod method, int threshold, int minSize)
{
    this->threshold = threshold;
    this->minSize = minSize;
    this->root = buildRecursive(image, x, y, width, height, method);
}

QuadTreeNode* QuadTree::buildRecursive(const vector<vector<RGB>>& image, int x, int y, int width, int height, ErrorMethod method)
{
    QuadTreeNode* node = new QuadTreeNode(x, y, width, height);
    float error = calculateError(image, x, y, width, height, method);

    if (width <= minSize || height <= minSize || error < threshold)
    {
        RGB mean = node->calculateAvgColor(image);
        node->setAvgColor(mean);
        return node;
    }
    
    node->split();
    for (int i = 0; i < 4; ++i)
    {
        QuadTreeNode* child = node->getChild(i);
        node->setChild(i, buildRecursive(image, child->getBounds().x, child->getBounds().y, child->getBounds().width, child->getBounds().height, method));
    }
    
    return node;
}

void QuadTree::reconstructImage(vector<vector<RGB>>& image)
{
    if (!root)
    {
        return;
    }
    reconstructRecursive(root, image);
}

void QuadTree::reconstructRecursive(QuadTreeNode* node, vector<vector<RGB>>& image)
{
    if (node->isLeafNode())
    {
        const Rect& rect = node->getBounds();
        for (int i = rect.y; i < rect.y + rect.height; ++i)
        {
            for (int j = rect.x; j < rect.x + rect.width; ++j)
            {
                image[i][j] = node->getAvgColor();
            }
        }
    }
    else
    {
        for (int i = 0; i < 4; ++i)
        {
            reconstructRecursive(node->getChild(i), image);
        }
    }
}
