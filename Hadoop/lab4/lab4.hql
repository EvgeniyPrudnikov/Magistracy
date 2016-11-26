lab 4 hadoop


# cоздаем таблицу для файла биграмм
CREATE TABLE default.2_gram_pr(n_gram STRING,year SMALLINT,occurrences BIGINT,books BIGINT) ROW FORMAT DELIMITED FIELDS TERMINATED BY '\t' LINES TERMINATED BY '\n' STORED AS TEXTFILE;

# загружаем в неё файл из файловой системы
LOAD DATA LOCAL INPATH '/media/sf_sf_hadoop1/googlebooks-eng-all-2gram-20120701-pr' INTO TABLE default.2_gram_pr;

# селект для задания 

SELECT n_gram, SUM(occurrences)/SUM(books) AS occ_per_book 
FROM 2_gram_pr 
GROUP BY n_gram
ORDER BY occ_per_book DESC, n_gram ASC
LIMIT 10;