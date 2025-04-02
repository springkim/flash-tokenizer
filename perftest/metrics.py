import numpy as np
from scipy.stats import spearmanr
from scipy.stats import pearsonr


def pearson_correlation(a, b):
    corr, _ = pearsonr(a, b)
    return corr


def spearman_correlation(a, b):
    corr, _ = spearmanr(a, b)
    return corr


def cosine_sim(a, b):
    a = np.array(a)
    b = np.array(b)
    return np.dot(a, b) / (np.linalg.norm(a) * np.linalg.norm(b))


def jaccard_sim(list1, list2):
    set1, set2 = set(list1), set(list2)
    intersection = set1 & set2
    union = set1 | set2
    return len(intersection) / len(union) if union else 0.0
