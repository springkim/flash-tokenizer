from typing import List, Union, Optional
from ._core import FlashBertTokenizer, FlashBertTokenizerBidirectional
from .batch_encoding import BatchEncoding
import numpy as np


class BertTokenizerFlash:

    def __init__(self, vocab_file,
                 do_lower_case=True,
                 model_max_length: int = 512,
                 tokenize_chinese_chars=True,
                 bidirectional=False,
                 **kwargs):
        if bidirectional:
            self.tokenizer = FlashBertTokenizerBidirectional(vocab_file, do_lower_case, model_max_length, tokenize_chinese_chars)
        else:
            self.tokenizer = FlashBertTokenizer(vocab_file, do_lower_case, model_max_length, tokenize_chinese_chars)
        self.__version = self.tokenizer.version()
        pass

    @property
    def version(self):
        return self.__version;

    def __call__(self,
                 text: Union[List[str], str],
                 padding: str = "max_length",
                 max_length: Optional[int] = None,
                 add_special_tokens: bool = True,
                 return_tensors: Optional[str] = None,
                 return_token_type_ids: bool = True,
                 return_attention_mask: bool = True,
                 do_multiprocess=True):
        if isinstance(text, str):
            input_ids = [self.tokenizer.encode(text, padding, max_length)]
        elif isinstance(text, list):
            input_ids = self.tokenizer.batch_encode(text, padding, max_length, do_multiprocess)
        else:
            raise ValueError("text must be str or list[str].")
        if return_tensors == "np":
            input_ids = np.asarray(input_ids)

        return BatchEncoding(input_ids, return_attention_mask=return_attention_mask,
                             return_token_type_ids=return_token_type_ids)

    def tokenize(self, text: str):
        return self.tokenizer.tokenize(text)
