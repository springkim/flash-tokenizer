import numpy as np
from collections import UserDict


# https://github.com/huggingface/transformers/blob/main/src/transformers/tokenization_utils_base.py#L193
class BatchEncoding(UserDict):
    def __init__(self, input_ids: list[list[int]] | np.ndarray, dtype=np.int32,
                 return_attention_mask: bool = True, return_token_type_ids: bool = True):
        super().__init__()
        self.input_ids = input_ids
        self.__setitem__("input_ids", self.input_ids)
        self.return_attention_mask = return_attention_mask
        self.return_token_type_ids = return_token_type_ids
        if isinstance(input_ids, np.ndarray):
            if self.return_attention_mask:
                self.attention_mask = (input_ids != 0).astype(dtype)
            if self.return_token_type_ids:
                self.token_type_ids = np.zeros_like(input_ids, dtype=dtype)
        else:
            if self.return_attention_mask:
                self.attention_mask = [[1 if token != 0 else 0 for token in ids] for ids in input_ids]
            if self.return_token_type_ids:
                self.token_type_ids = [[0 for _ in ids] for ids in input_ids]
        if self.return_attention_mask:
            self.__setitem__("attention_mask", self.attention_mask)
        if self.return_token_type_ids:
            self.__setitem__("token_type_ids", self.token_type_ids)

    def to(self, func: callable):
        self.input_ids = func(self.input_ids)
        if self.return_attention_mask:
            self.attention_mask = func(self.attention_mask)
        if self.return_token_type_ids:
            self.token_type_ids = func(self.token_type_ids)
        return self

    def __setitem__(self, key, value):
        super().__setitem__(key, value)
