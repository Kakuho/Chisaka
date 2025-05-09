#pragma once

// for now it holds int as keys, however
// later will refactor to allow using any keys which can be compared

#include "aii/expected.hpp"
#include "aii/error.hpp"

namespace Aii{

template<typename T>
class RbTree{
  public:
    using KeyType = T;

    enum class Colour{
      Black, Red
    };

    struct Node{
      Node* parent;
      Node* left;
      Node* right;
      Colour colour;
      KeyType key;
    };
  
  public:
    RbTree() noexcept;
    RbTree(KeyType rootKey) noexcept;
    
    Aii::Expected<void, Error> LeftRotate(Node* node) noexcept;
    void RightRotate(Node* node) noexcept;

  private:
    Node m_root;
};

} // namespace Aii

template<typename T>
Aii::RbTree<T>::RbTree() noexcept
  :RbTree(0)
{

}

template<typename T>
Aii::RbTree<T>::RbTree(KeyType rootKey) noexcept
  :
    m_root{
      .parent = nullptr, 
      .left = nullptr, 
      .right = nullptr, 
      .colour = Colour::Black,
      .key = rootKey
    }
{

}

template<typename T>
Aii::Expected<void, Aii::Error> 
Aii::RbTree<T>::LeftRotate(Node* node) noexcept{
  // Possible Errors:
  //  InvalidArgument
  //  
  // On error, the tree is left in the state before the function call
  if(!node){
    return {Aii::Error::InvalidArgument};
  }
  Node* pivot = node->right;
  if(!pivot){
    return {Aii::Error::RuntimeError};
  }
}
