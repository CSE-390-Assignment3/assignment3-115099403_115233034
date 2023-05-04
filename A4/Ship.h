// =====================================
// Ship - Assignment 4
// Sukesh Cheripalli, Puneet Udupi
// =====================================
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace shipping {
template <typename T> class NamedType {
  T t;

public:
  explicit NamedType(T t) : t(t) {}
  operator T() const { return t; }
};

struct X : NamedType<int> {
  using NamedType<int>::NamedType;
};

struct Y : NamedType<int> {
  using NamedType<int>::NamedType;
};

struct Height : NamedType<int> {
  using NamedType<int>::NamedType;
};

using Position = std::tuple<shipping::X, shipping::Y>;
} // namespace shipping

// a short pause from shipping to define hash for Position
namespace std {
template <> struct hash<shipping::Position> {
  std::size_t operator()(const shipping::Position &pos) const noexcept {
    return std::get<0>(pos) ^ (std::get<1>(pos) << 1);
  }
};
} // namespace std

// back to the shipping business
namespace shipping {
// TODO: verify exception definition
class BadShipOperationException {
  // X x;
  // Y y;
  std::string msg;

public:
  BadShipOperationException(X x, Y y, std::string msg) : msg(std::move(msg)) {}
  void print() const { std::cout << msg << std::endl; }
};

template <typename Container>
using Grouping =
    std::unordered_map<std::string,
                       std::function<std::string(const Container &)>>;

template <typename Container> class Ship {
  class GroupView {
    const std::unordered_map<Position, const Container &> *p_group = nullptr;
    using iterator_type =
        typename std::unordered_map<Position,
                                    const Container &>::const_iterator;

  public:
    GroupView(const std::unordered_map<Position, const Container &> &group)
        : p_group(&group) {}
    GroupView(int) {}
    auto begin() const { return p_group ? p_group->begin() : iterator_type{}; }
    auto end() const { return p_group ? p_group->end() : iterator_type{}; }
  };
  class iterator {
    using ContainersItr =
        typename std::vector<std::optional<Container>>::const_iterator;
    ContainersItr containers_itr;
    ContainersItr containers_end;
    void set_itr_to_occupied_load() {
      while (containers_itr != containers_end && !(*containers_itr)) {
        ++containers_itr;
      }
    }

  public:
    iterator(ContainersItr containers_itr, ContainersItr containers_end)
        : containers_itr(containers_itr), containers_end(containers_end) {
      set_itr_to_occupied_load();
    }
    iterator operator++() {
      ++containers_itr;
      set_itr_to_occupied_load();
      return *this;
    }
    const Container &operator*() const { return containers_itr->value(); }
    bool operator!=(iterator other) const {
      return containers_itr != other.containers_itr;
    }
  };

  std::vector<std::optional<Container>> containers;
  int x_size;
  int y_size;
  int h_size;

  Grouping<Container> groupingFunctions;
  using Pos2Container = std::unordered_map<Position, const Container &>;
  using Group = std::unordered_map<std::string, Pos2Container>;
  // all groupings by their grouping name
  mutable std::unordered_map<std::string, Group> groups;
  // private method
  int pos_index(X x, Y y) const {
    if (x >= 0 && x < x_size && y >= 0 && y < y_size) {
      return y * x_size + x;
    }
    throw BadShipOperationException(x, y, "index out of range");
  }
  Container &get_container(X x, Y y) {
    return containers[pos_index(x, y)].value();
  }
  void addContainerToGroups(X x, Y y) {
    Container &e = get_container(x, y);
    for (auto &group_pair : groupingFunctions) {
      groups[group_pair.first][group_pair.second(e)].insert(
          {std::tuple{x, y}, e});
    }
  }
  void removeContainerFromGroups(X x, Y y) {
    Container &e = get_container(x, y);
    for (auto &group_pair : groupingFunctions) {
      groups[group_pair.first][group_pair.second(e)].erase(std::tuple{x, y});
    }
  }

public:
  // TODO: create containers for x*y*h in ctors
  // TODO: implement restrictions

  Ship(X x, Y y, Height max_height) noexcept
      : x_size(x), y_size(y), h_size(max_height) {}

  Ship(X x, Y y, Height max_height,
       std::vector<std::tuple<X, Y, Height>> restrictions) noexcept(false)
      : : x_size(x), y_size(y), h_size(max_height) {}

  Ship(X x, Y y, Height max_height,
       std::vector<std::tuple<X, Y, Height>> restrictions,
       Grouping<Container> groupingFunctions) noexcept(false)
      : x_size(x), y_size(y), h_size(max_height),
        groupingFunctions(std::move(groupingFunctions)) {}

  Ship(X x, Y y, Grouping<Container> groupingFunctions) noexcept
      : containers(x * y), x_size(x), y_size(y),
        groupingFunctions(std::move(groupingFunctions)) {}

  void load(X x, Y y, Container c) noexcept(false) {
    // TODO: handle height of the container
    auto &compartment = containers[pos_index(x, y)];
    if (compartment) {
      throw BadShipOperationException(x, y, "occupied load");
    }
    compartment = std::move(c);
    addContainerToGroups(x, y);
  }

  Container unload(X x, Y y) noexcept(false) {
    auto &compartment = containers[pos_index(x, y)];
    if (!compartment) {
      throw BadShipOperationException(x, y, "no container to unload");
    }
    removeContainerFromGroups(x, y);
    auto container = std::optional<Container>{}; // empty compartment
    // swap empty compartment with real container and return the real container
    // might be more efficient than creating a temp copy
    std::swap(compartment, container); // compartment would become empty,
                                       // container would get the real container
    return container.value();
  }
  void move(X from_x, Y from_y, X to_x, Y to_y) noexcept(false) {
    // note that this implementation is problematic - if to "to" location is bad
    // an excpetion would be thrown after container already unloaded from
    // original compartment leaving us in an invalidated state
    // TODO: fix above issue (from skeleton)
    // TODO: add height checking & throw exception
    load(to_x, to_y, unload(from_x, from_y));
  }

  // TODO: verify if this works out of box from ExamHall
  GroupView getContainersViewByGroup(const std::string &groupingName,
                                     const std::string &groupName) const {
    auto itr = groups.find(groupingName);
    if (itr == groups.end() &&
        groupingFunctions.find(groupingName) != groupingFunctions.end()) {
      // for use of insert and tie, see:
      // 1. https://en.cppreference.com/w/cpp/utility/tuple/tie
      // 2. https://en.cppreference.com/w/cpp/container/unordered_map/insert
      std::tie(itr, std::ignore) = groups.insert({groupingName, Group{}});
      //--------------------------------------------------------------------
      // OR, with auto tuple unpack
      // see: https://en.cppreference.com/w/cpp/language/structured_binding
      //--------------------------------------------------------------------
      // auto [insert_itr, _] = groups.insert({groupingName, Group{}});
      // itr = insert_itr;
    }
    if (itr != groups.end()) {
      const auto &grouping = itr->second;
      auto itr2 = grouping.find(groupName);
      if (itr2 == grouping.end()) {
        std::tie(itr2, std::ignore) =
            itr->second.insert({groupName, Pos2Container{}});
        //--------------------------------------------------------------------
        // OR, with auto tuple unpack
        //--------------------------------------------------------------------
        // auto [insert_itr, _] = itr->second.insert({groupName,
        // Pos2Container{}}); itr2 = insert_itr;
      }
      return GroupView{itr2->second};
    }
    return GroupView{0};
  }
  // TODO: implement API
  GroupView getContainersViewByPosition(X x, Y y) const { return GroupView{0}; }

  iterator begin() const { return {containers.begin(), containers.end()}; }
  iterator end() const { return {containers.end(), containers.end()}; }

  //-------------------------------------------------------
  // Note:
  //-------------------------------------------------------
  // relying on move to keep validity of references to items in container
  // this is not guarenteed by the spec (yet) but there is almost no way
  // not to support that, as swap(vec1, vec2) must guarentee validity of
  // references see:
  // https://stackoverflow.com/questions/11021764/does-moving-a-vector-invalidate-iterators
  // and:
  // https://stackoverflow.com/questions/25347599/am-i-guaranteed-that-pointers-to-stdvector-elements-are-valid-after-the-vector
  // see also - spec open proposal:
  // http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-active.html#2321
  //-------------------------------------------------------
  Ship(const Ship &) = delete;
  Ship &operator=(const Ship &) = delete;
  Ship(Ship &&) = default;
  Ship &operator=(Ship &&) = default;
  //-------------------------------------------------------
};
} // namespace shipping
