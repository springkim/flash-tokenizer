from flash_tokenizer import BertTokenizerFlash
from transformers import BertTokenizer

titles = ['이렇게 하지만 과학자 연구 결과 스피노 사우루스 4 4족 보행 밝였습니다 이렇게 잘 보셨나 ㅎㅎ 스피노 사우루스 이제 4 4족 보행인 것 ㅎㅎ 아직 증거 부족',
          '인터뷰 단독 ① fc 구척장신 김진경 그동안 칼 갈았다 이번 시즌 시즌엔 우승 인터뷰 ytn',
          '솔직 후기 프라다 테수토 나일론 호보 백 숄더백 리뷰 네이버 블로그',
          '보성 역 위키 백과 우리 모두 모두의 백과 사전',
          '문화재 천적 흰개미 알고 보니 개미 개미가 아니라고 네이버 포스트',
          '부산점 말본 골프 래빗 m 7357 pnk 공용 골프 버킷 햇 신세계적 쇼핑 포털 ssg com',
          '캥거루 몸짱 캥거루 로저 12 12살 12살로 생 생을 마감 마감했다 bbc news 코리아',
          '아이폰 사파리 사파리에서 아이패드 혹은 맥 사파리 사파리의 탭 탭으로 접근 접근하는 방법 mac in june',
          '부동산 개발 협회 중부 지회 출범 초대 지회장 지회장에 우석디앤씨 박해상 회장 매일 경제',
          '아시아나 국내선 비즈니스 vs 이코노미 탑승 후기 feat 무료 무료로 업글 방법',
          '2 2년 만 만에 재현 재현된 푸우 인형 비 日 하뉴 男 첫 그랜드 슬램 종합',
          '안산 맛집 투 파인드 피터 고잔점 네이버 블로그']

vocab_file = "dataset/kcbert_base/vocab_kcbert_base.txt"

tokenizer1 = BertTokenizerFlash(vocab_file, do_lower_case=False)
tokenizer2 = BertTokenizer(vocab_file, do_lower_case=False, clean_up_tokenization_spaces=True)

for title in titles:
    ids1 = tokenizer1(title, max_length=300, padding="longest").input_ids
    ids2 = tokenizer2(title, max_length=300, padding="longest").input_ids
    print(title)
    print(ids1)
    print(ids2)

    tokens = tokenizer1(title, padding="longest", max_length=300, return_tensors="np", truncation=True)
    print(tokens.input_ids[0].tolist())
