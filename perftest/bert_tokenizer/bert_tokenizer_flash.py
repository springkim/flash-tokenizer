from flash_tokenizer import BertTokenizerFlash
from .config import get_config


class FlashBertTokenizer:
    def __init__(self, config_dir: str, bidirectional: bool = False):
        config = get_config(config_dir)
        self.tokenizer = BertTokenizerFlash(config.vocab_file, do_lower_case=config.do_lower_case, model_max_length=config.model_max_length, tokenize_chinese_chars=config.tokenize_chinese_chars
                                            , bidirectional=bidirectional)
        self.model_max_length = config.model_max_length

    def __call__(self, text: str, max_length=-1, padding="max_length"):
        if max_length == -1:
            max_length = self.model_max_length
        tokens = self.tokenizer(text, max_length=max_length, padding=padding,
                                return_attention_mask=False,
                                return_token_type_ids=False,
                                return_tensors=None)
        return tokens.input_ids[0]

    @property
    def name(self):
        return "FlashBertTokenizer"
