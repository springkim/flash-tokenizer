import time
from tqdm import tqdm

import os
from tabulate import tabulate
from bert_tokenizer import *
import pandas as pd
from enum import Enum
from typing import List, Tuple, Dict, Callable, Any, Union
import subprocess


class Config:
    bert_base_cased = "bert-base-cased"
    bert_base_uncased = "bert-base-uncased"
    bert_base_multilingual_cased = "bert-base-multilingual-cased"
    bert_base_multilingual_uncased = "bert-base-multilingual-uncased"
    bert_base_chinese = "bert-base-chinese"
    kcbert_base = "kcbert-base"
    llmlingua_2_bert_base_multilingual_cased_meetingbank = "llmlingua-2-bert-base-multilingual-cased-meetingbank"


class Data:
    texts_en_all = "texts_en.all.parquet"
    texts_ko_all = "texts_ko.all.parquet"
    texts_cn_all = "texts_cn.all.parquet"
    texts_multilingual_all = "texts_multilingual.all.parquet"


def load_parquet(file_path: str, config: str, ratio: Union[float, None] = None, count: Union[int, None] = None):
    df = pd.read_parquet(file_path,
                         columns=["text", config],
                         engine="pyarrow")
    texts = [row.decode() for row in df["text"]]
    gts = [row.tolist() for row in df[config]]
    if ratio is not None and 0.0 < ratio < 1.0:
        size = int(len(texts) * ratio)
    elif count is not None and count < len(texts):
        size = count
    else:
        size = len(texts)
    return texts[:size], gts[:size]


def single_encode_performance_test(tokenizer: Any, texts: List[str], gts: List[List[int]], show_tqdm: bool = True):
    t_beg = time.time()
    correct = 0
    with tqdm(total=len(texts), desc=f"{tokenizer.name:<{35}}", disable=not show_tqdm, ncols=100) as pbar:
        for text, gt in zip(texts, gts):
            input_ids = tokenizer(text, padding="longest")
            correct += gt == input_ids
            pbar.update(1)
    t_end = time.time()

    accuracy = correct * 100 / len(texts)
    elapsed = t_end - t_beg
    return [tokenizer.name, f'{elapsed:.4f}s', f"{len(texts):,}", f'{accuracy:.4f}%']


if __name__ == '__main__':

    config_path = "../dataset/config/" + Config.bert_base_cased
    dataset_path = "../dataset/data/" + Data.texts_en_all

    print("Initializing tokenizer...")
    tokenizer1 = HuggingFaceBertTokenizerFast(config_path)
    tokenizer2 = PaddleNLPBertTokenizerFast(config_path)
    tokenizers = [tokenizer1, tokenizer2]
    tokenizer3 = TensorflowBertTokenizerFast(config_path)
    if tokenizer3.setup:
        tokenizers.append(tokenizer3)
    if BlingfireBertTokenizer.check(config_path):
        pair_int, bf_unk = ((99, 29894), 100) if "kcbert" in config_path else (None, None)
        tokenizer4 = BlingfireBertTokenizer(config_path, pair_int=pair_int, unk=bf_unk)
        tokenizers.append(tokenizer4)
    tokenizer5 = FlashBertTokenizer(config_path)
    tokenizers.append(tokenizer5)
    print("Loading data...")
    texts, gts = load_parquet(dataset_path, os.path.basename(config_path))

    print("Performance comparisons are conducted using the following tokenizers:")
    for tokenizer in tokenizers:
        print(f'\t{tokenizer.name}')
    print('-' * 30)

    tables = []
    for tokenizer in tokenizers:
        r = single_encode_performance_test(tokenizer, texts, gts)
        tables.append(r)

    headers = ["Tokenizer", "Elapsed Time", "texts", "Accuracy"]
    colalign = ("left", "right", "right", "right")

    s = f'### {os.path.basename(config_path)} ({os.path.basename(dataset_path)})\n'
    s += tabulate(tables, headers=headers, tablefmt="github", colalign=colalign)
    s += '\n\n'

    print(s)

    with open("perftest_history.md", "at", encoding="utf-8") as f:
        f.write(s)
