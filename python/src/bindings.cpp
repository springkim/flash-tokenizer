#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "bert_tokenizer.h"

namespace py = pybind11;

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

PYBIND11_MODULE(_core, m) {
m.doc() = R"pbdoc(
        FlashBertTokenizer Python bindings
        ---------------------------------

        .. currentmodule:: flash_tokenizer

        .. autosummary::
           :toctree: _generate

           FlashBertTokenizer
    )pbdoc";

py::class_<FlashBertTokenizer>(m, "FlashBertTokenizer")
.def(py::init<const std::string &, bool, int>(),
        py::arg("vocab_file"),
        py::arg("do_lower_case") = true,
py::arg("max_input_chars_per_word") = 256)
.def("tokenize", [](FlashBertTokenizer &self, py::str text, int max_length) -> std::vector<std::string> {
std::string text_str = py::str(text).cast<std::string>();
return self.tokenize(text_str, max_length);
}, py::arg("text"), py::arg("max_length") = -1)
.def("convert_tokens_to_ids", &FlashBertTokenizer::convert_tokens_to_ids)
.def("convert_ids_to_tokens", &FlashBertTokenizer::convert_ids_to_tokens)
.def("__call__",
[](FlashBertTokenizer &self,
        py::str text,
py::str padding,
int max_length,
        py::str return_tensors,
bool truncation) -> std::vector<int> {
std::string text_str = text.cast<std::string>();
std::string padding_str = padding.cast<std::string>();
std::string return_tensors_str = return_tensors.cast<std::string>();
return self(text_str, padding_str, max_length, return_tensors_str, truncation);
},
py::arg("text"),
py::arg("padding") = "longest",
py::arg("max_length") = -1,
py::arg("return_tensors") = "np",
py::arg("truncation") = true);

#ifdef VERSION_INFO
m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
m.attr("__version__") = "dev";
#endif
}