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