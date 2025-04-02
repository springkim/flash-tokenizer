# FlashTokenizer 使用指南

FlashTokenizer 是一个高性能的 C++ Tokenizer，专为高效的 LLM 推理服务而设计。相比传统的 Tokenizer，它速度更快且准确性同样出色。

---

## 安装方法

可通过 pip 快速安装 FlashTokenizer：

```bash
pip install -U flash-tokenizer
```

或从源代码安装：

```bash
git clone https://github.com/NLPOptimize/flash-tokenizer
cd flash-tokenizer/prj
pip install .
```

### 系统需求
- Windows (AMD64)、MacOS (ARM64)、Ubuntu (x86-64)
- Python 版本：3.8 至 3.13
- 编译器：g++、clang++ 或 MSVC

---

## 快速开始

FlashTokenizer 支持多种预训练模型：

```python
from flash_tokenizer import BertTokenizerFlash

print(*BertTokenizerFlash.get_pretrained(), sep="\n")
```

输出：
```
bert-base-cased
bert-base-uncased
bert-base-chinese
bert-base-multilingual-cased
bert-base-multilingual-uncased
kcbert-base
llmlingua-2-bert-base-multilingual-cased-meetingbank
```

### 文本分词

FlashTokenizer 的用法与 Hugging Face 的 `BertTokenizer` 非常相似：

```python
from flash_tokenizer import BertTokenizerFlash
from transformers import BertTokenizer

titles = [
    '绝不能放弃，世界上没有失败，只有放弃。',
    'is there any doubt about it "None whatsoever"',
    "세상 어떤 짐승이 이를 드러내고 사냥을 해? 약한 짐승이나 몸을 부풀리지, 진짜 짐승은 누구보다 침착하지.",
    'そのように二番目に死を偽装して生き残るようになったイタドリがどうして初めて見る自分をこんなに気遣ってくれるのかと尋ねると「私が大切にする人たちがあなたを大切にするから」と答えては'
]

# 使用词汇表文件加载 Tokenizer
tokenizer = BertTokenizerFlash('vocab.txt', do_lower_case=False, model_max_length=512)

for title in titles:
    tokens = tokenizer.tokenize(title)
    token_ids = tokenizer(title, max_length=512, padding="longest").input_ids[0]
    print(f"Tokens: {tokens}")
    print(f"Token IDs: {token_ids}\n")
```

### 使用预训练的 Tokenizer

你也可以直接加载预训练的 Tokenizer：

```python
from flash_tokenizer import BertTokenizerFlash

tokenizer = BertTokenizerFlash.from_pretrained('bert-base-multilingual-cased')
```

---

## 准确性对比

FlashTokenizer 可以轻松与 Hugging Face 的 Tokenizer 进行准确性对比：

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

## 支持

FlashTokenizer 正在积极维护和优化中。如有任何问题、功能请求或贡献，请访问我们的 [GitHub 仓库](https://github.com/NLPOptimize/flash-tokenizer)。

---

祝你使用 FlashTokenizer 愉快，享受高速高效的分词体验！

