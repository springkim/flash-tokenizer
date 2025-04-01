import os
import platform

import os
import platform
import requests
import zipfile
import json


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


def get_cache_dir(app_name='flash_bert_tokenizer'):
    system = platform.system()
    if system == 'Windows':
        local_appdata = os.getenv('LOCALAPPDATA', os.path.expanduser('~\\AppData\\Local'))
        return os.path.join(local_appdata, app_name, 'Cache')
    elif system == 'Darwin':
        return os.path.expanduser(f'~/Library/Caches/{app_name}')
    elif system == 'Linux':
        return os.path.expanduser(f'~/.cache/{app_name}')
    else:
        raise NotImplementedError(f'Unsupported system: {system}')


def download_and_extract_zip(url, app_name='flash_bert_tokenizer'):
    cache_dir_path = get_cache_dir(app_name)
    os.makedirs(cache_dir_path, exist_ok=True)
    zip_path = os.path.join(cache_dir_path, os.path.basename(url))
    if os.path.exists(os.path.splitext(zip_path)[0]):
        return os.path.join(cache_dir_path, os.path.splitext(os.path.basename(url))[0])
    response = requests.get(url)
    response.raise_for_status()
    with open(zip_path, "wb") as f:
        f.write(response.content)

    with zipfile.ZipFile(zip_path, 'r') as zip_ref:
        zip_ref.extractall(cache_dir_path)

    os.remove(zip_path)
    return os.path.join(cache_dir_path, os.path.splitext(os.path.basename(url))[0])


if __name__ == "__main__":
    zip_url = "https://github.com/NLPOptimize/flash-tokenizer/releases/download/pretrained/bert-base-cased.zip"
    extracted_dir = download_and_extract_zip(zip_url)
    print(extracted_dir)
