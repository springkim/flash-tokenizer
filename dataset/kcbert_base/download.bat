@echo off

curl -L -o  vocab_kcbert_base.txt https://github.com/NLPOptimize/flash-tokenizer/releases/download/Dataset/vocab_kcbert_base.txt
curl -L -o  text_1M.txt https://github.com/NLPOptimize/flash-tokenizer/releases/download/Dataset/text_1M.txt
curl -L -o  text_1M_gt.txt https://github.com/NLPOptimize/flash-tokenizer/releases/download/Dataset/text_1M_gt.txt
curl -L -o  text_10M.txt https://github.com/NLPOptimize/flash-tokenizer/releases/download/Dataset/text_10M.txt
curl -L -o  text_10M_gt.txt https://github.com/NLPOptimize/flash-tokenizer/releases/download/Dataset/text_10M_gt.txt

echo "Complete!"
pause
