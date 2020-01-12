#ifndef __BST_HPP 
#define __BST_HPP

#include <iostream>
#include <algorithm>   // swap, sort
#include <utility>     // move, pair
#include <iterator>    // iterators
#include <memory>      // unique_ptr 
#include <functional>  // std::less
#include <vector>

template <typename K, typename V>
struct Node
{
    std::unique_ptr<Node> _left;
    std::unique_ptr<Node> _right;
    Node * _parent;
    std::pair<const K, V> _data;

    Node(const std::pair<const K, V> & data, Node * left, Node * right, Node * parent) noexcept : \
    _left{left}, _right{right}, _parent{parent}, _data{data} { }

    Node(const std::pair<const K, V> & data) noexcept : _left{}, _right{}, _parent{}, _data{data} { }

    Node(const std::pair<const K, V> & data, Node * parent) noexcept : _left{}, _right{}, _parent{parent}, _data{data} { }

    Node(std::pair<const K, V> && data, Node * left, Node * right, Node * parent) noexcept : \
    _left{left}, _right{right}, _parent{parent}, _data{std::move(data)} { }

    Node(std::pair<const K, V> && data) noexcept : _left{}, _right{}, _parent{}, _data{std::move(data)} { }

    Node(std::pair<const K, V> && data, Node * parent) noexcept : _left{}, _right{}, _parent{parent}, _data{std::move(data)} { }

    explicit Node (const std::unique_ptr<Node> & n) : _left{}, _right{}, _parent{}, _data{n->_data}
    { 
        if (n->_left)
        {
            _left = std::make_unique<Node>(n->_left);
            _left->_parent = this;
        }
        if (n->_right)
        {
            _right = std::make_unique<Node>(n->_right);
            _right->_parent = this;
        }
    }

    const K key() noexcept { return _data.first; }
    V value() noexcept { return _data.second; }
};

template <typename K, typename V, typename C = std::less<K>>
class bst
{
private:

    std::unique_ptr<Node<K,V>> _root;
    C comp = C();

    void fill_tree(std::vector<std::pair<const K,V>> & vec, int begin, int end);

public:

    bst() noexcept = default;

    bst(bst && tree) noexcept = default;

    bst & operator=(bst && tree) noexcept = default;

    bst(const bst & tree) : _root{}
    {
        if (tree._root)
            _root = std::make_unique<Node<K,V>>(tree._root);
    }

    bst & operator=(bst & tree)
    {
        _root.reset();

        auto tmp{tree};

        *this = std::move(tmp);

        return *this; 
    }

    Node<K,V> * root() noexcept { return _root.get(); }

    void clear() noexcept
    {
        _root.reset();
    }

    template <typename T>
    V & operator[](T && key);

    template <typename O>
    class __iterator;
    
    using iterator = __iterator<std::pair<const K,V>>;
    using const_iterator = __iterator<const std::pair<const K,V>>;
    
    iterator begin() noexcept 
    { 
        auto p = _root.get();
        if (!p)
            return iterator(p);
        while (p->_left)            // p->_left access requires p != nullptr or segmentation fault 
            p = (p->_left).get();   // (would otherwise dereference nullptr)
        return iterator{p}; 
    }
    iterator end() noexcept 
    { 
        return iterator{nullptr}; 
    }
    
    const_iterator begin() const noexcept
    { 
        auto p = _root.get();
        if (!p)
            return const_iterator(p);
        while (p->_left)
            p = (p->_left).get();
        return const_iterator{p}; 
    } 
    const_iterator end() const noexcept
    { 
        return const_iterator{nullptr}; 
    }
    
    const_iterator cbegin() const noexcept
    { 
        auto p = _root.get();
        if (!p)
            return const_iterator(p);
        while (p->_left)
            p = (p->_left).get();
        return const_iterator{p};  
    }
    const_iterator cend() const noexcept
    { 
        return const_iterator{nullptr}; 
    }
  
    iterator find(const K & key);

    const_iterator find(const K & key) const;

    std::pair<iterator, bool> insert(const std::pair<const K, V> & data);

    std::pair<iterator, bool> insert(std::pair<const K, V> && data);

    template <typename... Types> 
    std::pair<iterator, bool> emplace(Types&&... args);

    friend std::ostream & operator<<(std::ostream & os, const bst & tree)
    {
        for (const auto it : tree)
            os << it.first << " ";
        return os;
    }

    void balance();
};

template <typename K, typename V, typename C>
void bst<K,V,C>::fill_tree(std::vector<std::pair<const K,V>> & vec, int begin, int end)
{
    insert(vec[(begin + end)/2]);
    
    if (begin >= end)
        return;
    
    fill_tree(vec, begin, (begin + end)/2 - 1);
    fill_tree(vec, (begin + end)/2 + 1, end);
}

template <typename K, typename V, typename C>
void bst<K,V,C>::balance()
{
    if (!_root)
        return;

    std::vector<std::pair<const K,V>> temp_vec{};

    for (auto it = begin(); it != end(); ++it)
        temp_vec.push_back(*it);

    clear();

    fill_tree(temp_vec, 0, temp_vec.size() - 1);
}

