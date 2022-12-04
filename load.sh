
# $1 -> ip
# $2 -> max client
# $3 -> per iteration increment step
# $4 -> think time 
# $5 -> total test time

automated_testing()
{
    echo "inside the function"
    for (( i=100; i<$2; i=i+$3 ));
    do
        echo "iteration with parameter ip=$1, user=$i, thinktime=$4 testtime=$5"
        file="log_${i}_${4}_${5}.csv"
        touch $file
        taskset -c  0,2 ./load_gen localhost $1 $i $4 $5  >> $file    
        echo "saved to file=$file"
    done 

}

ulimit -S -n 20000
make clean
make
rm *.csv
touch "load_gen.csv"
echo "user_count,avg_throughput,avg_response_time" >> load_gen.csv
echo "going to run the server"
taskset -c 1 ./server $1 &
pid=$!
echo "process returned with pid = $pid"
echo "calling testing function"
automated_testing $1 $2 $3 $4 $5
kill -15 $pid
echo "saved to file= load_gen.csv"
