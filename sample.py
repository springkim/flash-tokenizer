from flash_tokenizer import BertTokenizerFlash
from transformers import BertTokenizer

titles = [
    'is there any doubt about it "None whatsoever"',
    "세상 어떤 짐승이 이를 드러내고 사냥을 해? 약한 짐승이나 몸을 부풀리지, 진짜 짐승은 누구보다 침착하지.",
    'そのように二番目に死を偽装して生き残るようになったイタドリがどうして初めて見る自分をこんなに気遣ってくれるのかと尋ねると「私が大切にする人たちがあなたを大切にするから」と答えては'
]

vocab_file = "sample/vocab.txt"

tokenizer1 = BertTokenizerFlash(vocab_file, do_lower_case=False)
tokenizer2 = BertTokenizer(vocab_file, do_lower_case=False)

for title in titles:
    print(title)
    print(tokenizer1.tokenize(title))
    print(tokenizer2.tokenize(title))
    ids1 = tokenizer1(title, max_length=512, padding="longest").input_ids[0]
    ids2 = tokenizer2(title, max_length=512, padding="longest").input_ids
    print(ids1)
    print(ids2)
    print()

