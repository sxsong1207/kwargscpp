### Segmentation fault

There are several reasons that can cause a segmentation fault. Here are some common reasons:

1. Cycling reference in the data structure. For example, a dict contains itself.

Crashed:
``` python
from_cpp = bind_nanobind.generate_dict()
# add some new key-value pairs to the dict
from_cpp["new_key"] = "new_value"
from_cpp["new_nested"] = {"new_inner_key": 42}
from_cpp["new_vector_val"] = [42, 3.14, "hello", True]

from_cpp["nest_self"] = from_cpp # with this will cause a crash

echo_from_cpp = bind_nanobind.echo_dict(from_cpp)
self.assertEqual(from_cpp, echo_from_cpp)
```

Working:
``` python
from_cpp = bind_nanobind.generate_dict()
# add some new key-value pairs to the dict
from_cpp["new_key"] = "new_value"
from_cpp["new_nested"] = {"new_inner_key": 42}
from_cpp["new_vector_val"] = [42, 3.14, "hello", True]

from_cpp["nest_self"] = from_cpp.copy() # safe

echo_from_cpp = bind_nanobind.echo_dict(from_cpp)
self.assertEqual(from_cpp, echo_from_cpp)
```
