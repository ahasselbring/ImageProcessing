/**
 * @file IPSLAbstractSyntaxTreeBuilder.cpp
 *
 * This file implements the IPSLAbstractSyntaxTreeBuilder class.
 *
 * @author Arne Hasselbring
 */

#include <cassert>

#include "IPSLAbstractSyntaxTreeBuilder.h"

void IPSLAbstractSyntaxTreeBuilder::init(IPSLAbstractSyntaxTree& tree)
{
  tree.root = IPSLAbstractSyntaxTree::Node(IPSLAbstractSyntaxTree::Node::Type::program);
  currentNode = &(tree.root);
  parentStack.clear();
}

void IPSLAbstractSyntaxTreeBuilder::enter(IPSLAbstractSyntaxTree::Node::Type type)
{
  currentNode->children.emplace_back(type);
  parentStack.push_back(currentNode);
  currentNode = &(currentNode->children.back());
}

void IPSLAbstractSyntaxTreeBuilder::leave()
{
  assert(!parentStack.empty());
  currentNode = parentStack.back();
  parentStack.pop_back();
}

IPSLAbstractSyntaxTree::Node& IPSLAbstractSyntaxTreeBuilder::current()
{
  return *currentNode;
}
