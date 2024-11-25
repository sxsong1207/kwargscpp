import sys
sys.dont_write_bytecode = True
import unittest
import bind_pybind11


class TestBindNanobind(unittest.TestCase):
    def test_generate_dict(self):
        cxx_dict = bind_pybind11.generate_dict()
        expected_dict = {
            "int": 42,
            "double": 3.14,
            "bool": True,
            "string": "hello",
            "nested": {"inner_key": 42},
            "vector_val": [42, 3.14, "hello", True],
        }
        self.assertEqual(cxx_dict, expected_dict)

    def test_echo_dict(self):
        py_dict = {
            "int": 42,
            "double": 3.14,
            "bool": True,
            "string": "hello",
            "nested": {"inner_key": 42},
            "vector_val": [42, 3.14, "hello", True],
        }
        echoed_dict = bind_pybind11.echo_dict(py_dict)
        self.assertEqual(echoed_dict, py_dict)

    def test_exchange_dict(self):
        from_cpp = bind_pybind11.generate_dict()
        # add some new key-value pairs to the dict
        from_cpp["new_key"] = "new_value"
        from_cpp["new_nested"] = {"new_inner_key": 42}
        from_cpp["new_vector_val"] = [42, 3.14, "hello", True]
        
        from_cpp["nest_self"] = from_cpp.copy()

        echo_from_cpp = bind_pybind11.echo_dict(from_cpp)
        self.assertEqual(from_cpp, echo_from_cpp)


if __name__ == "__main__":
    unittest.main()
