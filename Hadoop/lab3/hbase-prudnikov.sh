
if [ $# -ne 1 ]; then 
	echo "wrong number of arguments!" 1>&2
	exit 1
fi

ifile="$1"
tablename=prudnikov_"${ifile%.*}"
tmpfile=tmp_pk_"$ifile"
hdfstmpfolder="/user/tmp_prudnikov/"
strfile=storefile_"$tmpfile"

#clear old tmp files before load
#----------------------------------------
echo "Delete tmp files, dirs"
rm "$tmpfile" 2>/dev/null
hadoop fs -rm -r "$hdfstmpfolder" 2>/dev/null

#echo "disable '$tablename'" | hbase shell
#echo "drop '$tablename'" | hbase shell
#----------------------------------------

echo "Create tmp file with primary key as seq 1 2 3 ..."
if ! nl "$ifile" > "$tmpfile"; then 
	exit 1
fi

echo "create temp dir $hdfstmpfolder in hadoop fs"
hadoop fs -mkdir "$hdfstmpfolder" 2> /dev/null
hadoop fs ­-chown cloudera "$hdfstmpfolder" 2>/dev/null

echo "Put temp file into hadoop fs temp dir"
if ! hadoop fs -put "$tmpfile" "$hdfstmpfolder""$tmpfile"; then
	exit 1
fi

rm "$tmpfile"

echo "Start job hbase.mapreduce.ImportTsv to create storefile structure"
if ! hbase org.apache.hadoop.hbase.mapreduce.ImportTsv \
-Dimporttsv.columns="HBASE_ROW_KEY,edge:source_node_id,edge:target_node_id,edge:weight" \
-Dimporttsv.bulk.output=""$hdfstmpfolder""$strfile"" \
"$tablename" \
"$hdfstmpfolder""$tmpfile" ; then 

	exit 1
fi
echo "Start job hbase.mapreduce.LoadIncrementalHFiles to put storefile into hbase"
if ! hbase org.apache.hadoop.hbase.mapreduce.LoadIncrementalHFiles "$hdfstmpfolder""$strfile" "$tablename"; then 
	exit 1
fi

echo "Delete created temp dir"
hadoop fs -rm -r "$hdfstmpfolder"

echo "Start job to calculate row count of loaded table"
if ! hbase org.apache.hadoop.hbase.mapreduce.RowCounter "$tablename"; then 
	exit 1
fi

#echo "count '$tablename'" | hbase shell -another way to calculate count of rows


