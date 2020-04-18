.. title:: clang-tidy - bugprone-map-subscript-operator-lookup

bugprone-map-subscript-operator-lookup
======================================

Finds map lookups done with ``operator[]``.

Using ``std::map::operator[]`` for finding values from the map is a common
source of bugs in C++ programs. The operator's side effect of possibly inserting
a value to the map can easily cause surprises. The usage of the operator can be
replaced with ``at()`` or ``find()`` in cases where it's used to only look up
values from the map with no intention of modifying it.

This check warns when the operator is used for map lookup:

.. code-block:: c++

  std::map<int, int> m;
  const auto i = m[1]; // warning

The check only warns about *lookups* done with ``operator[]``. Modifying the map
using it is still ok:

.. code-block:: c++

  std::map<int, int> m;
  m[1] = 2; // ok

In general, doing any mutating operation through the reference returned by the
operator is allowed by this check. If the reference is used as
reference-to-const, a warning is given:

.. code-block:: c++

  std::map<int, Foo> m;
  m[1].setter(1); // ok
  const auto a = m[1].getter(); // warning

Options
-------

.. option:: MapTypes

   Semicolon-separated list of map types to check. Defaults to
   ``::std::map;::std::unordered_map``.
