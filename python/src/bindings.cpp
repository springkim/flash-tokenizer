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
           FlashBertTokenizerBidirectional
    )pbdoc";

    py::class_<FlashBertTokenizer>(m, "FlashBertTokenizer")
            .def(py::init<const std::string &, bool, const int>(),
                 py::arg("vocab_file"),
                 py::arg("do_lower_case"),
                 py::arg("model_max_length"))
            .def("version", &FlashBertTokenizer::version)
            .def("encode",
                 [](FlashBertTokenizer &self,
                    py::str text,
                    py::str padding,
                    int max_length) -> std::vector<int> {
                     std::string text_str = text.cast<std::string>();
                     std::string padding_str = padding.cast<std::string>();
                     return self.encode(text_str, padding_str, max_length);
                 },
                 py::arg("text"),
                 py::arg("padding") = "max_length",
                 py::arg("max_length") = -1)
            .def("batch_encode",
                 [](FlashBertTokenizer &self,
                    const std::vector<std::string> &texts,
                    py::str padding,
                    int max_length, bool parallel) -> std::vector<std::vector<int> > {
                     std::string padding_str = padding.cast<std::string>();
                     return self.batch_encode(texts, padding_str, max_length,parallel);
                 },
                 py::arg("texts"),
                 py::arg("padding") = "max_length",
                 py::arg("max_length") = -1,
                 py::arg("parallel") = true,
                 "Encode a batch of texts into token IDs. Returns a list of token ID lists.");

    py::class_<FlashBertTokenizerBidirectional, FlashBertTokenizer>(m, "FlashBertTokenizerBidirectional")
            .def(py::init<const std::string &, bool,const int>(),
                 py::arg("vocab_file"),
                 py::arg("do_lower_case"),
                 py::arg("model_max_length"))
            .def("encode",
                 [](FlashBertTokenizerBidirectional &self,
                    py::str text,
                    py::str padding,
                    int max_length) -> std::vector<int> {
                     std::string text_str = text.cast<std::string>();
                     std::string padding_str = padding.cast<std::string>();
                     return self.encode(text_str, padding_str, max_length);
                 },
                 py::arg("text"),
                 py::arg("padding") = "max_length",
                 py::arg("max_length") = -1)
            .def("batch_encode",
                 [](FlashBertTokenizer &self,
                    const std::vector<std::string> &texts,
                    py::str padding,
                    int max_length, bool parallel) -> std::vector<std::vector<int> > {
                     std::string padding_str = padding.cast<std::string>();
                     return self.batch_encode(texts, padding_str, max_length, parallel);
                 },
                 py::arg("texts"),
                 py::arg("padding") = "max_length",
                 py::arg("max_length") = -1,
                 py::arg("parallel") = true,
                 "Encode a batch of texts into token IDs. Returns a list of token ID lists.");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
