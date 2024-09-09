#ifndef __MAP_H__
#define __MAP_H__

#include "support_function_AVLTree.h"

namespace s21 {
template <typename Key, typename T>
class map : public AVLTree<Key, T> {
 public:
  class MapIterator;
  class ConstMapIterator;

  using key_type = Key;
  using mapped_type = T;
  using value_type = std::pair<const key_type, mapped_type>;
  using reference = value_type &;
  using const_reference = const value_type &;
  using iterator = MapIterator;
  using const_iterator = ConstMapIterator;
  using size_type = size_t;

  map() : AVLTree<Key, T>(){};
  map(std::initializer_list<value_type> const &items) {
    for (auto i = items.begin(); i != items.end(); i++) {
      insert(*i);
    }
  }
  map(const map &other) : AVLTree<Key, T>(other){};
  map(map &&other) noexcept : AVLTree<Key, T>(std::move(other)){};
  map &operator=(map &&other) noexcept {
    if (this != &other) {
      AVLTree<Key, T>::operator=(std::move(other));
    }
    return *this;
  }
  map &operator=(const map &other) {
    if (this != &other) {
      AVLTree<Key, T>::operator=(other);
    }
    return *this;
  }
  ~map() = default;

  iterator begin() {
    return map<Key, T>::MapIterator(
        AVLTree<Key, T>::GetMin(AVLTree<Key, T>::root_));
  }
  iterator end() {
    if (AVLTree<Key, T>::root_ == nullptr) {
      return begin();
    }

    typename AVLTree<Key, T>::Node *last_node =
        AVLTree<Key, T>::GetMax(AVLTree<Key, T>::root_);
    MapIterator test(nullptr, last_node);
    return test;
  }
  const_iterator cbegin() const {
    return map<Key, T>::ConstMapIterator(
        AVLTree<Key, T>::GetMin(AVLTree<Key, T>::root_));
  }
  const_iterator cend() const {
    if (AVLTree<Key, T>::root_ == nullptr) {
      return cbegin();
    }
    typename AVLTree<Key, T>::Node *last_node =
        AVLTree<Key, T>::GetMax(AVLTree<Key, T>::root_);
    ConstMapIterator test(nullptr, last_node);
    return test;
  }
  void merge(map &other) {
    map const_tree(other);
    iterator const_it = const_tree.begin();
    for (; const_it != const_tree.end(); const_it++) {
      auto key = (*const_it).first;
      auto obj = (*const_it).second;
      std::pair<iterator, bool> pr = insert(key, obj);
      if (pr.second) other.erase(pr.first);
    }
  }

  class MapIterator : public AVLTree<Key, T>::Iterator {
   public:
    friend class map;
    MapIterator() : AVLTree<Key, T>::Iterator(){};
    MapIterator(typename AVLTree<Key, T>::Node *node,
                typename AVLTree<Key, T>::Node *past_node = nullptr)
        : AVLTree<Key, T>::Iterator(node, past_node = nullptr){};
    value_type &operator*() {
      if (AVLTree<Key, T>::Iterator::iter_node_ == nullptr) {
        static value_type fake_value{};
        return fake_value;
      }
      std::pair<const key_type, mapped_type> pr =
          std::make_pair(AVLTree<Key, T>::Iterator::iter_node_->key_,
                         AVLTree<Key, T>::Iterator::iter_node_->value_);
      std::pair<const key_type, mapped_type> &ref = pr;
      return ref;
    }

   protected:
    T &return_value() {
      if (AVLTree<Key, T>::Iterator::iter_node_ == nullptr) {
        static T fake_value{};
        return fake_value;
      }
      return AVLTree<Key, T>::Iterator::iter_node_->value_;
    }
  };

  class ConstMapIterator : public MapIterator {
   public:
    friend class map;
    ConstMapIterator() : MapIterator(){};
    ConstMapIterator(typename AVLTree<Key, T>::Node *node,
                     typename AVLTree<Key, T>::Node *past_node = nullptr)
        : MapIterator(node, past_node = nullptr){};
    const_reference operator*() const { return MapIterator::operator*(); };
  };

  T &at(const Key &key) {
    auto it = find(key);
    if (it == nullptr) {
      throw "";
    }
    return it.return_value();
  }
  T &operator[](const Key &key) {
    auto it = find(key);
    if (it == nullptr) {
      auto pr = insert(std::make_pair(key, T()));
      it = pr.first;
    }
    return it.return_value();
  }
  std::pair<iterator, bool> insert(const value_type &value) {
    return insert(value.first, value.second);
  }
  std::pair<iterator, bool> insert(const Key &key, const T &obj) {
    std::pair<iterator, bool> return_value;
    if (AVLTree<Key, T>::root_ == nullptr) {
      AVLTree<Key, T>::root_ = new typename AVLTree<Key, T>::Node(key, obj);
      return_value.first = iterator(AVLTree<Key, T>::root_);
      return_value.second = true;
    } else {
      bool check_insert =
          AVLTree<Key, T>::RecursiveInsert(AVLTree<Key, T>::root_, key, obj);
      return_value.first = find(key);  // НАДО НАПИСАТЬ ФУНКЦИЮ FIND
      return_value.second = check_insert;
    }
    return return_value;
  }
  std::pair<iterator, bool> insert_or_assign(const Key &key, const T &obj) {
    auto it = find(key);
    if (it != this->end()) {
      erase(it);
      auto pr = insert(key, obj);
      pr.second = false;
      return pr;
    }
    return insert(key, obj);
  }
  template <class... Args>
  std::vector<std::pair<iterator, bool>> insert_many(Args &&...args) {
    std::vector<std::pair<typename map<Key, T>::iterator, bool>> vec;
    for (const auto &arg : {args...}) {
      vec.push_back(insert(arg));
    }
    return vec;
  }
  void erase(iterator pos) {
    if (AVLTree<Key, T>::root_ == nullptr || pos.iter_node_ == nullptr) return;
    AVLTree<Key, T>::root_ =
        AVLTree<Key, T>::RecursiveDelete(AVLTree<Key, T>::root_, (*pos).first);
  }

 private:
  iterator find(const Key &key) {
    typename AVLTree<Key, T>::Node *node =
        AVLTree<Key, T>::RecursiveFind(AVLTree<Key, T>::root_, key);
    return iterator(node);
  }
};

}  // namespace s21
#endif
