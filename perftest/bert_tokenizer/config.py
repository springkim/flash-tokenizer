import json
import os


class TokenizerConfig:
    def __init__(self, **kwargs):
        self.data = kwargs
        for k, v in kwargs.items():
            setattr(self, k, v)

    def __str__(self):
        return json.dumps(self.data, indent=2, ensure_ascii=False)


def get_config(dirname: str):
    file = os.path.join(dirname, "tokenizer_config.json")
    vocab_file = os.path.abspath(os.path.join(dirname, "vocab.txt"))
    if os.path.exists(file):
        with open(file, 'r', encoding='utf-8') as f:
            d = json.load(f)
    else:
        d = {}

    do_lower_case = d.get('do_lower_case', False)
    model_max_length = d.get('model_max_length', 512)
    tokenize_chinese_chars = d.get('tokenize_chinese_chars', True)
    clean_up_tokenization_spaces = d.get('clean_up_tokenization_spaces', True)

    return TokenizerConfig(do_lower_case=do_lower_case,
                           model_max_length=model_max_length,
                           tokenize_chinese_chars=tokenize_chinese_chars,
                           clean_up_tokenization_spaces=clean_up_tokenization_spaces,
                           vocab_file=vocab_file)