template <typename K, typename V, typename C>
typename bst<K,V,C>::iterator bst<K,V,C>::find(const K & key)
{ 
    auto p = _root.get();

    bool go_right;
    bool go_left;

    while (p)
    {
        go_right = comp(p->_data.first, key);
        go_left = comp(key,p->_data.first);

        if (go_right)
            p = (p->_right).get();
        else if (go_left)
            p = (p->_left).get();
        else 
            return iterator(p);
    }

    return end(); 
}

template <typename K, typename V, typename C>
typename bst<K,V,C>::const_iterator bst<K,V,C>::find(const K & key) const
{ 
    auto p = _root.get();

    bool go_right;
    bool go_left;

    while (p)
    {
        go_right = comp(p->_data.first, key);
        go_left = comp(key,p->_data.first);

        if (go_right)
            p = (p->_right).get();
        else if (go_left)
            p = (p->_left).get();
        else 
            return iterator(p);
    }

    return end(); 
}

template <typename K, typename V, typename C>
std::pair<typename bst<K,V,C>::iterator, bool> bst<K,V,C>::insert(const std::pair<const K, V> & data)
{    
    if (_root == nullptr)
    {
        _root.reset(new Node<K,V> {data, nullptr});
        return std::make_pair(iterator(_root.get()),true);
    }

    auto p = _root.get();

    bool go_right;
    bool go_left;

    while (true)
    {
        go_right = comp(p->_data.first, data.first);
        go_left = comp(data.first, p->_data.first);

        if (go_right)
        {
            if (p->_right)
                p = (p->_right).get();
            else
            {
                p->_right.reset(new Node<K,V> {data, p});
                return std::make_pair(iterator(p->_right.get()),true);
            }
        }
        else if (go_left)
        {
            if (p->_left)
                p = (p->_left).get();
            else
            {
                p->_left.reset(new Node<K,V> {data, p});
                return std::make_pair(iterator(p->_left.get()),true);
            }  
        }   
        else
            return std::make_pair(iterator(p),false);
    }
}

template <typename K, typename V, typename C>
std::pair<typename bst<K,V,C>::iterator, bool> bst<K,V,C>::insert(std::pair<const K, V> && data)
{
    if (_root == nullptr)
    {
        _root.reset(new Node<K,V> {std::move(data), nullptr});
        return std::make_pair(iterator(_root.get()),true);
    }

    auto p = _root.get();

    bool go_right;
    bool go_left;

    while (true)
    {
        go_right = comp(p->_data.first, data.first);
        go_left = comp(data.first, p->_data.first);

        if (go_right)
        {
            if (p->_right)
                p = (p->_right).get();
            else
            {
                p->_right.reset(new Node<K,V> {std::move(data), p});
                return std::make_pair(iterator(p->_right.get()),true);
            }
        }
        else if (go_left)
        {
            if (p->_left)
            {
                p = (p->_left).get();
            }
            else
            {
                p->_left.reset(new Node<K,V> {std::move(data), p});
                return std::make_pair(iterator(p->_left.get()),true);
            }  
        }   
        else
            return std::make_pair(iterator(p),false);            
    }
}

template <typename K, typename V, typename C>
template <typename... Types> 
std::pair<typename bst<K,V,C>::iterator, bool> bst<K,V,C>::emplace(Types&&... args)
{
    return insert(std::pair<const K, V>{std::forward<Types>(args)...});
}

template <typename K, typename V, typename C>
template <typename T>
V & bst<K,V,C>::operator[](T && key)
{
    auto p = find(std::forward<T>(key));

    if (p != end())
        return p->second;
    else
    {
        p = insert({std::forward<T>(key), V{}}).first;
        return p->second;
    }
}

template <typename K, typename V, typename C>
template <typename O>
class bst<K,V,C>::__iterator
{
private:

    Node<K,V> * current;

public:

    explicit __iterator(Node<K,V> * x) noexcept : current{x} {}
    
    using value_type = O; // typename of iterator
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;
    using reference = value_type&;
    using pointer = value_type*;
    
    reference operator*() const noexcept { return current->_data; } // return an O reference
    pointer operator->() const noexcept { return &(*(*this)); }

    friend bool operator==(const __iterator & a, const __iterator & b)
    {
        return a.current == b.current;
    }

    friend bool operator!=(const __iterator & a, const __iterator & b)
    {
        return  !(a == b);
    }

    __iterator & operator++() noexcept // pre-increment
    {
        auto current_right = current->_right.get();
        auto current_parent = current->_parent;

        if (current_right)
        {
            while (current_right->_left)
                current_right = current_right->_left.get();
            current = current_right;
        }
        else 
        {
            while (current_parent && current_parent->_right.get() == current)
            {
                current = current_parent;
                current_parent = current_parent->_parent;
            }
            current = current_parent;
        }

        return *this;
    }
};

#endif
