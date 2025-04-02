import blingfire
import os
from .config import get_config


class BlingfireBertTokenizer:
    def __init__(self, config_dir: str, pair_int: tuple[int, int] | None = None, unk: int | None = None):
        config = get_config(config_dir)
        self.tokenizer = blingfire.load_model(config_dir + ".bin")
        with open(config.vocab_file, "rt", encoding="utf-8") as f:
            vocab = f.read().strip().splitlines()
        self.CLS = vocab.index("[CLS]")
        self.SEP = vocab.index("[SEP]")
        self.UNK = vocab.index("[UNK]") if unk is None else unk
        self.model_max_length = config.model_max_length
        self.pair_int = pair_int
        # kcbert-base: (99,29894)

    @classmethod
    def check(cls, config_dir):
        return os.path.exists(config_dir + ".bin")

    @property
    def name(self):
        return "Blingfire(Microsoft)"

    def __call__(self, text: str, max_length=-1, padding="max_length"):
        if max_length == -1:
            max_length = self.model_max_length
        input_ids = blingfire.text_to_ids(self.tokenizer, text, max_len=max_length, unk=self.UNK, no_padding=True)

        if len(input_ids) > max_length - 2:
            input_ids = input_ids[:max_length - 2]

        if self.pair_int is not None:
            mask_positive = input_ids > 0
            mask_negative = input_ids < 0
            input_ids[mask_positive] -= self.pair_int[0]
            input_ids[mask_negative] += self.pair_int[1]

        input_ids = [self.CLS] + input_ids.tolist() + [self.SEP]
        if padding == "max_length":
            input_ids = input_ids + [0] * (max_length - len(input_ids))

        return input_ids

    def free(self):
        if self.tokenizer is not None and blingfire is not None:
            blingfire.free_model(self.tokenizer)
            self.tokenizer = None

    def __del__(self):
        self.free()
