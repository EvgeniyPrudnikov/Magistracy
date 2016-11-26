# ------------------------------------------------------------------------
# lab 4 HIVE Prudnikov
# ------------------------------------------------------------------------

# ------------------------------------------------------------------------
# Cоздаем таблицу для файла биграмм
# ------------------------------------------------------------------------

CREATE TABLE default.2_gram_pr(n_gram STRING, year SMALLINT, occurrences BIGINT, books BIGINT) ROW FORMAT DELIMITED FIELDS TERMINATED BY '\t' LINES TERMINATED BY '\n' STORED AS TEXTFILE;

# ------------------------------------------------------------------------
# Загружаем в неё файл из файловой системы
# ------------------------------------------------------------------------

LOAD DATA LOCAL INPATH '/media/sf_sf_hadoop1/googlebooks-eng-all-2gram-20120701-pr' INTO TABLE default.2_gram_pr;

# ------------------------------------------------------------------------
# Селект для задания 
# ------------------------------------------------------------------------

SELECT n_gram, SUM(occurrences)/SUM(books) AS occ_per_book 
FROM 2_gram_pr 
GROUP BY n_gram
ORDER BY occ_per_book DESC, n_gram ASC
LIMIT 10;

# ------------------------------------------------------------------------
# Результат
# ------------------------------------------------------------------------

Project Processing_VERB	482.2878787878788
Project_NOUN Processing_VERB	482.2878787878788
Project Processing	255.42168674698794
Project_NOUN Processing	255.42168674698794
printout submissions	242.61702127659575
printout submissions_NOUN	242.61702127659575
printout_NOUN submissions	242.5531914893617
printout_NOUN submissions_NOUN	242.5531914893617
principal sudder	220.0
principal sudder_NOUN	219.9108910891089


Time taken: 3315.135 seconds, Fetched: 10 row(s)

# ------------------------------------------------------------------------
