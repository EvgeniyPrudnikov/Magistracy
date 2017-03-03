def matthews_corr_coef(df,label_col,prediction_col): 
    import math
    tp = df[(df[label_col] == 1) & (df[prediction_col] == 1)].count()
    tn = df[(df[label_col] == 0) & (df[prediction_col] == 0)].count()
    fp = df[(df[label_col] == 0) & (df[prediction_col] == 1)].count()
    fn = df[(df[label_col] == 1) & (df[prediction_col] == 0)].count()
    print "True Positives:", tp
    print "True Negatives:", tn
    print "False Positives:", fp
    print "False Negatives:", fn
    #print "Total", df.count()
    try:
        MCC = ((tp * tn) - (fp * fn)) / math.sqrt((tp + fp)*(tp + fn)*(tn + fp)*(tn + fn))
    except ZeroDivisionError:
        MCC = -1
    print "Matthews correlation coefficient: ", MCC
