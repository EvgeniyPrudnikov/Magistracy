from sklearn.model_selection import train_test_split
from sklearn.model_selection import cross_val_score
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import average_precision_score
from sklearn.metrics import precision_recall_curve
from sklearn.metrics.scorer import make_scorer
from sklearn.metrics import matthews_corrcoef
from operator import itemgetter
import matplotlib.pyplot as plt
from numba import jit
import xgboost as xgb
import pandas as pd
import numpy as np
import datetime
import gc

DIRECTORY = '../../../diploma_data/'
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


useful_date_features = get_date_features()


def get_min_max_date():
    train_file = TRAIN_FILES[1]

    features = None
    subset = None

    for i, chunk in enumerate(pd.read_csv(DIRECTORY + train_file, usecols=useful_date_features, chunksize=50000,
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
    if isinstance(dtrain, np.ndarray):
        y_true = dtrain
        best_mcc = eval_mcc(y_true, y_prob)
        return best_mcc
    else:
        y_true = dtrain.get_label()
        best_mcc = eval_mcc(y_true, y_prob)
        return 'MCC', best_mcc


def plot_precision_recall(lines, file_name):
    plt.clf()

    for precision, recall, avg_pres_score, label in lines:
        plt.plot(recall, precision, label='{0} (area = {1:0.2f}) '.format(label, avg_pres_score))
    plt.xlabel('Recall')
    plt.ylabel('Precision')
    plt.ylim([0.0, 1.05])
    plt.xlim([0.0, 1.0])
    plt.title('Precision-Recall')
    plt.legend(loc="upper right")
    #plt.show()
    plt.savefig(file_name, format='png')


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
    # as most useful features with lowest correlation between themselves and max correlation with target variable
    cols = [['Id',
             'L1_S24_F1559', 'L3_S32_F3851','L1_S24_F1827', 'L1_S24_F1582',
             'L3_S32_F3854', 'L1_S24_F1510','L1_S24_F1525'],
            ['Id',
             'L3_S30_D3496', 'L3_S30_D3506',
             'L3_S30_D3501', 'L3_S30_D3516',
             'L3_S30_D3511'
             ],
            ['Id',
             'L0_S0_F12', 'L0_S1_F24', 'L0_S2_F32', 'L0_S3_F92',
             'L0_S4_F104', 'L0_S5_F114', 'L0_S6_F118', 'L0_S7_F136',
             'L0_S8_F149', 'L0_S9_F180', 'L0_S10_F264','L0_S11_F306',
             'L0_S12_F346', 'L0_S13_F354', 'L0_S14_F366', 'L0_S15_F406',
             'L0_S16_F421', 'L0_S17_F431', 'L0_S18_F439', 'L0_S19_F453',
             'L0_S20_F466', 'L0_S21_F482', 'L0_S22_F586', 'L0_S23_F635',
             'L1_S24_F1421', 'L1_S25_F2677', 'L2_S26_F3106', 'L2_S27_F3214',
             'L2_S28_F3292', 'L3_S29_F3430', 'L3_S30_F3624', 'L3_S31_F3842',
             'L3_S32_F3850', 'L3_S33_F3857', 'L3_S34_F3880', 'L3_S35_F3889',
             'L3_S36_F3918', 'L3_S37_F3950', 'L3_S38_F3952', 'L3_S39_F3972',
             'L3_S40_F3982', 'L3_S41_F4026', 'L3_S43_F4090', 'L3_S44_F4112',
             'L3_S45_F4124', 'L3_S47_F4148', 'L3_S48_F4193', 'L3_S49_F4236',
             'L3_S50_F4243', 'L3_S51_F4262',
             'Response']]
    train_data = None

    for k, f in enumerate(TRAIN_FILES):
        print f
        subset = None
        for i, chunk in enumerate(pd.read_csv(DIRECTORY + f, usecols=cols[k], chunksize=50000, low_memory=False)):
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


def get_train_test_data():
    train_data = data_transformation()
    print 'train size:', train_data.shape
    features = list(train_data.columns)
    features.remove('Response')
    features.remove('Id')
    X_train, X_test, y_train, y_test = train_test_split(train_data[features], train_data.Response, test_size=0.33,
                                                        random_state=42)
    return features, X_train, X_test, y_train, y_test


def train_xgb_model(features, X_train, X_test, y_train, y_test):
    num_rounds = 50
    params = {
        'objective': "binary:logistic"
        , 'eta': 0.021
        , 'max_depth': 7
        , 'colsample_bytree': 0.82
        , 'min_child_weight': 3
        , 'base_score': 0.005
        , 'silent': True
    }
    print 'Fitting XGBoost'
    train_predictions = None

    X_train = xgb.DMatrix(X_train[features], y_train, silent=True)
    X_test = xgb.DMatrix(X_test[features], y_test, silent=True)

    folds = 5
    for i in range(folds):
        print 'Fold:', i
        params['seed'] = i
        watchlist = [(X_train, 'train'), (X_test, 'val')]
        clf = xgb.train(params, X_train,
                        num_boost_round=num_rounds,
                        evals=watchlist,
                        early_stopping_rounds=20,
                        feval=mcc_eval,
                        maximize=True
                        )
        limit = clf.best_iteration + 1

        predictions = clf.predict(X_test, ntree_limit=limit)

        best_mcc= eval_mcc(y_test, predictions)
        print 'tree limit:', limit
        print 'mcc:', best_mcc

        if train_predictions is None:
            train_predictions = predictions
        else:
            train_predictions += predictions

        imp = get_importance(clf, features)
        print 'Importance array: ', imp

    best_proba, best_mcc, y_pred = eval_mcc(y_test, train_predictions / folds, True)
    print best_proba, best_mcc
    y_pred = (train_predictions / folds > 0.2365).astype(int)

    precision, recall, threshold = precision_recall_curve(y_test, y_pred)
    avg_pres_score = average_precision_score(y_test, y_pred)
    plot_precision_recall([(precision, recall, avg_pres_score, 'pr')],'pr_xgb.png')
    print matthews_corrcoef(y_test, y_pred)


def train_rnf_model(features, X_train, X_test, y_train, y_test):
    params = {
        'max_depth': 7
        , 'min_weight_fraction_leaf': 0
        , 'n_jobs': 3
        , 'n_estimators': 400
        , 'max_features': None
    }
    print 'Fitting Random Forest'
    train_predictions = None

    X_train = X_train.fillna(999999)
    X_test = X_test.fillna(999999)

    folds = 5
    for i in range(folds):
        print 'Fold:', i
        params['random_state'] = i

        mcc_eval_scorer = make_scorer(mcc_eval, greater_is_better=True)

        clf = RandomForestClassifier(**params)

        clf.fit(X_train, y_train)

        print 'score: ', mcc_eval_scorer(clf, X_train, y_train)

        for l, s in enumerate(cross_val_score(clf, X_train, y_train, scoring=mcc_eval_scorer, cv=5, n_jobs=3)):
            print '[{0}] train-MCC:{1}'.format(l, s)

        predictions = clf.predict(X_test)

        if train_predictions is None:
            train_predictions = predictions
        else:
            train_predictions += predictions

    best_proba, best_mcc, y_pred = eval_mcc(y_test, train_predictions / folds, True)
    print 'best_proba, best_mcc : ', best_proba, best_mcc
    y_pred = (train_predictions / folds > 0.2365).astype(int)
    precision, recall, threshold = precision_recall_curve(y_test, y_pred)
    avg_pres_score = average_precision_score(y_test, y_pred)
    plot_precision_recall([(precision, recall, avg_pres_score, 'pr')], 'pr_rnf.png')
    print matthews_corrcoef(y_test, y_pred)


print 'started - ' + str(datetime.datetime.now())

features, X_train, X_test, y_train, y_test = get_train_test_data()

train_xgb_model(features, X_train, X_test, y_train, y_test)
gc.collect()
train_rnf_model(features, X_train, X_test, y_train, y_test)
gc.collect()

print 'finished - ' + str(datetime.datetime.now())
