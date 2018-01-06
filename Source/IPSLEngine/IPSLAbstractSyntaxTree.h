/**
 * @file IPSLAbstractSyntaxTree.h
 *
 * This file declares and implements the IPSLAbstractSyntaxTree class.
 *
 * @author Arne Hasselbring
 */

#pragma once

#include <string>
#include <vector>

/**
 * @brief This class represents an abstract syntax tree in the IPS language.
 */
class IPSLAbstractSyntaxTree
{
public:
  /**
   * @brief This struct represents a node in the abstract syntax tree.
   */
  struct Node
  {
    /**
     * @brief This enum class enumerates the types of nodes.
     */
    enum class Type
    {
      number,      ///< A number node.
      symbol,      ///< A symbol node.
      string,      ///< A string node.
      comment,     ///< A comment node.
      sExpression, ///< An S-Expression node.
      qExpression, ///< A Q-Expression node.
      expression,  ///< An expression node.
      program,     ///< A program node.
      numOfTypes   ///< The number of node types.
    };
    /**
     * @brief Constructs an abstract syntax tree node.
     * @param type The type of the node.
     */
    explicit Node(Type type) :
      type(type)
    {
    }
    Type type;                  ///< The type of the node.
    std::string value;          ///< The value of the node (only valid for numbers, symbols, strings and comments).
    std::vector<Node> children; ///< The children of the node.
  };
  Node root = Node(Node::Type::numOfTypes); ///< The root of the tree.
};
