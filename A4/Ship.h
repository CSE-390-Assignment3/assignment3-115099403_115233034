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

namespace exams {
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

using Position = std::tuple<exams::X, exams::Y>;
} // namespace exams

// a short pause from exams to define hash for Position
namespace std {
template <> struct hash<exams::Position> {
  std::size_t operator()(const exams::Position &pos) const noexcept {
    return std::get<0>(pos) ^ (std::get<1>(pos) << 1);
  }
};
} // namespace std

// back to the exams business
namespace exams {
class BadPositionException {
  X x;
  Y y;
  std::string msg;

public:
  BadPositionException(X x, Y y, std::string msg)
      : x(x), y(y), msg(std::move(msg)) {}
  void print() const {
    std::cout << msg << " : X {" << x << "}, Y {" << y << "}\n";
  }
};

template <typename Examinee>
using Grouping =
    std::unordered_map<std::string,
                       std::function<std::string(const Examinee &)>>;

template <typename Examinee> class ExamHall {
  class GroupView {
    const std::unordered_map<Position, const Examinee &> *p_group = nullptr;
    using iterator_type =
        typename std::unordered_map<Position, const Examinee &>::const_iterator;

  public:
    GroupView(const std::unordered_map<Position, const Examinee &> &group)
        : p_group(&group) {}
    GroupView(int) {}
    auto begin() const { return p_group ? p_group->begin() : iterator_type{}; }
    auto end() const { return p_group ? p_group->end() : iterator_type{}; }
  };
  class iterator {
    using ExamineesItr =
        typename std::vector<std::optional<Examinee>>::const_iterator;
    ExamineesItr examinees_itr;
    ExamineesItr examinees_end;
    void set_itr_to_occupied_sit() {
      while (examinees_itr != examinees_end && !(*examinees_itr)) {
        ++examinees_itr;
      }
    }

  public:
    iterator(ExamineesItr examinees_itr, ExamineesItr examinees_end)
        : examinees_itr(examinees_itr), examinees_end(examinees_end) {
      set_itr_to_occupied_sit();
    }
    iterator operator++() {
      ++examinees_itr;
      set_itr_to_occupied_sit();
      return *this;
    }
    const Examinee &operator*() const { return examinees_itr->value(); }
    bool operator!=(iterator other) const {
      return examinees_itr != other.examinees_itr;
    }
  };

  std::vector<std::optional<Examinee>> examinees;
  int x_size;
  int y_size;
  Grouping<Examinee> groupingFunctions;
  using Pos2Examinee = std::unordered_map<Position, const Examinee &>;
  using Group = std::unordered_map<std::string, Pos2Examinee>;
  // all groupings by their grouping name
  mutable std::unordered_map<std::string, Group> groups;
  // private method
  int pos_index(X x, Y y) const {
    if (x >= 0 && x < x_size && y >= 0 && y < y_size) {
      return y * x_size + x;
    }
    throw BadPositionException(x, y, "index out of range");
  }
  Examinee &get_examinee(X x, Y y) {
    return examinees[pos_index(x, y)].value();
  }
  void addExamineeToGroups(X x, Y y) {
    Examinee &e = get_examinee(x, y);
    for (auto &group_pair : groupingFunctions) {
      groups[group_pair.first][group_pair.second(e)].insert(
          {std::tuple{x, y}, e});
    }
  }
  void removeExamineeFromGroups(X x, Y y) {
    Examinee &e = get_examinee(x, y);
    for (auto &group_pair : groupingFunctions) {
      groups[group_pair.first][group_pair.second(e)].erase(std::tuple{x, y});
    }
  }

public:
  ExamHall(X x, Y y, Grouping<Examinee> groupingFunctions) noexcept
      : examinees(x * y), x_size(x), y_size(y),
        groupingFunctions(std::move(groupingFunctions)) {}

  void sit(X x, Y y, Examinee e) noexcept(false) {
    auto &seat = examinees[pos_index(x, y)];
    if (seat) {
      throw BadPositionException(x, y, "occupied sit");
    }
    seat = std::move(e);
    addExamineeToGroups(x, y);
  }

  Examinee unsit(X x, Y y) noexcept(false) {
    auto &seat = examinees[pos_index(x, y)];
    if (!seat) {
      throw BadPositionException(x, y, "no examinee to unsit");
    }
    removeExamineeFromGroups(x, y);
    auto examinee = std::optional<Examinee>{}; // empty seat
    // swap empty seat with real examinee and return the real examinee
    // might be more efficient than creating a temp copy
    std::swap(seat, examinee); // seat would become empty, examinee would get
                               // the real examinee
    return examinee.value();
  }
  void move(X from_x, Y from_y, X to_x, Y to_y) noexcept(false) {
    // note that this implementation is problematic - if to "to" location is bad
    // an excpetion would be thrown after examinee already unsitted from
    // original seat leaving us in an invalidated state
    // TODO: fix above issue
    sit(to_x, to_y, unsit(from_x, from_y));
  }

  GroupView getExamineesViewByGroup(const std::string &groupingName,
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
            itr->second.insert({groupName, Pos2Examinee{}});
        //--------------------------------------------------------------------
        // OR, with auto tuple unpack
        //--------------------------------------------------------------------
        // auto [insert_itr, _] = itr->second.insert({groupName,
        // Pos2Examinee{}}); itr2 = insert_itr;
      }
      return GroupView{itr2->second};
    }
    return GroupView{0};
  }

  iterator begin() const { return {examinees.begin(), examinees.end()}; }
  iterator end() const { return {examinees.end(), examinees.end()}; }

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
  ExamHall(const ExamHall &) = delete;
  ExamHall &operator=(const ExamHall &) = delete;
  ExamHall(ExamHall &&) = default;
  ExamHall &operator=(ExamHall &&) = default;
  //-------------------------------------------------------
};
} // namespace exams
