import pandas as pd

def load_parquet(file_path: str, config: str):
    df = pd.read_parquet(file_path,
                         columns=["text", config],
                         engine="pyarrow")
    text = [row.decode() for row in df["text"]]
    gts = [row.tolist() for row in df[config]]
    return text, gts