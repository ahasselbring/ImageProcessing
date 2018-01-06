/**
 * @file IPSLAbstractSyntaxTreeBuilder.h
 *
 * This file declares the IPSLAbstractSyntaxTreeBuilder class.
 *
 * @author Arne Hasselbring
 */

#pragma once

#include <vector>

#include "IPSLAbstractSyntaxTree.h"

class IPSLAbstractSyntaxTreeBuilder final
{
public:
  /**
   * @brief init Initializes the builder.
   * @param tree The tree that should be built.
   */
  void init(IPSLAbstractSyntaxTree& tree);
  /**
   * @brief Creates a child node below the current node and descends into it.
   * @param type The type of the new node.
   */
  void enter(IPSLAbstractSyntaxTree::Node::Type type);
  /**
   * @brief Finishes the current node and goes up to the parent node.
   */
  void leave();
  /**
   * @brief Returns a reference to the current node.
   * @return A reference to the current node.
   */
  IPSLAbstractSyntaxTree::Node& current();
private:
  IPSLAbstractSyntaxTree::Node* currentNode = nullptr;    ///< The node that is currently manipulated.
  std::vector<IPSLAbstractSyntaxTree::Node*> parentStack; ///< The stack of parents.
};
