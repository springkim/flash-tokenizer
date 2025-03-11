## KcBert_base

| Tokenizer             |   Elapsed Time |   titles |   Accuracy |
|-----------------------|----------------|----------|------------|
| FlashBertTokenizer    |         7.9542 |  1000000 |    99.5792 |
| BertTokenizerFast(PP) |        38.3839 |  1000000 |    99.9995 |
| BertTokenizerFast(HF) |        49.0197 |  1000000 |    99.9995 |
| FastBertTokenizer(TF) |       188.633  |  1000000 |    99.9826 |
| Blingfire             |        13.454  |  1000000 |    99.9244 |



| Tokenizer             |   Elapsed Time |   titles |   Accuracy |
|-----------------------|----------------|----------|------------|
| BertTokenizerFlash    |        9.18146 |  1000000 |    99.5792 |
| BertTokenizerFast(PP) |       44.0897  |  1000000 |    99.9995 |
| BertTokenizerFast(HF) |       53.0362  |  1000000 |    99.9995 |
| FastBertTokenizer(TF) |      186.849   |  1000000 |    99.9826 |
| Blingfire             |       14.0538  |  1000000 |    99.9244 |

## DeepCT

| Tokenizer             |   Elapsed Time |   titles |   Accuracy |
|-----------------------|----------------|----------|------------|
| BertTokenizerFlash    |        12.5391 |   404464 |    99.8442 |
| BertTokenizerFast(PP) |        64.6828 |   404464 |    99.8615 |
| BertTokenizerFast(HF) |        69.6647 |   404464 |    99.8615 |
| FastBertTokenizer(TF) |        85.5056 |   404464 |    99.8507 |
| Blingfire             |        12.1941 |   404464 |    96.8979 |