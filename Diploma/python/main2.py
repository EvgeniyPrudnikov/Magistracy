import gc
import numpy as np
import pandas as pd
import xgboost as xgb
from sklearn.metrics import matthews_corrcoef
from sklearn.metrics import precision_recall_curve
from sklearn.metrics import average_precision_score
from operator import itemgetter
import matplotlib.pyplot as plt
from numba import jit
import datetime

DIRECTORY = '../../../diploma_data/'
#TRAIN_FILES = ['tr_cat.csv', 'tr_date.csv', 'tr_num.csv']
TRAIN_FILES = ['train_categorical.csv', 'train_date.csv', 'train_numeric.csv']

def get_date_features():
    train_file = TRAIN_FILES[1]

    for i, chunk in enumerate(pd.read_csv(DIRECTORY + train_file, chunksize=1, low_memory=False)):
        features = list(chunk.columns)
        break

    seen = np.zeros(52)
    rv = []
    for f in features:
        if f == 'Id' or 'S24' in f or 'S25' in f:
            rv.append(f)
            continue

        station = int(f.split('_')[1][1:])

        if seen[station]:
            continue

        seen[station] = 1
        rv.append(f)

    return rv


processed_date_features = get_date_features()


def get_min_max_date():
    train_file = TRAIN_FILES[1]

    features = None
    subset = None

    for i, chunk in enumerate(pd.read_csv(DIRECTORY + train_file, usecols=processed_date_features, chunksize=50000,
                                          low_memory=False)):
        print i

        if features is None:
            features = list(chunk.columns)
            features.remove('Id')

        df_date_chunk = chunk[['Id']].copy()
        df_date_chunk['mindate'] = chunk[features].min(axis=1).values
        df_date_chunk['maxdate'] = chunk[features].max(axis=1).values

        if subset is None:
            subset = df_date_chunk.copy()
        else:
            subset = pd.concat([subset, df_date_chunk])

        del chunk
        gc.collect()

    return subset


def get_min_max_date_df():
    df_date = get_min_max_date()

    df_date.sort_values(by=['mindate', 'Id'], inplace=True)
    df_date['mindate_id_diff'] = df_date.Id.diff()
    dt_id_reverse = np.full_like(df_date.mindate_id_diff.values, np.nan)
    dt_id_reverse[0:-1] = -df_date.mindate_id_diff.values[1:]
    df_date['mindate_id_diff_reverse'] = dt_id_reverse

    df_date.sort_values(by=['maxdate', 'Id'], inplace=True)
    df_date['maxdate_id_diff'] = df_date.Id.diff()
    dt_id_reverse = np.full_like(df_date.maxdate_id_diff.values, np.nan)
    dt_id_reverse[0:-1] = -df_date.maxdate_id_diff.values[1:]
    df_date['maxdate_id_diff_reverse'] = dt_id_reverse

    return df_date


prepared_df_date = get_min_max_date_df()


@jit
def mcc(tp, tn, fp, fn):
    sup = tp * tn - fp * fn
    inf = (tp + fp) * (tp + fn) * (tn + fp) * (tn + fn)
    if inf == 0:
        return 0
    else:
        return sup / np.sqrt(inf)


@jit
def eval_mcc(y_true, y_prob, show=False):
    idx = np.argsort(y_prob)
    y_true_sort = y_true[idx]
    n = y_true.shape[0]
    nump = 1.0 * np.sum(y_true)  # number of positive
    numn = n - nump  # number of negative
    tp = nump
    tn = 0.0
    fp = numn
    fn = 0.0
    best_mcc = 0.0
    best_id = -1
    mccs = np.zeros(n)
    for i in range(n):
        if y_true_sort[i] == 1:
            tp -= 1.0
            fn += 1.0
        else:
            fp -= 1.0
            tn += 1.0
        new_mcc = mcc(tp, tn, fp, fn)
        mccs[i] = new_mcc
        if new_mcc >= best_mcc:
            best_mcc = new_mcc
            best_id = i
    if show:
        best_proba = y_prob[idx[best_id]]
        y_pred = (y_prob > best_proba).astype(int)
        plt.interactive(False)
        plt.plot(mccs)
        plt.show()
        return best_proba, best_mcc, y_pred
    else:
        return best_mcc


def mcc_eval(y_prob, dtrain):
    y_true = dtrain.get_label()
    best_mcc = eval_mcc(y_true, y_prob)
    return 'MCC', best_mcc


def plot_precision_recall(lines):
    plt.clf()

    for precision, recall,avg_pres_score, label in lines:
        plt.plot(recall, precision, label='{0} (area = {1:0.2f}) '.format(label, avg_pres_score))
    plt.xlabel('Recall')
    plt.ylabel('Precision')
    plt.ylim([0.0, 1.05])
    plt.xlim([0.0, 1.0])
    plt.title('Precision-Recall')
    plt.legend(loc="upper right")
    plt.show()


def create_feature_map(features):
    with open('xgb.fmap', 'w') as f:
        for i, feature in enumerate(features):
            f.write('{0}\t{1}\tq\n'.format(i, feature))


