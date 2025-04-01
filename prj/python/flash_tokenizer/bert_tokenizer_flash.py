from typing import List, Union, Optional, Tuple, Dict
from ._core import FlashBertTokenizer, FlashBertTokenizerBidirectional
from .batch_encoding import BatchEncoding
from .config import get_config, download_and_extract_zip, TokenizerConfig
import numpy as np
from .bert_tokenizer_original import OriginalBertTokenizer, OriginalBidirectionalTokenizer


class BertTokenizerFlash:
    __pretrained = [
        'bert-base-cased',
        'bert-base-uncased',
        'bert-base-chinese',
        'bert-base-multilingual-cased',
        'bert-base-multilingual-uncased',
        'kcbert-base',
        'llmlingua-2-bert-base-multilingual-cased-meetingbank'
    ]

    @staticmethod
    def get_pretrained():
        return BertTokenizerFlash.__pretrained

    @classmethod
    def from_pretrained(cls, pretrained_model_name_or_path: str, **kwargs):
        if pretrained_model_name_or_path in cls.__pretrained:
            zip_url = f"https://github.com/NLPOptimize/flash-tokenizer/releases/download/pretrained/{pretrained_model_name_or_path}.zip"
            extracted_dir = download_and_extract_zip(zip_url)
            config = get_config(extracted_dir)
            return BertTokenizerFlash(config.vocab_file, config.do_lower_case, config.model_max_length, config.tokenize_chinese_chars, bidirectional=False, **kwargs)
        else:
            raise OSError(f'OSError: {pretrained_model_name_or_path} is not a local folder and is not a valid model identifier listed on "https://github.com/NLPOptimize/flash-tokenizer/releases/download/pretrained"')

    def __init__(self, vocab_file, /,
                 do_lower_case=True,
                 model_max_length: int = 512,
                 tokenize_chinese_chars=True,
                 bidirectional=False,
                 **kwargs):
        self.model_max_length = model_max_length
        if bidirectional:
            self.tokenizer = FlashBertTokenizerBidirectional(vocab_file, do_lower_case, model_max_length, tokenize_chinese_chars)
        else:
            self.tokenizer = FlashBertTokenizer(vocab_file, do_lower_case, model_max_length, tokenize_chinese_chars)
        self.__version = self.tokenizer.version()
        self.have_original = False
        if kwargs.get("original", False):
            self.have_original = True
            if bidirectional:
                self.original_tokenizer = OriginalBidirectionalTokenizer(vocab_file, do_lower_case, model_max_length)
            else:
                self.original_tokenizer = OriginalBertTokenizer(vocab_file, do_lower_case, model_max_length)

    @property
    def version(self):
        return self.__version

    def verify(self, text: str, padding: str = "longest", max_length: int = -1) -> Union[Tuple[bool, List[int], List[int]], None]:
        if self.have_original:
            if max_length == -1:
                max_length = self.model_max_length
            ids0: list = self.tokenizer.encode(text, padding, max_length)
            ids1: list = self.original_tokenizer(text, max_length, padding)
            return ids0 == ids1, ids0, ids1
        else:
            ModuleNotFoundError(f'No original_tokenizer. You can use it by specifying original=True when calling the constructor.')

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
