# Usage Guide for FlashTokenizer

FlashTokenizer is a high-performance tokenizer implemented in C++ for efficient LLM inference. It's designed to be significantly faster and equally accurate compared to traditional tokenizers.

---

## Installation

Install FlashTokenizer easily using pip:

```bash
pip install -U flash-tokenizer
```

Or from source:

```bash
git clone https://github.com/NLPOptimize/flash-tokenizer
cd flash-tokenizer/prj
pip install .
```

### Prerequisites
- Windows (AMD64), MacOS (ARM64), Ubuntu (x86-64)
- Python 3.8 to 3.13
- g++, clang++, or MSVC

---

## Quick Start

FlashTokenizer supports various pretrained models:

```python
from flash_tokenizer import BertTokenizerFlash

print(*BertTokenizerFlash.get_pretrained(), sep="\n")
```

Output:
```
bert-base-cased
bert-base-uncased
bert-base-chinese
bert-base-multilingual-cased
bert-base-multilingual-uncased
kcbert-base
llmlingua-2-bert-base-multilingual-cased-meetingbank
```

### Tokenizing Text

FlashTokenizer usage aligns closely with Hugging Face's `BertTokenizer`:

```python
from flash_tokenizer import BertTokenizerFlash
from transformers import BertTokenizer

titles = [
    'is there any doubt about it "None whatsoever"',
    "세상 어떤 짐승이 이를 드러내고 사냥을 해? 약한 짐승이나 몸을 부풀리지, 진짜 짐승은 누구보다 침착하지.",
    'そのように二番目に死を偽装して生き残るようになったイタドリがどうして初めて見る自分をこんなに気遣ってくれるのかと尋ねると「私が大切にする人たちがあなたを大切にするから」と答えては'
]

# Load tokenizer with vocab file
tokenizer = BertTokenizerFlash('vocab.txt', do_lower_case=False, model_max_length=512)

for title in titles:
    tokens = tokenizer.tokenize(title)
    token_ids = tokenizer(title, max_length=512, padding="longest").input_ids[0]
    print(f"Tokens: {tokens}")
    print(f"Token IDs: {token_ids}\n")
```

### Using Pretrained Tokenizers

You can also directly load pretrained tokenizers:

```python
from flash_tokenizer import BertTokenizerFlash

tokenizer = BertTokenizerFlash.from_pretrained('bert-base-multilingual-cased')
```

---

## Comparing Accuracy

FlashTokenizer allows easy accuracy comparison with Hugging Face's tokenizer:

```python
from flash_tokenizer import BertTokenizerFlash
from transformers import BertTokenizer

texts = ["Chess is Life.", "Dies Spiel ist ein Probierstein des Gehirns."]

flash_tokenizer = BertTokenizerFlash.from_pretrained('bert-base-multilingual-uncased', original=True)
hf_tokenizer = BertTokenizer.from_pretrained('bert-base-multilingual-uncased')

correct = 0
for text in texts:
    flash_ids = flash_tokenizer(text, max_length=512).input_ids[0]
    hf_ids = hf_tokenizer(text, max_length=512, return_tensors="np").input_ids[0].tolist()
    correct += int(flash_ids == hf_ids)

accuracy = correct * 100 / len(texts)
print(f"Accuracy: {accuracy:.2f}%")
```

---

## Support

FlashTokenizer is actively maintained and optimized. For issues, feature requests, or contributions, visit our [GitHub repository](https://github.com/NLPOptimize/flash-tokenizer).

---

Enjoy the fast and efficient tokenization with FlashTokenizer!