def get_importance(gbm, features):
    create_feature_map(features)
    importance = gbm.get_fscore(fmap='xgb.fmap')
    importance = sorted(importance.items(), key=itemgetter(1), reverse=True)
    return importance


def leave_one_out(data1, data2, column_name, use_LOO=False):
    grp_outcomes = data1.groupby(column_name)['Response'].mean().reset_index()
    grp_count = data1.groupby(column_name)['Response'].count().reset_index()
    grp_outcomes['cnt'] = grp_count.Response
    if (use_LOO):
        grp_outcomes = grp_outcomes[grp_outcomes.cnt > 1]
    else:
        grp_outcomes = grp_outcomes[grp_outcomes.cnt >= 10]
    grp_outcomes.drop('cnt', inplace=True, axis=1)
    outcomes = data2['Response'].values
    x = pd.merge(data2[[column_name, 'Response']], grp_outcomes,
                 suffixes=('x_', ''),
                 how='left',
                 on=column_name,
                 left_index=True)['Response']
    if (use_LOO):
        x = ((x * x.shape[0]) - outcomes) / (x.shape[0] - 1)
    return x.fillna(x.mean())


def data_transformation():
    cols = [['Id',
             'L1_S24_F1559', 'L3_S32_F3851',
             'L1_S24_F1827', 'L1_S24_F1582',
             'L3_S32_F3854', 'L1_S24_F1510',
             'L1_S24_F1525'],
            ['Id',
             'L3_S30_D3496', 'L3_S30_D3506',
             'L3_S30_D3501', 'L3_S30_D3516',
             'L3_S30_D3511'],
            ['Id',
             'L1_S24_F1846', 'L3_S32_F3850',
             'L1_S24_F1695', 'L1_S24_F1632',
             'L3_S33_F3855', 'L1_S24_F1604',
             'L3_S29_F3407', 'L3_S33_F3865',
             'L3_S38_F3952', 'L1_S24_F1723',
             'Response']]
    train_data = None

    for k, f in enumerate(TRAIN_FILES):
        print f
        subset = None
        for i, chunk in enumerate(pd.read_csv(DIRECTORY + f,
                                              usecols=cols[k],
                                              chunksize=50000,
                                              low_memory=False)):
            print i
            if subset is None:
                subset = chunk.copy()
            else:
                subset = pd.concat([subset, chunk])
            del chunk
            gc.collect()
        if train_data is None:
            train_data = subset.copy()
        else:
            train_data = pd.merge(train_data, subset.copy(), on="Id")
        del subset
        gc.collect()

    train_data = train_data.merge(prepared_df_date, on='Id')

    visible_train_data = train_data[::2]
    print visible_train_data.columns

    blind_train_data = train_data[1::2]
    print blind_train_data.columns

    for i in xrange(2):
        for col in cols[i][1:]:
            print col
            blind_train_data.loc[:, col] = leave_one_out(visible_train_data, blind_train_data, col, True).values
    del visible_train_data
    gc.collect()
    return blind_train_data


def train_model():
    train_data = data_transformation()
    print 'train size:', train_data.shape
    features = list(train_data.columns)
    features.remove('Response')
    features.remove('Id')
    print features
    num_rounds = 50
    params = {
        'objective': "binary:logistic"
        , 'eta': 0.021
        , 'max_depth': 7
        , 'colsample_bytree': 0.82
        , 'min_child_weight': 3
        , 'base_score': 0.005
        , 'silent': True
        # ,'scale_pos_weight': 0.06 - test
    }
    print 'Fitting'
    train_predictions = None

    dvisible_train = xgb.DMatrix(train_data[features], train_data.Response, silent=True)

    folds = 1
    for i in range(folds):
        print 'Fold:', i
        params['seed'] = i
        watchlist = [(dvisible_train, 'train'), (dvisible_train, 'val')]
        clf = xgb.train(params, dvisible_train,
                        num_boost_round=num_rounds,
                        evals=watchlist,
                        early_stopping_rounds=20,
                        feval=mcc_eval,
                        maximize=True
                        )
        limit = clf.best_iteration + 1

        predictions = clf.predict(dvisible_train, ntree_limit=limit)

        best_proba, best_mcc, y_pred = eval_mcc(train_data.Response, predictions, True)
        print 'tree limit:', limit
        print 'mcc:', best_mcc
        print matthews_corrcoef(train_data.Response, y_pred)
        if train_predictions is None:
            train_predictions = predictions
        else:
            train_predictions += predictions

        imp = get_importance(clf, features)
        print 'Importance array: ', imp

    best_proba, best_mcc, y_pred = eval_mcc(train_data.Response, train_predictions / folds, True)

    print best_proba, best_mcc
    y_pred = (train_predictions / folds > 0.08).astype(int)

    precision, recall, threshold = precision_recall_curve(train_data.Response, y_pred)
    avg_pres_score = average_precision_score(train_data.Response, y_pred)
    plot_precision_recall([(precision, recall, avg_pres_score, 'pr')])
    print matthews_corrcoef(train_data.Response, y_pred)


print 'started - ' + str(datetime.datetime.now())
train_model()
print 'finished - ' + str(datetime.datetime.now())
