import os

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'

import tensorflow as tf

tf.autograph.set_verbosity(0)
tf.get_logger().setLevel('ERROR')

from tensorflow_text.python.ops.fast_bert_tokenizer import FastBertTokenizer as FastBertTokenizerTF
import tensorflow_text as tf_text
from .config import get_config


class TensorflowBertTokenizerFast:
    def __init__(self, config_dir: str):
        config = get_config(config_dir)
        with open(config.vocab_file, "rt", encoding="utf-8") as f:
            vocab = f.read().strip().splitlines()

        self.CLS = vocab.index("[CLS]")
        self.SEP = vocab.index("[SEP]")
        self.setup = True
        # https://github.com/tensorflow/text/blob/8db6911b75e340c0fcc34e6b583ff37f9556adcc/tensorflow_text/core/pybinds/pywrap_fast_wordpiece_tokenizer_model_builder.cc#L26
        # (vocal, max_bytes_per_token, suffix_indicator, unk_token, no_pretokenization, support_detokenization)
        try:
            self.wordpiece_model_buffer = tf_text.build_fast_wordpiece_model(vocab, config.model_max_length, '##', '[UNK]', False, False)
            # https://www.tensorflow.org/text/api_docs/python/text/FastBertTokenizer
            # self.bert_normalizer_model_buffer = tf_text.build_fast_bert_normalizer_model(False)
            self.model_max_length = config.model_max_length
            self.tokenizer = FastBertTokenizerTF(
                vocab=vocab,
                suffix_indicator='##',
                token_out_type=tf.int32,
                max_bytes_per_word=self.model_max_length,
                fast_wordpiece_model_buffer=self.wordpiece_model_buffer,
                # fast_bert_normalizer_model_buffer=self.bert_normalizer_model_buffer,
                lower_case_nfd_strip_accents=config.do_lower_case
            )
        except Exception as e:
            self.setup = False

    @property
    def name(self):
        return "FastBertTokenizer(Tensorflow)"

    @tf.function
    def _tokenize(self, texts, max_length: -1, padding="max_length"):
        input_ids = self.tokenizer.tokenize(texts)
        batch_size = tf.shape(input_ids)[0]

        cls_tokens = tf.fill([batch_size, 1], self.CLS)
        sep_tokens = tf.fill([batch_size, 1], self.SEP)
        input_ids = tf.concat([cls_tokens, input_ids, sep_tokens], axis=-1)

        input_ids = input_ids[:, :max_length - 2]

        if padding == "max_length":
            input_ids = input_ids.to_tensor(default_value=0, shape=[batch_size, max_length])

        return input_ids

    def __call__(self, text: str, max_length=-1, padding="max_length"):
        if isinstance(text, str):
            text = [text]
        if max_length == -1:
            max_length = self.model_max_length
        token_tensor = self._tokenize(tf.constant(text), max_length, padding)
        return token_tensor.numpy()[0].tolist()
