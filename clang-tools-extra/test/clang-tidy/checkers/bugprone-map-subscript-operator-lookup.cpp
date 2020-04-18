// RUN: %check_clang_tidy %s bugprone-map-subscript-operator-lookup %t

namespace std {

template <typename T>
struct allocator {};

template <typename T>
struct equal_to {};

template <typename T>
struct hash {};

template <typename T>
struct less {};

template <typename T, typename U>
struct pair {};

template <typename Key,
          typename T,
          typename Compare = std::less<Key>,
          typename Allocator = std::allocator<std::pair<const Key, T>>>
struct map {
  T &operator[](const Key &);
  T &operator[](Key &&);
};

// the check should be able to match std lib calls even if the functions are
// declared inside inline namespaces
inline namespace v1 {

template <typename Key,
          typename T,
          typename Hash = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>,
          typename Allocator = std::allocator<std::pair<const Key, T>>>
struct unordered_map {
  T &operator[](const Key &);
  T &operator[](Key &&);
};

} // namespace v1
} // namespace std

struct Foo {
  int getter() const;
  void setter(int);
  bool operator==(const Foo &) const;
};

using FooPtr = Foo *;

void readInt(const int &);
void writeInt(int &);
void copyInt(int);

void readFoo(const Foo &);
void writeFoo(Foo &);
void copyFoo(Foo);

void copyFooPtr(Foo *);
void copyConstFooPtr(const Foo *);

std::map<int, int> IntMap;
std::map<int, Foo> FooMap;
std::unordered_map<int, FooPtr> FooPtrMap;

void copyFromMap() {
  int A = IntMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:11: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
  Foo B = FooMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:11: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
  FooPtr C = FooPtrMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:14: warning: do not use operator[] for unordered_map lookup [bugprone-map-subscript-operator-lookup]
}

void constRefFromMap() {
  const int &A = IntMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:18: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
  const Foo &B = FooMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:18: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
  const FooPtr &C = FooPtrMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:21: warning: do not use operator[] for unordered_map lookup [bugprone-map-subscript-operator-lookup]
}

void constPtrFromMap() {
  const int *A = &IntMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:19: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
  const Foo *B = &FooMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:19: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
  const FooPtr *C = &FooPtrMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:22: warning: do not use operator[] for unordered_map lookup [bugprone-map-subscript-operator-lookup]
}

int returnInt() {
  return IntMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:10: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
}

Foo returnFoo() {
  return FooMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:10: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
}

FooPtr returnFooPtr() {
  return FooPtrMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:10: warning: do not use operator[] for unordered_map lookup [bugprone-map-subscript-operator-lookup]
}

const int &returnRefToConstInt() {
  return IntMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:10: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
}

const Foo &returnRefToConstFoo() {
  return FooMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:10: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
}

const FooPtr &returnRefToConstFooPtr() {
  return FooPtrMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:10: warning: do not use operator[] for unordered_map lookup [bugprone-map-subscript-operator-lookup]
}

const int *returnPtrToConstInt() {
  return &IntMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:11: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
}

const Foo *returnPtrToConstFoo() {
  return &FooMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:11: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
}

const FooPtr *returnPtrToConstFooPtr() {
  return &FooPtrMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:11: warning: do not use operator[] for unordered_map lookup [bugprone-map-subscript-operator-lookup]
}

void callReadX() {
  readInt(IntMap[1]);
  // CHECK-MESSAGES: [[@LINE-1]]:11: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
  readFoo(FooMap[1]);
  // CHECK-MESSAGES: [[@LINE-1]]:11: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
}

void callCopyX() {
  copyInt(IntMap[1]);
  // CHECK-MESSAGES: [[@LINE-1]]:11: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
  copyFoo(FooMap[1]);
  // CHECK-MESSAGES: [[@LINE-1]]:11: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
  copyFooPtr(FooPtrMap[1]);
  // CHECK-MESSAGES: [[@LINE-1]]:14: warning: do not use operator[] for unordered_map lookup [bugprone-map-subscript-operator-lookup]
  copyConstFooPtr(FooPtrMap[1]);
  // CHECK-MESSAGES: [[@LINE-1]]:19: warning: do not use operator[] for unordered_map lookup [bugprone-map-subscript-operator-lookup]
}

void passAddrToFunctionTakingConstPtr() {
  copyConstFooPtr(&FooMap[1]);
  // CHECK-MESSAGES: [[@LINE-1]]:20: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
}

void callConstMemf() {
  FooMap[1].getter();
  // CHECK-MESSAGES: [[@LINE-1]]:3: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
}

void deref() {
  Foo F = *FooPtrMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:12: warning: do not use operator[] for unordered_map lookup [bugprone-map-subscript-operator-lookup]
  FooPtrMap[1]->getter();
  // CHECK-MESSAGES: [[@LINE-1]]:3: warning: do not use operator[] for unordered_map lookup [bugprone-map-subscript-operator-lookup]
  FooPtrMap[1]->setter(1);
  // CHECK-MESSAGES: [[@LINE-1]]:3: warning: do not use operator[] for unordered_map lookup [bugprone-map-subscript-operator-lookup]
}

void arithmetic() {
  int A = 1 + IntMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:15: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
}

void comparison() {
  bool A = IntMap[1] == 1;
  // CHECK-MESSAGES: [[@LINE-1]]:12: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
  bool B = FooMap[1] == Foo{};
  // CHECK-MESSAGES: [[@LINE-1]]:12: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
}

void convertedToBool() {
  if (IntMap[1])
    // CHECK-MESSAGES: [[@LINE-1]]:7: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
    ;
}

void thrown() {
  throw IntMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:9: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
  throw FooMap[1];
  // CHECK-MESSAGES: [[@LINE-1]]:9: warning: do not use operator[] for map lookup [bugprone-map-subscript-operator-lookup]
}

void writeToMap() {
  IntMap[1] = 1;
  FooMap[1] = Foo{};
  FooPtrMap[1] = new Foo;
}

void refFromMap() {
  int &A = IntMap[1];
  Foo &B = FooMap[1];
  FooPtr &C = FooPtrMap[1];
}

void ptrFromMap() {
  int *A = &IntMap[1];
  Foo *B = &FooMap[1];
  FooPtr *C = &FooPtrMap[1];
}

int &returnRefToInt() {
  return IntMap[1];
}

Foo &returnRefToFoo() {
  return FooMap[1];
}

FooPtr &returnRefToFooPtr() {
  return FooPtrMap[1];
}

int *returnPtrToInt() {
  return &IntMap[1];
}

Foo *returnPtrToFoo() {
  return &FooMap[1];
}

FooPtr *returnPtrToFooPtr() {
  return &FooPtrMap[1];
}

void callWriteX() {
  writeInt(IntMap[1]);
  writeFoo(FooMap[1]);
}

void passAddrToFunctionTakingPtr() {
  copyFooPtr(&FooMap[1]);
}

void callMemf() {
  FooMap[1].setter(1);
}

void increment() {
  ++IntMap[1];
  IntMap[1]++;
}

void unused() {
  IntMap[1];
  FooMap[1];
  FooPtrMap[1];
}
