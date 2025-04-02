# flash-tokenizer/dataset

## config（配置目录）

每个配置文件夹中都包含了对应的 `vocab.txt` 和 `tokenizer_config.json` 文件。此外，config 目录中还包含了用于 blingfire 的模型文件。

## data（数据目录）

运行 `download.bat` 或 `download.sh` 后，会下载 4 个 parquet 数据文件。

每个 parquet 文件存储了原始文本（text）以及使用 [Google 官方 BertTokenizer](https://github.com/google-research/bert/blob/master/tokenization.py) 针对该文本生成的 token IDs。具体包含以下几种配置：

* `text`（原始文本）
* `bert-base-cased`
* `bert-base-chinese`
* `bert-base-multilingual-cased`
* `bert-base-multilingual-uncased`
* `bert-base-uncased`
* `kcbert-base`
* `llmlingua-2-bert-base-multilingual-cased-meetingbank`