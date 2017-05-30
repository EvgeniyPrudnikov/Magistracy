from sklearn.metrics import matthews_corrcoef
import matplotlib.pyplot as plt
import numpy as np
from numba import jit


def mcc(tp, tn, fp, fn):
    sup = tp * tn - fp * fn
    inf = (tp + fp) * (tp + fn) * (tn + fp) * (tn + fn)
    if inf==0:
        return 0
    else:
        return sup / np.sqrt(inf)


def eval_mcc(y_true, y_prob, show=False):
    idx = np.argsort(y_prob)
    y_true_sort = y_true[idx]
    n = y_true.shape[0]
    nump = 1.0 * np.sum(y_true) # number of positive
    numn = n - nump # number of negative
    tp = nump
    tn = 0.0
    fp = numn
    fn = 0.0
    best_mcc = 0.0
    best_id = -1
    prev_proba = -1
    best_proba = -1
    mccs = np.zeros(n)
    for i in range(n):
        # all items with idx < i are predicted negative while others are predicted positive
        # only evaluate mcc when probability changes
        proba = y_prob[idx[i]]
        if proba != prev_proba:
            prev_proba = proba
            new_mcc = mcc(tp, tn, fp, fn)
            if new_mcc >= best_mcc:
                best_mcc = new_mcc
                best_id = i
                best_proba = proba
        mccs[i] = new_mcc
        if y_true_sort[i] == 1:
            tp -= 1.0
            fn += 1.0
        else:
            fp -= 1.0
            tn += 1.0
    if show:
        y_pred = (y_prob >= best_proba).astype(int)
        score = matthews_corrcoef(y_true, y_pred)
        print(score, best_mcc)
        plt.plot(mccs)
        plt.show()
        return best_proba, best_mcc, y_pred
    else:
        return best_mcc

y_prob0 = np.random.rand(1000000)
y_prob = y_prob0 + 0.4 * np.random.rand(1000000) - 0.02


y_true = (y_prob0 > 0.6).astype(int)
best_proba, best_mcc, y_pred = eval_mcc(y_true, y_prob, True)

print best_proba, best_mcc, y_pred

def roundn(yprob, scale):
    return np.around(yprob * scale) / scale

best_proba, best_mcc, y_pred = eval_mcc(y_true, roundn(y_prob, 100), True)