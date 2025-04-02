import pandas as pd

df = pd.read_parquet("../dataset/data/texts_multilingual.all.parquet", engine="pyarrow")

print(*df.columns.tolist(), sep="\n")

"""
bert-base-cased
bert-base-chinese
bert-base-multilingual-cased
bert-base-multilingual-uncased
bert-base-uncased
kcbert-base
llmlingua-2-bert-base-multilingual-cased-meetingbank
text
"""
