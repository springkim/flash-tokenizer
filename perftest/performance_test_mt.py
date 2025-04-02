import logging

logging.disable(logging.WARNING)
import time
from tqdm import tqdm

import os
from tabulate import tabulate
from bert_tokenizer import *
import pandas as pd
from typing import List, Tuple, Dict, Callable, Any, Union

import threading
import time
from rich.progress import Progress, TextColumn, BarColumn, TaskProgressColumn, TimeRemainingColumn
import queue
import argparse


# logging.disable(logging.INFO)


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


def single_encode_performance_test(progress, task_id, tokenizer: Any, texts: List[str], gts: List[List[int]], result_queue):
    t_beg = time.time()
    correct = 0

    for text, gt in zip(texts, gts):
        input_ids = tokenizer(text, padding="longest")
        correct += gt == input_ids
        progress.update(task_id, advance=1)
    t_end = time.time()

    accuracy = correct * 100 / len(texts)
    elapsed = t_end - t_beg
    item = [f'{tokenizer.name}', f'{elapsed:.4f}s', f"{len(texts):,}", f'{accuracy:.4f}%']
    result_queue[task_id] = item


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='BertTokenizer performance test', add_help=False)

    parser.add_argument('-c', '--config', type=str, default=Config.bert_base_cased, required=True)
    parser.add_argument('-d', '--dataset', type=str, default=Data.texts_en_all, required=True)
    parser.add_argument('-s', '--size', type=int, default=100000, required=False)
    args = parser.parse_args()

    # config_path = "../dataset/config/" + Config.bert_base_multilingual_cased
    # dataset_path = "../dataset/data/" + Data.texts_multilingual_all

    config_path = "../dataset/config/" + args.config
    dataset_path = "../dataset/data/" + args.dataset
    size = args.size

    logging.info("Initializing tokenizer...")
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

    tokenizer6 = RustBertTokenizer(config_path)
    tokenizers.append(tokenizer6)
    tokenizers.append(tokenizer5)

    logging.info("Loading data...")
    texts, gts = load_parquet(dataset_path, os.path.basename(config_path), count=size)

    logging.info("Performance comparisons are conducted using the following tokenizers:")
    for tokenizer in tokenizers:
        logging.info(f'\t{tokenizer.name}')
    logging.info('-' * 30)

    tasks = []
    for tokenizer in reversed(tokenizers):
        tasks.append((tokenizer.name, tokenizer, texts, gts))
    result_queue = [None] * len(tokenizers)

    with Progress(
            TextColumn("[bold blue]{task.description}"),
            BarColumn(),
            TaskProgressColumn(),
            TimeRemainingColumn()
    ) as progress:
        threads = []
        task_ids = []

        for task in tasks:
            task_id = progress.add_task(f"[red]{task[0]}", total=len(texts))
            task_ids.append(task_id)
            t = threading.Thread(target=single_encode_performance_test, args=(progress, task_id, task[1], task[2], task[3], result_queue))
            threads.append(t)
        print(f'{os.path.basename(config_path)}, texts: {len(texts)}({os.path.basename(dataset_path).split('.')[0]})')

        for t in threads:
            t.start()
        for t in threads:
            t.join()

    headers = ["Tokenizer", "Elapsed Time", "texts", "Accuracy"]
    colalign = ("left", "right", "right", "right")

    tables = result_queue
    tables.sort(key=lambda x: float(x[1][:-1]))
    s = tabulate(tables, headers=headers, tablefmt="rounded_outline", colalign=colalign)

    print(s)
