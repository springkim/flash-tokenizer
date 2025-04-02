# 性能测试

本目录对绑定 Python 接口的 FlashBertTokenizer 与其他几种 BertTokenizer 实现进行了性能和准确性的基准测试。

我们通过 Python 接口，比较了 FlashBertTokenizer 和以下四种 Tokenizer 实现的性能与准确性。

测试使用了在 [DATA.md](../dataset/DATA.md) 中描述的多种配置和 parquet 文件。准确性比较的基准（Ground Truth）由原始的 [Google BertTokenizer](https://github.com/google-research/bert/blob/master/tokenization.py) 生成的输出结果确定。

准确性指标定义为：输出的 Token IDs 与基准结果完全一致的 `input_ids` 所占比例。

### 参与比较的实现：

* [Huggingface 的 BertTokenizerFast](https://huggingface.co/docs/transformers/en/model_doc/bert#transformers.BertTokenizerFast)
  * 最广泛使用的 BertTokenizerFast 的 Rust 实现。

* [PaddleNLP 的 BertTokenizerFast](https://paddlenlp.readthedocs.io/en/stable/_modules/paddlenlp/experimental/faster_tokenizer.html)
  * 完全兼容 Huggingface tokenizer 的 C++ 实现，速度稳定约快 1.2 倍。

* [Tensorflow-text 的 FastBertTokenizer](https://www.tensorflow.org/text/api_docs/python/text/FastBertTokenizer)
  * Tensorflow-text 提供的实现，但由于速度较慢且准确性稍低，并无显著优势。

* [微软的 BlingFire](https://github.com/microsoft/BlingFire)
  * 由微软开发的独特实现，以最快的处理速度著称。

* [guillaume-be 的 rust-tokenizers](https://github.com/guillaume-be/rust-tokenizers)
  * 速度慢于 BertTokenizerFlash 和 Blingfire，但比其他实现更快且准确性更高。

```bash
brew install rust
git clone https://github.com/guillaume-be/rust-tokenizers
cd rust-tokenizers/python-bindings
python setup.py install
```