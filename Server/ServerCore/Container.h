#pragma once

#include "Types.h"
#include "Allocator.h"
#include <array>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

using namespace std;

template<typename T, uint32 SIZE>
using Xarray = array<T, SIZE>;

template<typename T>
using Xvector = vector<T, StlAllocator<T>>;

template<typename T>
using Xlist = list<T, StlAllocator<T>>;

template<typename K, typename V, typename Pred = less<K>>
using Xmap = map<K, V, Pred, StlAllocator<pair<const K, V>>>;

template<typename K, typename Pred = less<K>>
using Xset = set<K, Pred, StlAllocator<K>>;

template<typename T>
using Xdeque = deque<T, StlAllocator<T>>;

template<typename T, typename Container = Xdeque<T>>
using Xqueue = queue<T, Container>;

template<typename T, typename Container = Xdeque<T>>
using Xstack = stack<T, Container>;

template<typename T, typename Container = Xvector<T>, typename Pred = less<typename Container::value_type>>
using XpriorityQueue = priority_queue<T, Container, Pred>;

using Xstring = basic_string<char, char_traits<char>, StlAllocator<char>>;

using Xwstring = basic_string<wchar_t, char_traits<wchar_t>, StlAllocator<wchar_t>>;

template<typename K, typename T, typename Hasher = hash<K>, typename KeyEq = equal_to<K>>
using XhashMap = unordered_map<K, T, Hasher, KeyEq, StlAllocator<pair<const K, T>>>;

template<typename K, typename Hasher = hash<K>, typename KeyEq = equal_to<K>>
using XhashSet = unordered_set<K, Hasher, KeyEq, StlAllocator<K>>;